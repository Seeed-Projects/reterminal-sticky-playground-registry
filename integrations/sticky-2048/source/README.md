# Firmware source

Place the complete ESP-IDF project in this directory. Include `CMakeLists.txt`,
`sdkconfig.defaults`, application code, local components, dependency metadata,
and the applicable license. Keep `CONFIG_APP_REPRODUCIBLE_BUILD=y` in
`sdkconfig.defaults` so CI can reproduce the packaged firmware exactly.
