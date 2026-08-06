#pragma once

#include <cstdint>

#include "game/game_2048.h"
#include "hardware/sticky_display.h"

class GameView {
public:
    explicit GameView(StickyDisplay &display);

    void draw(const Game2048 &game,
              uint32_t best_score,
              const char *status_message,
              int battery_percent,
              bool charging);
    static bool is_new_game_button(uint16_t x, uint16_t y);
    void draw_shutdown();

private:
    void draw_text(int x, int y, const char *text, int scale);
    void draw_centered_text(int center_x, int y, const char *text, int scale);
    void draw_status(const char *message, int battery_percent, bool charging);
    void draw_board(const Game2048 &game);

    StickyDisplay &display_;
};
