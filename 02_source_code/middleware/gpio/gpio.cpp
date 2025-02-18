#include "gpio.h"

GPIO::GPIO(const std::string& pin, const std::string& direction, const int active_val)
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

void GPIO::set_gpio_path(const std::string& pin) 
{
    fp_direction = std::string(GPIO_PATH) + pin + "/direction";
    std::cout << "Direction path:" << fp_direction << std::endl;

    fp_value = std::string(GPIO_PATH) + pin + "/value";
    std::cout << "Value path:" << fp_value << std::endl;

    fp_edge = std::string(GPIO_PATH) + pin + "/edge";
    std::cout << "Edge path:" << fp_edge << std::endl;

    fp_active_low = std::string(GPIO_PATH) + pin + "/active_low";
    std::cout << "Active_low path:" << fp_active_low << std::endl;

    std::cout << "finish set_gpio_path" << std::endl;
}

void GPIO::export_gpio(const std::string& pin) 
{
    std::ofstream export_file(GPIO_EXPORT);
    if (!export_file.is_open()) 
    {
        throw std::runtime_error("Unable to open export file");
    }
    export_file << pin;
    export_file.close();
    std::cout << "finish export gpio" << std::endl;
}

void GPIO::unexport_gpio(const std::string& pin) 
{
    std::ofstream unexport_file(GPIO_UNEXPORT);
    if (!unexport_file.is_open()) 
    {
        throw std::runtime_error("Unable to open unexport file");
    }
    unexport_file << pin;
    unexport_file.close();
}

void GPIO::set_direction(const std::string& direction) 
{
    std::ofstream direction_file(fp_direction);
    if (!direction_file.is_open()) 
    {
        throw std::runtime_error("Unable to open direction file");
    }
    direction_file << direction;
    //close file before attempt to read
    direction_file.close();

    std::cout << "finish set_direction" << std::endl;
}

void GPIO::set_active_val(const int active_value)
{
    //set HIGH for fp_active_low
    std::ofstream set_active_low(fp_active_low);
    if (!set_active_low.is_open()) 
    {
        throw std::runtime_error("Unable to open direction file");
    }
    set_active_low << active_value;
    //close file before attempt to read
    set_active_low.close();


    //recheck fp_active_low vale
    const uint8_t MAX_RETRIES = 10;
    uint8_t count_retries = 0;
    
    int ret = -1;
    //open file for reading value
    std::ifstream check_active_low(fp_active_low);
    //get value for ret
    check_active_low >> ret;
    std::cout << "Active value ret: " << ret << std::endl;
    //close file
    check_active_low.close();
    
    while((ret != active_value) && (count_retries < MAX_RETRIES))
    {
        //reassign value
        std::cout << "recheck assign active high" << std::endl;
        std::ofstream set_active_low(fp_active_low);
        set_active_low << active_value;
        set_active_low.close();

        //recheck active high value
        std::cout << "recheck check active high" << std::endl;
        std::ifstream check_active_low(fp_active_low);
        check_active_low >> ret;
        std::cout << "Active value ret: " << ret << std::endl;
        check_active_low.close();

        usleep(100);
        
        count_retries++;
    }

    std::cout << "finish set_active_val" << std::endl;
}

void GPIO::write(const std::string& state) 
{
    std::ofstream value_file(fp_value);
    if (!value_file.is_open()) 
    {
        throw std::runtime_error("Unable to open value file");
    }
    value_file << state;
    value_file.close();
}

uint8_t GPIO::read() 
{
    std::ifstream value_file(fp_value);
    if (!value_file.is_open()) 
    {
        throw std::runtime_error("Unable to open value file");
    }

    int value;
    value_file >> value;
    // std::cout << "GPIO read value: " << value << std::endl;
    return static_cast<uint8_t>(value);
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
