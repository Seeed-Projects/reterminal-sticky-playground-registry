idf_component_register(
    SRCS "gt911.cpp"
    INCLUDE_DIRS "include"
    REQUIRES esp_driver_i2c esp_driver_gpio esp_timer debug_logging
)
