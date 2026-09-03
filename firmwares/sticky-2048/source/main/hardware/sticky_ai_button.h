#pragma once

#include <atomic>

#include "iot_button.h"

enum class AiButtonEvent {
    None,
    ShortPress,
    LongPress,
};

class StickyAiButton {
public:
    bool init();
    AiButtonEvent poll();
    bool is_pressed() const;

private:
    static void on_short_press(void *button_handle, void *user_data);
    static void on_long_press(void *button_handle, void *user_data);

    button_handle_t handle_ = nullptr;
    std::atomic<AiButtonEvent> pending_event_ = AiButtonEvent::None;
};
