#include <iostream>

#include "camera_control.h"
#include "button_handler.h"
#include "gui_display.h"
#include "log.h"

void initializeModules() 
{

}

int main() 
{
    // Initialize all modules
    initializeModules();

    // Main loop or functionality goes here
    std::cout << "Application started!" << std::endl;
    Button test(USR_BUTTON, GPIO_INPUT, GPIO_ACTIVE_H);
    // Read the input button state

    // Placeholder for the main loop or application logic
    while (true)
    {
        // test.read();
        usleep(500000);
        // Application logic (empty for now)
    }

    return 0;
}
