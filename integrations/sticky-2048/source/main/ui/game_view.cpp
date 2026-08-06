#include "ui/game_view.h"

#include <cstdio>
#include <cstring>

#include "ui/font_5x7.h"

namespace {
constexpr int kBoardX = 12;
constexpr int kBoardY = 145;
constexpr int kBoardSize = 456;
constexpr int kCellSize = kBoardSize / 4;
constexpr int kNewGameX = 90;
constexpr int kNewGameY = 700;
constexpr int kNewGameWidth = 300;
constexpr int kNewGameHeight = 60;

int text_width(const char *text, int scale)
{
    const int length = static_cast<int>(std::strlen(text));
    return length == 0 ? 0 : length * 6 * scale - scale;
}
} // namespace

GameView::GameView(StickyDisplay &display) : display_(display)
{
}

bool GameView::is_new_game_button(uint16_t x, uint16_t y)
{
    return x >= kNewGameX && x < kNewGameX + kNewGameWidth &&
           y >= kNewGameY && y < kNewGameY + kNewGameHeight;
}

void GameView::draw(const Game2048 &game,
                    uint32_t best_score,
                    const char *status_message,
                    int battery_percent,
                    bool charging)
{
    display_.clear();

    draw_status(status_message, battery_percent, charging);

    draw_centered_text(StickyDisplay::kWidth / 2, 54, "Sticky 2048", 4);

    display_.draw_rect(16, 96, 210, 38);
    display_.draw_rect(254, 96, 210, 38);
    char score_text[24] = {};
    char best_text[24] = {};
    std::snprintf(score_text,
                  sizeof(score_text),
                  "SCORE %lu",
                  static_cast<unsigned long>(game.score()));
    std::snprintf(best_text,
                  sizeof(best_text),
                  "BEST %lu",
                  static_cast<unsigned long>(best_score));
    draw_centered_text(121, 108, score_text, 2);
    draw_centered_text(359, 108, best_text, 2);

    draw_board(game);

    display_.draw_rect(0, 614, StickyDisplay::kWidth, 1);
    draw_centered_text(StickyDisplay::kWidth / 2,
                       642,
                       game.is_game_over() ? "GAME OVER" : "SWIPE TO MOVE",
                       3);
    display_.draw_rect(kNewGameX, kNewGameY, kNewGameWidth, kNewGameHeight);
    draw_centered_text(StickyDisplay::kWidth / 2, 719, "NEW GAME", 3);
}

void GameView::draw_status(const char *message, int battery_percent, bool charging)
{
    display_.draw_rect(0, 43, StickyDisplay::kWidth, 1);
    draw_text(12, 15, message, 2);

    constexpr int kBatteryX = 370;
    constexpr int kBatteryY = 12;
    constexpr int kBatteryWidth = 32;
    constexpr int kBatteryHeight = 20;
    display_.draw_rect(kBatteryX, kBatteryY, kBatteryWidth, kBatteryHeight);
    display_.draw_rect(kBatteryX + kBatteryWidth, kBatteryY + 6, 4, 8);

    if (charging) {
        static constexpr uint8_t kBolt[13] = {
            0x06, 0x06, 0x0C, 0x0C, 0x18, 0x1F, 0x0E,
            0x06, 0x06, 0x0C, 0x0C, 0x08, 0x08,
        };
        for (int row = 0; row < 13; ++row) {
            for (int column = 0; column < 5; ++column) {
                if ((kBolt[row] & (1U << (4 - column))) != 0) {
                    display_.draw_pixel(357 + column, 15 + row);
                }
            }
        }
    }

    if (battery_percent >= 0) {
        const int fill_width = battery_percent * (kBatteryWidth - 6) / 100;
        for (int y = kBatteryY + 3; y < kBatteryY + kBatteryHeight - 3; ++y) {
            for (int x = kBatteryX + 3; x < kBatteryX + 3 + fill_width; ++x) {
                display_.draw_pixel(x, y);
            }
        }
    }

    char percent_text[16] = "--%";
    if (battery_percent >= 0) {
        std::snprintf(percent_text, sizeof(percent_text), "%d%%", battery_percent);
    }
    draw_text(416, 15, percent_text, 2);
}

void GameView::draw_shutdown()
{
    display_.clear();
    draw_centered_text(StickyDisplay::kWidth / 2, 300, "Sticky 2048", 4);
    draw_centered_text(StickyDisplay::kWidth / 2, 380, "Press AI button to start", 2);
}

void GameView::draw_text(int x, int y, const char *text, int scale)
{
    for (const char *character = text; *character != '\0'; ++character) {
        const uint8_t *rows = Font5x7::rows(*character);
        for (int row = 0; row < 7; ++row) {
            for (int column = 0; column < 5; ++column) {
                if ((rows[row] & (1U << (4 - column))) == 0) {
                    continue;
                }
                for (int dy = 0; dy < scale; ++dy) {
                    for (int dx = 0; dx < scale; ++dx) {
                        display_.draw_pixel(x + column * scale + dx,
                                            y + row * scale + dy);
                    }
                }
            }
        }
        x += 6 * scale;
    }
}

void GameView::draw_centered_text(int center_x, int y, const char *text, int scale)
{
    draw_text(center_x - text_width(text, scale) / 2, y, text, scale);
}

void GameView::draw_board(const Game2048 &game)
{
    display_.draw_rect(kBoardX, kBoardY, kBoardSize, kBoardSize);

    for (int index = 1; index < 4; ++index) {
        const int offset = index * kCellSize;
        display_.draw_rect(kBoardX + offset, kBoardY, 1, kBoardSize);
        display_.draw_rect(kBoardX, kBoardY + offset, kBoardSize, 1);
    }

    for (int row = 0; row < 4; ++row) {
        for (int column = 0; column < 4; ++column) {
            const uint16_t value = game.tile(row, column);
            if (value == 0) {
                continue;
            }

            char number[6] = {};
            std::snprintf(number, sizeof(number), "%u", value);
            const int center_x = kBoardX + column * kCellSize + kCellSize / 2;
            const int center_y = kBoardY + row * kCellSize + kCellSize / 2;
            const std::size_t digits = std::strlen(number);
            const int number_scale = digits <= 2 ? 7 : (digits == 3 ? 5 : (digits == 4 ? 4 : 3));
            draw_centered_text(center_x,
                               center_y - (7 * number_scale) / 2,
                               number,
                               number_scale);
        }
    }
}
