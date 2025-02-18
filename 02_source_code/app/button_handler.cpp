#include "button_handler.h"

Button::Button(const std::string& pin, const std::string& direction, const int active_val)
    : GPIO(pin, direction, active_val), state(ButtonState::RELEASED)
{
    lastValue = read();
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

    return state;
}

void Button::toggle() {}

void Button::write(bool state) {}

void Button::updateButtonState() 
{
    uint8_t currentValue = read();  // Read GPIO pin state
    auto now = std::chrono::steady_clock::now();

    if (currentValue == 0 && lastValue == 1) 
    { 
        // Button Pressed
        state = ButtonState::PRESSED;
        lastPressTime = now;  // Save press time
    } 
    else if (currentValue == 0 && lastValue == 0) 
    { 
        // Button is still pressed -> Check for hold
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastPressTime).count() > 1000) 
        {
            state = ButtonState::HELD;
        }
    } 
    else if (currentValue == 1 && lastValue == 0) 
    { 
        // Button Released (always reset to RELEASED)
        state = ButtonState::RELEASED;
    }

    lastValue = currentValue; // Store last state
}

uint32_t Button::getShotCount()
{
    return camera_shutter_count;
}

ButtonState Button::getState() 
{
    return state;
}

void Button::close() 
{
    GPIO::close();
    
}

void Button::updateShotCounter()
{
    static ButtonState last_state = ButtonState::IDLE;
    ButtonState new_state;

    if(last_state == ButtonState::RELEASED)
    {
        std::cout << "Button Released\n";
        updateButtonState();
        new_state = state;
        if(new_state == ButtonState::PRESSED)
        {
            std::cout << "Button Pressed\n";
            camera_shutter_count++;
            std::cout << "shutter counting: " << camera_shutter_count <<"\n";
            last_state = ButtonState::IDLE;
        }
    }
    else
    {
        last_state = state;
    }
}