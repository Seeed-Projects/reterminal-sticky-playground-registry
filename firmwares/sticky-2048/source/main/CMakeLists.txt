idf_component_register(
    SRCS "main.cpp"
         "game/game_2048.cpp"
         "game/game_save_state.cpp"
         "hardware/sticky_ai_button.cpp"
         "hardware/sticky_buzzer.cpp"
         "hardware/sticky_display.cpp"
         "hardware/sticky_status.cpp"
         "hardware/sticky_touch.cpp"
         "ui/game_view.cpp"
    INCLUDE_DIRS "."
    REQUIRES bq27220 button esp_driver_gpio esp_driver_i2c esp_driver_ledc esp_driver_spi esp_hw_support esp_rom esp_timer gt911 nvs_flash seeed_epaper
)
