#include "gpio.h"
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>
#include <stdexcept>

GPIO::GPIO(const std::string& pin, const std::string& direction)
    : gpio_pin(pin) {
    std::cout << "Set GPIO " << direction << " for pin " << pin << std::endl;

    set_gpio_path(pin);
    export_gpio();
    set_direction(direction);

    if (direction == GPIO_OUTPUT) {
        std::cout << "GPIO set to output\n";
        write(GPIO_RESET);
    } else {
        std::cout << "GPIO set to input\n";
    }
}

GPIO::~GPIO() {
    close();
}

void GPIO::set_gpio_path(const std::string& pin) {
    fp_direction = std::string(GPIO_PATH) + pin + "/direction";
    fp_value = std::string(GPIO_PATH) + pin + "/value";
}

void GPIO::export_gpio() {
    std::ofstream export_file(GPIO_EXPORT);
    if (!export_file.is_open()) {
        throw std::runtime_error("Unable to open export file");
    }
    export_file << gpio_pin;
}

void GPIO::unexport_gpio() {
    std::ofstream unexport_file(GPIO_UNEXPORT);
    if (!unexport_file.is_open()) {
        throw std::runtime_error("Unable to open unexport file");
    }
    unexport_file << gpio_pin;
}

void GPIO::set_direction(const std::string& direction) {
    std::ofstream direction_file(fp_direction);
    if (!direction_file.is_open()) {
        throw std::runtime_error("Unable to open direction file");
    }
    direction_file << direction;
}

void GPIO::write(const std::string& state) {
    std::ofstream value_file(fp_value);
    if (!value_file.is_open()) {
        throw std::runtime_error("Unable to open value file");
    }
    value_file << state;
}

uint8_t GPIO::read() {
    std::ifstream value_file(fp_value);
    if (!value_file.is_open()) {
        throw std::runtime_error("Unable to open value file");
    }
    std::string buf;
    value_file >> buf;
    std::cout << "Value is: " << buf << std::endl;
    return static_cast<uint8_t>(std::stoi(buf));
}

void GPIO::toggle() {
    if (read() == 0) {
        write(GPIO_SET);
    } else {
        write(GPIO_RESET);
    }
}

void GPIO::close() {
    unexport_gpio();
    fp_direction.clear();
    fp_value.clear();
}
