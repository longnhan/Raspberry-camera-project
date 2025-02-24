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

    LOG_STT("Application started!");
    
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
    LOG_STT(":::::::::::: <--- BUTTON HANDLER START ---> ::::::::::::");
    
    static uint32_t currnt_shot = 0;
    while (keep_running)
    {
        shutter_btn.updateButtonState();
        shutter_btn.updateShotCounter();
        ButtonState state = shutter_btn.getState();

        // Detect single press
        if ((shutter_btn.getShotCount() - currnt_shot) == 1)
        {
            currnt_shot = shutter_btn.getShotCount();
            LOG_DBG("[LOG_DEBUG] get currnt shot: ", currnt_shot);
            shutterQueue.push(currnt_shot);
            LOG_DBG("[LOG_DEBUG] finish push to queue: ", shutterQueue.size());
        }
    }
}

void cameraThread()
{
    LOG_STT(":::::::::::: <--- CAMERA OPERATING START ---> ::::::::::::");

    while (keep_running)
    {
        if (!shutterQueue.empty())
        {
            LOG_DBG("[LOG_DEBUG] camera check queue not empty");
            int shotCount = shutterQueue.front();
            
            LOG_DBG("[LOG_DEBUG] pop queue");
            shutterQueue.pop();
            LOG_DBG("[LOG_DEBUG] finish pop to queue: ", shutterQueue.size());
            
            LOG_DBG("[LOG_DEBUG] start capture image");
            sonyGlobalShutterCam.captureImage();
            LOG_DBG("[LOG_DEBUG] finish capture image");
            
            LOG_DBG("[LOG_DEBUG] waiting for the next pressing");
        }
    }
}

void signalHandler(int signal)
{
    if (signal == SIGINT)
    {
        LOG_STT("Ctrl+C detected. Cleaning up and exiting...");
        keep_running = false;
    }
}
