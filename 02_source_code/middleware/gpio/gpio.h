#ifndef _GPIO_H_
#define _GPIO_H_

#include <cstdint>
#include <string>
#include <fstream>

#define GW_BOARD

#ifdef GW_BOARD

/* Define GPIO state */
constexpr auto GPIO_SET = "1";
constexpr auto GPIO_RESET = "0";

/* Define GPIO system files */
constexpr auto GPIO_PATH = "/sys/class/gpio/gpio";
constexpr auto GPIO_EXPORT = "/sys/class/gpio/export";
constexpr auto GPIO_UNEXPORT = "/sys/class/gpio/unexport";

/* Define direction */
constexpr auto GPIO_INPUT = "in";
constexpr auto GPIO_OUTPUT = "out";

constexpr auto USR_BUTTON = "117";

#endif /* GW_BOARD */

class GPIO {
private:
    std::string fp_value;
    std::string fp_direction;
    std::string fp_edge;
    std::string fp_label;
    std::string fp_active_low;
    std::string gpio_pin;

    void export_gpio();
    void unexport_gpio();
    void set_gpio_path(const std::string& pin);
    void set_direction(const std::string& direction);

public:
    GPIO(const std::string& pin, const std::string& direction);
    ~GPIO();

    uint8_t read();
    void toggle();
    void write(const std::string& state);
    void close();
};

#endif /* _GPIO_H_ */
