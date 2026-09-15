# The whole application lives in main.cpp. It is the same file the Arduino
# sketch includes, so there is exactly one copy of the source.
idf_component_register(
    SRCS "main.cpp"
    INCLUDE_DIRS "."
)
