#include <iostream>

#include "camera_control.h"
#include "gpio.h"
#include "gui_display.h"
#include "log.h"

void initializeModules() 
{
    // // Example initialization of each module
    // std::cout << "Initializing camera module..." << std::endl;
    // camera::initializeCamera();

    // std::cout << "Initializing GPIO module..." << std::endl;
    // gpio::initializeGPIO();

    // std::cout << "Initializing GUI module..." << std::endl;
    // gui::initializeDisplay();

    // std::cout << "Initializing logging module..." << std::endl;
    // logging::initializeLogging();
}

int main() 
{
    // Initialize all modules
    initializeModules();

    // Main loop or functionality goes here
    std::cout << "Application started!" << std::endl;

    // Placeholder for the main loop or application logic
    while (true) 
    {
        // Application logic (empty for now)
    }

    return 0;
}
