#ifndef _BUTTON_HANDLER_H_
#define _BUTTON_HANDLER_H_

#include "gpio.h"
#include <cstdint>
#include <iostream>

constexpr auto EN_ACTIVE_HIGH  = GPIO_ACTIVE_H;
constexpr auto SHUTTER_BUTTON = GPIO_PIN_16;

class Button : public GPIO {
private:
    bool activeHigh;
public:
    Button(const std::string& pin, const std::string& direction, const uint8_t active_val);
    ~Button();

    uint8_t read();
    void toggle();
    void write(bool state);
    void close();
};

#endif /* _BUTTON_HANDLER_H_ */
