#ifndef _GPIO_H_
#define _GPIO_H_

#include <cstdint>
#include <string>
#include <fstream>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>
#include <stdexcept>


/* Define GPIO state */
constexpr auto GPIO_SET = "1";
constexpr auto GPIO_RESET = "0";
constexpr auto GPIO_ACTIVE_H = 1;
constexpr auto GPIO_ACTIVE_L = 0;

/* Define GPIO system files */
constexpr auto GPIO_PATH = "/sys/class/gpio/gpio";
constexpr auto GPIO_EXPORT = "/sys/class/gpio/export";
constexpr auto GPIO_UNEXPORT = "/sys/class/gpio/unexport";

/* Define direction */
constexpr auto GPIO_INPUT = "in";
constexpr auto GPIO_OUTPUT = "out";

constexpr auto GPIO_PIN_16 = "528"; //gpio16 + gpiochip512

class GPIO {
private:
    std::string fp_value;
    std::string fp_direction;
    std::string fp_edge;
    std::string fp_active_low;

    std::string gpio_pin;
    uint8_t gpio_active_val;

    void export_gpio(const std::string& pin);
    void unexport_gpio(const std::string& pin);
    void set_gpio_path(const std::string& pin);
    void set_direction(const std::string& direction);
    void set_active_val(const uint8_t active_value);

public:
    GPIO(const std::string& pin, const std::string& direction, const uint8_t active_val);
    ~GPIO();
    uint8_t read();
    void toggle();
    void write(const std::string& state);
    void close();
};

#endif /* _GPIO_H_ */
