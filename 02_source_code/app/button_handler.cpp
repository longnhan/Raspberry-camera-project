#include "button_handler.h"

Button::Button(const std::string& pin, const std::string& direction, const int active_val)
    : GPIO(pin, direction, active_val)
{

}

Button::~Button() 
{
    close();
}

uint8_t Button::read() 
{
    // Ensure the button is configured as input before reading
    // if (this->getDirection() != GPIO::INPUT) {
    //     std::cerr << "Error: Attempt to read from a non-input button\n";
    //     return 0;
    // }
    // Read the GPIO pin state and adjust for active-high or active-low configuration
    uint8_t state = GPIO::read();
    return activeHigh ? state : !state;
}

void Button::toggle() 
{
    // Ensure the button is configured as output before toggling
    // if (this->getDirection() != GPIO::OUTPUT) {
    //     std::cerr << "Error: Attempt to toggle a non-output button\n";
    //     return;
    // }
    // // Buttons don't typically toggle, but we can simulate for testing purposes
    // uint8_t currentState = read();
    // write(!currentState);
}

void Button::write(bool state) {
    // Ensure the button is configured as output before writing
    // if (this->getDirection() != GPIO::OUTPUT) {
    //     std::cerr << "Error: Attempt to write to a non-output button\n";
    //     return;
    // }
    // // For testing or simulating, write a state to the GPIO pin
    // GPIO::write(activeHigh ? state : !state);
}

void Button::close() 
{
    GPIO::close();
    // std::cout << "Button on pin " << static_cast<int>(getPin()) << " closed.\n";
}
