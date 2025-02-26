#ifndef _BUTTON_HANDLER_H_
#define _BUTTON_HANDLER_H_

#include "gpio.h"
#include <cstdint>
#include <iostream>
#include <chrono>
#include <thread>

constexpr int EN_ACTIVE_HIGH  = GPIO_ACTIVE_H;
constexpr int EN_ACTIVE_LOW  = GPIO_ACTIVE_L;
constexpr auto SHUTTER_BUTTON = GPIO_PIN_16;

enum class ButtonState{
    IDLE,
    PRESSED,
    HELD,
    RELEASED,
    DOUBLE_PRESSED
};

class Button : public GPIO {
private:
    // bool activeHigh;
    ButtonState state = ButtonState::IDLE;
    uint8_t lastValue = 1;
    uint32_t camera_shutter_count = 0;
    std::chrono::steady_clock::time_point lastPressTime;

public:
    Button(const std::string& pin, const std::string& direction, const int active_val);
    ~Button();

    uint8_t read();
    void toggle();
    void write(bool state);
    void close();

    void updateButtonState();
    void updateShotCounter();
    ButtonState getState();
    uint32_t getShotCount();
};

#endif /* _BUTTON_HANDLER_H_ */
