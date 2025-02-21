#include "main.h"

Button shutter_btn(SHUTTER_BUTTON, GPIO_INPUT, EN_ACTIVE_HIGH);

std::atomic<bool> keep_running(true);

CameraControl sonyGlobalShutterCam;

// Shared data structure for shooting mechanism
std::queue<int> shutterQueue;
std::mutex shutterLockingMutex;
std::condition_variable shutter_cv;

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
    
    static std::atomic<uint32_t> currnt_shot = 0;
    while (keep_running)
    {
        shutter_btn.updateButtonState();
        shutter_btn.updateShotCounter();
        ButtonState state = shutter_btn.getState();

        // calculate a single press
        if((shutter_btn.getShotCount() - currnt_shot) == 1)
        {
            currnt_shot = shutter_btn.getShotCount();
            
            // mutex locking
            std::lock_guard<std::mutex> lock(shutterLockingMutex);
            //push current shot value into queue
            shutterQueue.push(currnt_shot);
            
            // trigger camera take image
            // trigger shutter_cv.wait
            shutter_cv.notify_one();
        }
    }
}

void cameraThread()
{
    std::cout << ":::::::::::: <--- CAMERA OPERATING START ---> ::::::::::::\n";

    while (keep_running)
    {
		// init and immidiately lock mutex
        std::unique_lock<std::mutex> shutterMutex(shutterLockingMutex);
        shutter_cv.wait(shutterMutex, [] { return !shutterQueue.empty(); });

        while (!shutterQueue.empty())
        {
            int shotCount = shutterQueue.front();
            shutterQueue.pop();
            
			// Unlock before capturing to avoid blocking buttonThread
			shutterMutex.unlock();
            
			sonyGlobalShutterCam.captureImage();

			// No need to re-lock; loop will reacquire it if needed
        }
    }
}

void signalHandler(int signal)
{
    if (signal == SIGINT)
    {
        std::cout << "\nCtrl+C detected. Cleaning up and exiting..." << std::endl;
        keep_running = false;

        // Wake up cameraThread() to allow exit
        shutter_cv.notify_one();
    }
}