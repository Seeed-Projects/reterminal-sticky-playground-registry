#include "gt911.h"

#include <algorithm>
#include <array>
#include <utility>

#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "app_debug_log.h"

namespace {
constexpr const char* kTag = "GT911";
constexpr uint32_t kI2CFreqHz = 100000;
constexpr int kI2CTimeoutMs = 20;
constexpr uint32_t kPostResetSettleMs = 30;
constexpr uint32_t kResetHoldLowMs = 20;
constexpr uint32_t kResetReleaseHighMs = 20;
constexpr uint32_t kResetAddressSettleMs = 80;
constexpr uint32_t kResetRetryDelayMs = 20;
constexpr int kResetRetryCount = 3;

uint64_t monotonic_ms()
{
    return static_cast<uint64_t>(esp_timer_get_time() / 1000ULL);
}
}

GT911::~GT911()
{
    detachDevice();
}

bool GT911::begin(int intPin, int rstPin, uint16_t width, uint16_t height, i2c_master_bus_handle_t i2c_bus)
{
    if(intPin < 0 || rstPin < 0 || i2c_bus == nullptr)
    {
        APP_TOUCH_DEBUG_LOGW(kTag,
                 "begin rejected: int=%d rst=%d bus=%p map=%ux%u",
                 intPin,
                 rstPin,
                 static_cast<void*>(i2c_bus),
                 static_cast<unsigned>(width),
                 static_cast<unsigned>(height));
        return false;
    }

    const uint64_t start_ms = monotonic_ms();
    APP_TOUCH_DEBUG_LOGI(kTag,
             "begin: int=%d rst=%d map=%ux%u bus=%p",
             intPin,
             rstPin,
             static_cast<unsigned>(width),
             static_cast<unsigned>(height),
             static_cast<void*>(i2c_bus));

    intPin_ = intPin;
    rstPin_ = rstPin;
    width_ = width;
    height_ = height;
    i2c_bus_ = i2c_bus;

    rotation_ = 0;
    statusCached_ = false;
    cachedStatus_ = 0;
    ready_ = false;

    gpio_set_direction(static_cast<gpio_num_t>(intPin_), GPIO_MODE_OUTPUT);
    gpio_set_direction(static_cast<gpio_num_t>(rstPin_), GPIO_MODE_OUTPUT);

    bool reset_ok = false;
    for(int attempt = 0; attempt < kResetRetryCount; ++attempt)
    {
        APP_TOUCH_DEBUG_LOGI(kTag,
                 "reset attempt %d/%d",
                 attempt + 1,
                 kResetRetryCount);
        if(reset())
        {
            reset_ok = true;
            break;
        }

        APP_TOUCH_DEBUG_LOGW(kTag,
                 "reset attempt %d/%d failed",
                 attempt + 1,
                 kResetRetryCount);
        vTaskDelay(pdMS_TO_TICKS(kResetRetryDelayMs));
    }

    if(!reset_ok)
    {
        APP_TOUCH_DEBUG_LOGW(kTag,
                 "begin failed: reset/probe did not find GT911 after %d attempt(s)",
                 kResetRetryCount);
        return false;
    }

    vTaskDelay(pdMS_TO_TICKS(kPostResetSettleMs));

    uint16_t maxX = 0;
    uint16_t maxY = 0;
    readResolution(maxX, maxY);
    if(!clearStatus())
    {
        APP_TOUCH_DEBUG_LOGW(kTag, "begin: clear status failed after reset");
    }

    ready_ = true;
    APP_TOUCH_DEBUG_LOGI(kTag,
             "begin ok: addr=0x%02X sensor=%ux%u map=%ux%u elapsed=%llums",
             addr_,
             static_cast<unsigned>(maxXSensor_),
             static_cast<unsigned>(maxYSensor_),
             static_cast<unsigned>(width_),
             static_cast<unsigned>(height_),
             static_cast<unsigned long long>(monotonic_ms() - start_ms));
    return true;
}

void GT911::onTouch(TouchCallback callback)
{
    callback_ = std::move(callback);
}

void GT911::setRotation(uint8_t rotation)
{
    rotation_ = static_cast<uint8_t>(rotation & 0x03U);
}

void GT911::readResolution(uint16_t& maxX, uint16_t& maxY)
{
    uint8_t buf[4] = {};
    if(readRegisters(GT911_REG_COORD_RES, buf, sizeof(buf)))
    {
        uint16_t x = static_cast<uint16_t>((static_cast<uint16_t>(buf[1]) << 8) | buf[0]);
        uint16_t y = static_cast<uint16_t>((static_cast<uint16_t>(buf[3]) << 8) | buf[2]);
        if(x != 0)
        {
            maxXSensor_ = x;
        }
        if(y != 0)
        {
            maxYSensor_ = y;
        }
        APP_TOUCH_DEBUG_LOGI(kTag,
                 "resolution register: raw=%02X %02X %02X %02X sensor=%ux%u",
                 buf[0],
                 buf[1],
                 buf[2],
                 buf[3],
                 static_cast<unsigned>(maxXSensor_),
                 static_cast<unsigned>(maxYSensor_));
    }
    else
    {
        APP_TOUCH_DEBUG_LOGW(kTag,
                 "resolution register read failed, keep sensor=%ux%u",
                 static_cast<unsigned>(maxXSensor_),
                 static_cast<unsigned>(maxYSensor_));
    }

    maxX = maxXSensor_;
    maxY = maxYSensor_;
}

bool GT911::is_available()
{
    if(i2c_dev_ == nullptr)
    {
        APP_TOUCH_DEBUG_LOGW(kTag, "is_available: i2c device is null");
        return false;
    }

    if(statusCached_)
    {
        const uint8_t cachedCount = cachedStatus_ & 0x0FU;
        return ((cachedStatus_ & 0x80U) != 0U) && (cachedCount > 0U) && (cachedCount <= kMaxTouchPoints);
    }

    uint8_t status = 0;
    if(!readRegister(GT911_REG_STATUS, status))
    {
        APP_TOUCH_DEBUG_LOGW(kTag, "is_available: status read failed addr=0x%02X", addr_);
        return false;
    }

    APP_TOUCH_DEBUG_LOGI(kTag,
             "status read: addr=0x%02X status=0x%02X ready=%u count=%u",
             addr_,
             status,
             static_cast<unsigned>((status & 0x80U) != 0U),
             static_cast<unsigned>(status & 0x0FU));

    if((status & 0x80U) == 0U)
    {
        return false;
    }

    const uint8_t touchCount = status & 0x0FU;
    if(touchCount == 0U || touchCount > kMaxTouchPoints)
    {
        APP_TOUCH_DEBUG_LOGW(kTag,
                 "status invalid touch count: status=0x%02X count=%u",
                 status,
                 static_cast<unsigned>(touchCount));
        (void)clearStatus();
        return false;
    }

    cachedStatus_ = status;
    statusCached_ = true;
    return true;
}

int8_t GT911::read_points(GTPoint* points, uint8_t maxPoints)
{
    if(i2c_dev_ == nullptr)
    {
        APP_TOUCH_DEBUG_LOGW(kTag, "read_points: i2c device is null");
        return -1;
    }

    uint8_t status = 0;
    const bool used_cached_status = statusCached_;
    if(statusCached_)
    {
        status = cachedStatus_;
        statusCached_ = false;
        cachedStatus_ = 0;
    }
    else
    {
        if(!readRegister(GT911_REG_STATUS, status))
        {
            APP_TOUCH_DEBUG_LOGW(kTag, "read_points: status read failed addr=0x%02X", addr_);
            return -1;
        }
    }

    APP_TOUCH_DEBUG_LOGI(kTag,
             "read_points: addr=0x%02X status=0x%02X cached=%u ready=%u count=%u max=%u",
             addr_,
             status,
             static_cast<unsigned>(used_cached_status),
             static_cast<unsigned>((status & 0x80U) != 0U),
             static_cast<unsigned>(status & 0x0FU),
             static_cast<unsigned>(maxPoints));

    if((status & 0x80U) == 0U)
    {
        return 0;
    }

    const uint8_t touchCount = status & 0x0FU;
    if(touchCount == 0U || touchCount > kMaxTouchPoints)
    {
        APP_TOUCH_DEBUG_LOGW(kTag,
                 "read_points: invalid touch count, status=0x%02X count=%u",
                 status,
                 static_cast<unsigned>(touchCount));
        (void)clearStatus();
        return 0;
    }

    std::array<uint8_t, kMaxTouchPoints * 8> rawData = {};
    const uint8_t readLength = static_cast<uint8_t>(touchCount * 8U);
    if(!readRegisters(GT911_REG_POINTS, rawData.data(), readLength))
    {
        APP_TOUCH_DEBUG_LOGW(kTag,
                 "read_points: point data read failed addr=0x%02X len=%u",
                 addr_,
                 static_cast<unsigned>(readLength));
        (void)clearStatus();
        return -1;
    }

    const uint8_t outCount = std::min(touchCount, maxPoints);
    if(points != nullptr)
    {
        for(uint8_t i = 0; i < outCount; ++i)
        {
            const uint8_t* ptData = &rawData[static_cast<size_t>(i) * 8U];

            const uint16_t x = static_cast<uint16_t>(ptData[0] | (static_cast<uint16_t>(ptData[1]) << 8));
            const uint16_t y = static_cast<uint16_t>(ptData[2] | (static_cast<uint16_t>(ptData[3]) << 8));
            const uint16_t size = static_cast<uint16_t>(ptData[4] | (static_cast<uint16_t>(ptData[5]) << 8));
            const uint8_t id = ptData[7];

            const uint16_t mapX = mapCoord(x, maxXSensor_, width_);
            const uint16_t mapY = mapCoord(y, maxYSensor_, height_);

            uint16_t finalX = mapX;
            uint16_t finalY = mapY;
            switch(rotation_)
            {
                case 1:
                    finalX = mapY;
                    finalY = (mapX > width_) ? 0U : static_cast<uint16_t>(width_ - mapX);
                    break;
                case 2:
                    finalX = (mapX > width_) ? 0U : static_cast<uint16_t>(width_ - mapX);
                    finalY = (mapY > height_) ? 0U : static_cast<uint16_t>(height_ - mapY);
                    break;
                case 3:
                    finalX = (mapY > height_) ? 0U : static_cast<uint16_t>(height_ - mapY);
                    finalY = mapX;
                    break;
                default:
                    break;
            }

            points[i].x = finalX;
            points[i].y = finalY;
            points[i].id = id;
            points[i].size = size;

            APP_TOUCH_DEBUG_LOGI(kTag,
                     "point[%u]: raw{x=%u y=%u size=%u id=%u} mapped{x=%u y=%u} rotation=%u",
                     static_cast<unsigned>(i),
                     static_cast<unsigned>(x),
                     static_cast<unsigned>(y),
                     static_cast<unsigned>(size),
                     static_cast<unsigned>(id),
                     static_cast<unsigned>(finalX),
                     static_cast<unsigned>(finalY),
                     static_cast<unsigned>(rotation_));
        }
    }

    if(!clearStatus())
    {
        APP_TOUCH_DEBUG_LOGW(kTag, "read_points: clear status failed after %u point(s)", static_cast<unsigned>(outCount));
    }
    return static_cast<int8_t>(outCount);
}

void GT911::loop()
{
    if(callback_ == nullptr || !is_available())
    {
        return;
    }

    GTPoint points[kMaxTouchPoints] = {};
    const int8_t touchCount = read_points(points, kMaxTouchPoints);
    if(touchCount > 0)
    {
        callback_(touchCount, points);
    }
}

bool GT911::reset()
{
    if(i2c_bus_ == nullptr)
    {
        APP_TOUCH_DEBUG_LOGW(kTag, "reset: i2c bus is null");
        return false;
    }

    constexpr uint8_t kProbeAddrs[] = {GT911_ADDR2, GT911_ADDR1};
    uint8_t id[4] = {};
    for(uint8_t candidate : kProbeAddrs)
    {
        APP_TOUCH_DEBUG_LOGI(kTag, "reset: try candidate addr=0x%02X", candidate);
        if(!resetForAddress(candidate))
        {
            APP_TOUCH_DEBUG_LOGW(kTag, "reset: resetForAddress failed addr=0x%02X", candidate);
            continue;
        }

        const esp_err_t probe_err = i2c_master_probe(i2c_bus_, candidate, kI2CTimeoutMs);
        if(probe_err != ESP_OK)
        {
            APP_TOUCH_DEBUG_LOGW(kTag,
                     "reset: probe failed addr=0x%02X err=%s",
                     candidate,
                     esp_err_to_name(probe_err));
            continue;
        }

        if(!attachDevice(candidate))
        {
            APP_TOUCH_DEBUG_LOGW(kTag, "reset: attach device failed addr=0x%02X", candidate);
            continue;
        }

        if(readRegisters(GT911_REG_ID, id, sizeof(id)))
        {
            APP_TOUCH_DEBUG_LOGI(kTag,
                     "reset: id read ok addr=0x%02X id='%c%c%c%c' raw=%02X %02X %02X %02X",
                     candidate,
                     id[0],
                     id[1],
                     id[2],
                     id[3],
                     id[0],
                     id[1],
                     id[2],
                     id[3]);
            return true;
        }

        APP_TOUCH_DEBUG_LOGW(kTag, "reset: id read failed addr=0x%02X", candidate);
        detachDevice();
    }

    return false;
}

bool GT911::resetForAddress(uint8_t address)
{
    // GT911 address select:
    // INT=0 -> 0x5D, INT=1 -> 0x14 during reset rising edge.
    const int int_level = (address == GT911_ADDR1) ? 0 : 1;
    APP_TOUCH_DEBUG_LOGI(kTag,
             "resetForAddress: addr=0x%02X int_pin=%d rst_pin=%d int_level=%d",
             address,
             intPin_,
             rstPin_,
             int_level);

    gpio_hold_dis(static_cast<gpio_num_t>(rstPin_));
    gpio_hold_dis(static_cast<gpio_num_t>(intPin_));

    gpio_set_direction(static_cast<gpio_num_t>(rstPin_), GPIO_MODE_OUTPUT);
    gpio_set_direction(static_cast<gpio_num_t>(intPin_), GPIO_MODE_OUTPUT);

    gpio_set_level(static_cast<gpio_num_t>(rstPin_), 0);
    gpio_set_level(static_cast<gpio_num_t>(intPin_), int_level);
    vTaskDelay(pdMS_TO_TICKS(kResetHoldLowMs));

    gpio_set_level(static_cast<gpio_num_t>(rstPin_), 1);
    vTaskDelay(pdMS_TO_TICKS(kResetReleaseHighMs));

    gpio_set_direction(static_cast<gpio_num_t>(intPin_), GPIO_MODE_INPUT);
    vTaskDelay(pdMS_TO_TICKS(kResetAddressSettleMs));

    return true;
}

bool GT911::clearStatus()
{
    statusCached_ = false;
    cachedStatus_ = 0;
    const bool ok = writeRegister(GT911_REG_STATUS, 0);
    if(ok)
    {
        APP_TOUCH_DEBUG_LOGI(kTag, "clear status ok addr=0x%02X", addr_);
    }
    return ok;
}

bool GT911::writeRegister(uint16_t reg, uint8_t val)
{
    if(i2c_dev_ == nullptr)
    {
        APP_TOUCH_DEBUG_LOGW(kTag, "writeRegister: i2c device is null reg=0x%04X val=0x%02X", reg, val);
        return false;
    }

    const uint8_t payload[] = {
        static_cast<uint8_t>(reg >> 8),
        static_cast<uint8_t>(reg & 0xFFU),
        val,
    };
    const esp_err_t err = i2c_master_transmit(i2c_dev_, payload, sizeof(payload), kI2CTimeoutMs);
    if(err != ESP_OK)
    {
        APP_TOUCH_DEBUG_LOGW(kTag,
                 "writeRegister failed addr=0x%02X reg=0x%04X val=0x%02X err=%s",
                 addr_,
                 reg,
                 val,
                 esp_err_to_name(err));
        return false;
    }
    return true;
}

bool GT911::readRegister(uint16_t reg, uint8_t& val)
{
    return readRegisters(reg, &val, 1);
}

bool GT911::readRegisters(uint16_t reg, uint8_t* buffer, uint8_t len)
{
    if(i2c_dev_ == nullptr || buffer == nullptr || len == 0)
    {
        APP_TOUCH_DEBUG_LOGW(kTag,
                 "readRegisters rejected: dev=%p buffer=%p reg=0x%04X len=%u",
                 static_cast<void*>(i2c_dev_),
                 static_cast<void*>(buffer),
                 reg,
                 static_cast<unsigned>(len));
        return false;
    }

    const uint8_t reg_buf[] = {
        static_cast<uint8_t>(reg >> 8),
        static_cast<uint8_t>(reg & 0xFFU),
    };
    const esp_err_t err =
        i2c_master_transmit_receive(i2c_dev_, reg_buf, sizeof(reg_buf), buffer, len, kI2CTimeoutMs);
    if(err != ESP_OK)
    {
        APP_TOUCH_DEBUG_LOGW(kTag,
                 "readRegisters failed addr=0x%02X reg=0x%04X len=%u err=%s",
                 addr_,
                 reg,
                 static_cast<unsigned>(len),
                 esp_err_to_name(err));
        return false;
    }
    return true;
}

bool GT911::attachDevice(uint8_t address)
{
    if(i2c_bus_ == nullptr)
    {
        APP_TOUCH_DEBUG_LOGW(kTag, "attachDevice: i2c bus is null addr=0x%02X", address);
        return false;
    }

    detachDevice();

    const i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = address,
        .scl_speed_hz = kI2CFreqHz,
        .scl_wait_us = 0,
        .flags = {
            .disable_ack_check = 0,
        },
    };

    const esp_err_t err = i2c_master_bus_add_device(i2c_bus_, &dev_cfg, &i2c_dev_);
    if(err != ESP_OK)
    {
        i2c_dev_ = nullptr;
        APP_TOUCH_DEBUG_LOGW(kTag,
                 "attachDevice failed addr=0x%02X err=%s",
                 address,
                 esp_err_to_name(err));
        return false;
    }

    addr_ = address;
    APP_TOUCH_DEBUG_LOGI(kTag, "attachDevice ok addr=0x%02X freq=%u", addr_, static_cast<unsigned>(kI2CFreqHz));
    return true;
}

void GT911::detachDevice()
{
    if(i2c_dev_ == nullptr)
    {
        return;
    }

    i2c_master_bus_rm_device(i2c_dev_);
    i2c_dev_ = nullptr;
}

uint16_t GT911::mapCoord(uint16_t value, uint16_t inMax, uint16_t outMax)
{
    if(inMax == 0)
    {
        return 0;
    }

    const uint32_t mapped = (static_cast<uint32_t>(value) * static_cast<uint32_t>(outMax)) /
                            static_cast<uint32_t>(inMax);
    return static_cast<uint16_t>(mapped > 0xFFFFU ? 0xFFFFU : mapped);
}
