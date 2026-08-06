#pragma once

#include <cstdint>

class StickyBuzzer {
public:
    bool init();
    bool tone(uint32_t frequency_hz);
    void stop();

private:
    bool ready_ = false;
};
