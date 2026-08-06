#include <cstdint>

#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_random.h"
#include "esp_rom_sys.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "game/game_2048.h"
#include "game/game_save_state.h"
#include "hardware/sticky_ai_button.h"
#include "hardware/sticky_display.h"
#include "hardware/sticky_status.h"
#include "hardware/sticky_touch.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "pin_config.h"
#include "ui/game_view.h"

namespace {
constexpr const char *kTag = "main";
constexpr uint8_t kFullRefreshInterval = 20;
constexpr uint8_t kGameSaveInterval = 20;
constexpr int64_t kBatteryPollIntervalUs = 10LL * 1000 * 1000;
constexpr const char *kNvsNamespace = "sticky2048";
constexpr const char *kBestScoreKey = "best";

bool init_storage()
{
    const esp_err_t err = nvs_flash_init();
    if (err != ESP_OK) {
        ESP_LOGW(kTag, "NVS initialization failed: %s", esp_err_to_name(err));
        return false;
    }
    return true;
}

uint32_t load_best_score()
{
    nvs_handle_t handle = 0;
    esp_err_t err = nvs_open(kNvsNamespace, NVS_READONLY, &handle);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        return 0;
    }
    if (err != ESP_OK) {
        ESP_LOGW(kTag, "Best Score open failed: %s", esp_err_to_name(err));
        return 0;
    }

    uint32_t best_score = 0;
    err = nvs_get_u32(handle, kBestScoreKey, &best_score);
    nvs_close(handle);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGW(kTag, "Best Score read failed: %s", esp_err_to_name(err));
        return 0;
    }
    return best_score;
}

bool save_best_score(uint32_t best_score)
{
    nvs_handle_t handle = 0;
    esp_err_t err = nvs_open(kNvsNamespace, NVS_READWRITE, &handle);
    if (err == ESP_OK) {
        err = nvs_set_u32(handle, kBestScoreKey, best_score);
    }
    if (err == ESP_OK) {
        err = nvs_commit(handle);
    }
    if (handle != 0) {
        nvs_close(handle);
    }
    if (err != ESP_OK) {
        ESP_LOGW(kTag, "Best Score save failed: %s", esp_err_to_name(err));
        return false;
    }
    return true;
}

bool to_move_direction(TouchEventType event, MoveDirection &move)
{
    switch (event) {
    case TouchEventType::SwipeUp: move = MoveDirection::Up; return true;
    case TouchEventType::SwipeDown: move = MoveDirection::Down; return true;
    case TouchEventType::SwipeLeft: move = MoveDirection::Left; return true;
    case TouchEventType::SwipeRight: move = MoveDirection::Right; return true;
    case TouchEventType::None:
    case TouchEventType::Tap:
        return false;
    }
    return false;
}

void power_on_hold()
{
    gpio_config_t config = {};
    config.pin_bit_mask = (1ULL << PIN_POWER_HOLD) | (1ULL << PIN_POWER_LOCK);
    config.mode = GPIO_MODE_OUTPUT;
    gpio_config(&config);
    gpio_hold_dis(static_cast<gpio_num_t>(PIN_POWER_HOLD));
    gpio_set_level(static_cast<gpio_num_t>(PIN_POWER_HOLD), 1);
    gpio_set_level(static_cast<gpio_num_t>(PIN_POWER_LOCK), 0);
    esp_rom_delay_us(10);
    gpio_set_level(static_cast<gpio_num_t>(PIN_POWER_LOCK), 1);
    esp_rom_delay_us(10);
    gpio_set_level(static_cast<gpio_num_t>(PIN_POWER_LOCK), 0);
}

void power_off()
{
    gpio_hold_dis(static_cast<gpio_num_t>(PIN_POWER_HOLD));
    gpio_set_level(static_cast<gpio_num_t>(PIN_POWER_HOLD), 0);
    gpio_set_level(static_cast<gpio_num_t>(PIN_POWER_LOCK), 0);
    esp_rom_delay_us(10);
    gpio_set_level(static_cast<gpio_num_t>(PIN_POWER_LOCK), 1);
    esp_rom_delay_us(10);
    gpio_set_level(static_cast<gpio_num_t>(PIN_POWER_LOCK), 0);
}
} // namespace

extern "C" void app_main(void)
{
    ESP_LOGI(kTag, "reTerminal Sticky static 2048 UI");
    power_on_hold();
    vTaskDelay(pdMS_TO_TICKS(100));

    static StickyDisplay display;
    const bool display_ready = display.init();
    static Game2048 game;
    const bool storage_ready = init_storage();
    uint32_t best_score = storage_ready ? load_best_score() : 0;
    const bool game_restored = storage_ready &&
        game_load_state(game, esp_random(), best_score);
    if (!game_restored) {
        game.reset(esp_random());
    }
    GameView view(display);
    uint8_t valid_moves_since_full_refresh = 0;
    uint8_t valid_moves_since_save = 0;
    static StickyTouch touch;
    static StickyStatus status;
    status.init();
    status.update();
    (void)status.poll_battery_percent();
    (void)status.poll_external_power();
    int64_t last_battery_poll_us = esp_timer_get_time();

    auto start_new_game = [&]() {
        game.reset(esp_random());
        valid_moves_since_full_refresh = 0;
        valid_moves_since_save = 0;
        status.update();
        last_battery_poll_us = esp_timer_get_time();
        ESP_LOGI(kTag, "New Game: score reset, best=%lu",
                 static_cast<unsigned long>(best_score));
        if (display_ready) {
            view.draw(game,
                      best_score,
                      status.message(),
                      status.battery_percent(),
                      status.is_charging());
            display.refresh_full();
        }
    };

    if (!display_ready) {
        ESP_LOGE(kTag, "Display initialization failed");
    } else {
        view.draw(game,
                  best_score,
                  status.message(),
                  status.battery_percent(),
                  status.is_charging());
        if (display.refresh_full()) {
            ESP_LOGI(kTag, "Initial 2048 board ready");
        }
    }

    const bool touch_ready = touch.init();
    if (touch_ready) {
        ESP_LOGI(kTag, "Swipe calibration ready");
    } else {
        ESP_LOGE(kTag, "Touch initialization failed");
    }

    static StickyAiButton ai_button;
    const bool ai_button_ready = ai_button.init();
    if (!ai_button_ready) {
        ESP_LOGE(kTag, "AI button initialization failed");
    }

    while (true) {
        if (ai_button_ready) {
            const AiButtonEvent button_event = ai_button.poll();
            if (button_event == AiButtonEvent::ShortPress) {
                start_new_game();
            } else if (button_event == AiButtonEvent::LongPress) {
                ESP_LOGI(kTag, "Power off requested");
                if (storage_ready) {
                    (void)game_save_state(game, best_score);
                    valid_moves_since_save = 0;
                }
                if (display_ready) {
                    view.draw_shutdown();
                    display.refresh_full();
                }

                while (ai_button.is_pressed()) {
                    vTaskDelay(pdMS_TO_TICKS(20));
                }
                vTaskDelay(pdMS_TO_TICKS(100));
                power_off();

                while (true) {
                    vTaskDelay(portMAX_DELAY);
                }
            }
        }

        if (touch_ready) {
            const TouchEvent event = touch.poll();
            if (event.type == TouchEventType::Tap &&
                GameView::is_new_game_button(event.x, event.y)) {
                start_new_game();
            }

            MoveDirection direction = MoveDirection::Up;
            if (to_move_direction(event.type, direction)) {
                const MoveResult result = game.move(direction);
                if (result.changed) {
                    ++valid_moves_since_full_refresh;
                    ++valid_moves_since_save;
                    if (game.score() > best_score) {
                        best_score = game.score();
                        if (storage_ready) {
                            (void)save_best_score(best_score);
                        }
                    }
                    status.update();
                    ESP_LOGI(kTag,
                             "Move accepted: score_delta=%lu score=%lu",
                             static_cast<unsigned long>(result.score_delta),
                             static_cast<unsigned long>(game.score()));
                    if (display_ready) {
                        view.draw(game,
                                  best_score,
                                  status.message(),
                                  status.battery_percent(),
                                  status.is_charging());
                        const bool needs_full_refresh = result.game_over ||
                            valid_moves_since_full_refresh >= kFullRefreshInterval;
                        if (needs_full_refresh) {
                            if (display.refresh_full()) {
                                valid_moves_since_full_refresh = 0;
                            }
                        } else {
                            display.refresh_partial();
                        }
                    }
                    if (valid_moves_since_save >= kGameSaveInterval) {
                        if (storage_ready) {
                            (void)game_save_state(game, best_score);
                        }
                        valid_moves_since_save = 0;
                    }
                } else {
                    ESP_LOGI(kTag, "Move ignored: board unchanged");
                }
            }
        }

        bool status_changed = status.poll_external_power();
        if (status_changed) {
            ESP_LOGI(kTag,
                     "External power changed: charging=%s",
                     status.is_charging() ? "true" : "false");
        }

        const int64_t now_us = esp_timer_get_time();
        if (now_us - last_battery_poll_us >= kBatteryPollIntervalUs) {
            last_battery_poll_us = now_us;
            if (status.poll_battery_percent()) {
                status_changed = true;
                ESP_LOGI(kTag,
                         "Battery changed: %d%%",
                         status.battery_percent());
            }
        }
        if (status_changed && display_ready) {
            view.draw(game,
                      best_score,
                      status.message(),
                      status.battery_percent(),
                      status.is_charging());
            display.refresh_partial();
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}
