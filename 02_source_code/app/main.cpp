#include "main.h"

Button shutter_btn(SHUTTER_BUTTON, GPIO_INPUT, EN_ACTIVE_HIGH);

std::atomic<bool> keep_running(true);

void initializeModules() 
{

}

int main() 
{
    // Set up the signal handler
    std::signal(SIGINT, signalHandler);

    // Initialize all modules
    initializeModules();

    std::cout << "Application started!" << std::endl;
    
    while (keep_running)
    {

    }

    return 0;
}

void signalHandler(int signal)
{
    if (signal == SIGINT)
    {
        std::cout << "\nCtrl+C detected. Cleaning up and exiting..." << std::endl;
        keep_running = false;
    }
}