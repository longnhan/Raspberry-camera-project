#include "gpio.h"

GPIO::GPIO(const std::string& pin, const std::string& direction, const uint8_t active_val)
    : gpio_pin(pin), gpio_active_val(active_val)
{
    set_gpio_path(gpio_pin);
    export_gpio(gpio_pin);
    set_direction(direction);
    set_active_val(gpio_active_val);

    if (direction == GPIO_OUTPUT)
    {
        std::cout << "GPIO set to output\n";
        write(GPIO_RESET);
    } 
    else 
    {
        std::cout << "GPIO set to input\n";
    }
}

GPIO::~GPIO() 
{
    close();
}

void GPIO::set_gpio_path(const std::string& pin) {
    fp_direction = std::string(GPIO_PATH) + pin + "/direction";
    fp_value = std::string(GPIO_PATH) + pin + "/value";
    fp_edge = std::string(GPIO_PATH) + pin + "/edge";
    fp_active_low = std::string(GPIO_PATH) + pin + "/active_low";
    std::cout << "finish set_gpio_path";
}

void GPIO::export_gpio(const std::string& pin) 
{
    std::ofstream export_file(GPIO_EXPORT);
    if (!export_file.is_open()) 
    {
        throw std::runtime_error("Unable to open export file");
    }
    export_file << pin;
    std::cout << "finish export gpio";
}

void GPIO::unexport_gpio(const std::string& pin) 
{
    std::ofstream unexport_file(GPIO_UNEXPORT);
    if (!unexport_file.is_open()) 
    {
        throw std::runtime_error("Unable to open unexport file");
    }
    unexport_file << pin;
}

void GPIO::set_direction(const std::string& direction) 
{
    std::ofstream direction_file(fp_direction);
    std::cout << fp_direction << std::endl;
    if (!direction_file.is_open()) 
    {
        throw std::runtime_error("Unable to open direction file");
    }
    direction_file << direction;
    std::cout << "finish set_direction";
}

void GPIO::set_active_val(const uint8_t active_value)
{
    std::ofstream active_low_file(fp_active_low);
    if (!active_low_file.is_open()) 
    {
        throw std::runtime_error("Unable to open direction file");
    }
    active_low_file << active_value;
    std::cout << "finish set_active_val";
}

void GPIO::write(const std::string& state) 
{
    std::ofstream value_file(fp_value);
    if (!value_file.is_open()) 
    {
        throw std::runtime_error("Unable to open value file");
    }
    value_file << state;
}

uint8_t GPIO::read() 
{
    std::ifstream value_file(fp_value);
    if (!value_file.is_open()) 
    {
        throw std::runtime_error("Unable to open value file");
    }
    std::string buf;
    value_file >> buf;
    std::cout << "Value is: " << buf << std::endl;
    return static_cast<uint8_t>(std::stoi(buf));
}

void GPIO::toggle() 
{
    if (read() == 0) 
    {
        write(GPIO_SET);
    } 
    else 
    {
        write(GPIO_RESET);
    }
}

void GPIO::close() 
{
    unexport_gpio(gpio_pin);
    fp_direction.clear();
    fp_value.clear();
    fp_active_low.clear();
    fp_edge.clear();
}
