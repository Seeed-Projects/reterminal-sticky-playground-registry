#pragma once

#include <cstdint>
#include <functional>

#include "driver/i2c_master.h"

#define GT911_ADDR1 0x5D
#define GT911_ADDR2 0x14

#define GT911_REG_COMMAND 0x8040
#define GT911_REG_CONFIG_START 0x8047
#define GT911_REG_ID 0x8140
#define GT911_REG_COORD_RES 0x8146
#define GT911_REG_STATUS 0x814E
#define GT911_REG_POINTS 0x8150

struct GTPoint {
    uint16_t x;
    uint16_t y;
    uint16_t size;
    uint8_t id;
};

using TouchCallback = std::function<void(int8_t, const GTPoint*)>;

class GT911 {
public:
    static constexpr uint8_t kMaxTouchPoints = 5;

    GT911() = default;
    ~GT911();

    bool begin(int intPin, int rstPin, uint16_t width, uint16_t height, i2c_master_bus_handle_t i2c_bus);

    void onTouch(TouchCallback callback);
    void loop();

    bool is_available();
    int8_t read_points(GTPoint* points, uint8_t maxPoints = kMaxTouchPoints);

    void setRotation(uint8_t rotation);
    void readResolution(uint16_t& maxX, uint16_t& maxY);

    bool isReady() const { return ready_; }
    uint8_t address() const { return addr_; }

private:
    bool attachDevice(uint8_t address);
    void detachDevice();
    bool resetForAddress(uint8_t address);
    bool reset();
    bool clearStatus();

    bool writeRegister(uint16_t reg, uint8_t val);
    bool readRegister(uint16_t reg, uint8_t& val);
    bool readRegisters(uint16_t reg, uint8_t* buffer, uint8_t len);

    static uint16_t mapCoord(uint16_t value, uint16_t inMax, uint16_t outMax);

private:
    int intPin_ = -1;
    int rstPin_ = -1;

    uint16_t width_ = 0;
    uint16_t height_ = 0;

    uint16_t maxXSensor_ = 2048;
    uint16_t maxYSensor_ = 2048;

    uint8_t addr_ = GT911_ADDR1;
    uint8_t rotation_ = 0;

    i2c_master_bus_handle_t i2c_bus_ = nullptr;
    i2c_master_dev_handle_t i2c_dev_ = nullptr;
    TouchCallback callback_ = nullptr;

    bool ready_ = false;
    bool statusCached_ = false;
    uint8_t cachedStatus_ = 0;
};
