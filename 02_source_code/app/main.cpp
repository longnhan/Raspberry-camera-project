#include "main.h"

Button shutter_btn(SHUTTER_BUTTON, GPIO_INPUT, EN_ACTIVE_HIGH);

std::atomic<bool> keep_running(true);

CameraControl sonyGlobalShutterCam;

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
    
    // Create a separate thread for button handling
    std::thread button_handler(buttonThread);

    // Create a separate thread for button handling
    std::thread camera_handler(cameraThread);

    // Join the thread before exiting
    button_handler.join();
    camera_handler.join();

    return 0;
}

void buttonThread()
{
    std::cout << ":::::::::::: <--- BUTTON HANDLER START ---> ::::::::::::\n";

    while (keep_running)
    {
        shutter_btn.update();
        ButtonState state = shutter_btn.getState();

        if (state == ButtonState::PRESSED) std::cout << "Button Pressed\n";
        if (state == ButtonState::HELD) std::cout << "Button Held\n";
        if (state == ButtonState::RELEASED) std::cout << "Button Released\n";

        usleep(10000); // Sleep for 10ms to avoid excessive CPU usage
    }
}

void cameraThread()
{
    std::cout << ":::::::::::: <--- CAMERA OPERATING START ---> ::::::::::::\n";

    while (keep_running)
    {
        usleep(10000); // Sleep for 10ms to avoid excessive CPU usage
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