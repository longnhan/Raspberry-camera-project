#include "main.h"

Button shutter_btn(SHUTTER_BUTTON, GPIO_INPUT, EN_ACTIVE_HIGH);

std::atomic<bool> keep_running(true);

void initializeModules() 
{

}

int main() 
{
    // Save build time , __DATE__ and __TIME__ macro will be process in preprocesser
    std::cerr << "Time build : " << __DATE__ << " " << __TIME__ << std::endl; 
    // Set up the signal handler
    std::signal(SIGINT, signalHandler);

    // Initialize all modules
    initializeModules();

    std::cout << "Application started!" << std::endl;
    
    // Create a separate thread for button handling
    std::thread button_handler(buttonThread);

    // Join the thread before exiting
    button_handler.join();

    return 0;
}

void buttonThread()
{
    while (keep_running)
    {
        shutter_btn.update();
        ButtonState state = shutter_btn.getState();

        if (state == ButtonState::PRESSED) std::cout << "Button Pressed\n";
        if (state == ButtonState::HELD) std::cout << "Button Held\n";
        if (state == ButtonState::RELEASED) std::cout << "Button Released\n";

        DELAY_MS(10); // Sleep for 10ms to avoid excessive CPU usage
    }
}

void signalHandler(int signal)
{
    if (signal == SIGINT)
    {
        std::cout << "\nCtrl+C detected. Cleaning up and exiting..." << std::endl;
        keep_running = false;
    }
}