#include "main.h"

Button shutter_btn(SHUTTER_BUTTON, GPIO_INPUT, EN_ACTIVE_HIGH);

std::atomic<bool> keep_running(true);

CameraControl sonyGlobalShutterCam;

// Shared data structure for shooting mechanism
std::queue<int> shutterQueue;

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

    // Create a separate thread for camera handling
    std::thread camera_handler(cameraThread);

    // Join the threads before exiting
    button_handler.join();
    camera_handler.join();

    return 0;
}

void buttonThread()
{
    std::cout << ":::::::::::: <--- BUTTON HANDLER START ---> ::::::::::::\n";
    
    static std::atomic<uint32_t> currnt_shot = 0;
    while (keep_running)
    {
        shutter_btn.updateButtonState();
        shutter_btn.updateShotCounter();
        ButtonState state = shutter_btn.getState();

        // Detect single press
        if ((shutter_btn.getShotCount() - currnt_shot) == 1)
        {
            currnt_shot = shutter_btn.getShotCount();
            shutterQueue.push(currnt_shot);
        }
    }
}

void cameraThread()
{
    std::cout << ":::::::::::: <--- CAMERA OPERATING START ---> ::::::::::::\n";

    while (keep_running)
    {
        if (!shutterQueue.empty())
        {
            int shotCount = shutterQueue.front();
            shutterQueue.pop();

            sonyGlobalShutterCam.captureImage();
        }
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
