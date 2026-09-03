set(PRIVREQ esp_timer)
if("${IDF_VERSION_MAJOR}.${IDF_VERSION_MINOR}" VERSION_GREATER_EQUAL "5.3")
    set(REQ esp_driver_gpio)
else()
    set(REQ driver)
endif()
set(SRC_FILES "button_gpio.c" "iot_button.c" "button_matrix.c")

if("${IDF_VERSION_MAJOR}.${IDF_VERSION_MINOR}" VERSION_GREATER_EQUAL "5.0")
    list(APPEND REQ esp_adc)
    if(CONFIG_SOC_ADC_SUPPORTED)
        list(APPEND SRC_FILES "button_adc.c")
    endif()
endif()

idf_component_register(SRCS ${SRC_FILES}
                        INCLUDE_DIRS include interface
                        REQUIRES ${REQ}
                        PRIV_REQUIRES ${PRIVREQ})

target_compile_definitions(${COMPONENT_LIB} PRIVATE
    BUTTON_VER_MAJOR=4
    BUTTON_VER_MINOR=1
    BUTTON_VER_PATCH=6
)
