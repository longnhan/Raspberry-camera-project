#include "main.h" 
#include "gui_display.h" 
#include "screen.h"      
#include "photo_management.h" 

#include <iostream>
#include <thread>
#include <csignal>
#include <atomic>
#include <queue>

// --- Global Variables ---
Button shutter_btn(SHUTTER_BUTTON, GPIO_INPUT, EN_ACTIVE_HIGH);
std::atomic<bool> keep_running(true);
cameraSetting sonyGSCSettings;
photo_management sonyGSCPhotoMgr; 
CameraApp sonyGSC(ISO_800, SS1_125, Disable, F4_0, FP1_8); 
std::queue<int> shutterQueue;

GUIDisplay guiDisplay; 
Screen appScreen(&guiDisplay, &sonyGSC);

void signalHandler(int signum) {
    keep_running = false;
    // Wake up threads if they are sleeping (optional improvement)
}

void initializeModules() {}

void buttonThread()
{
    LOG_DBG(":::::::::::: <--- BUTTON HANDLER START ---> ::::::::::::");
    static uint32_t currnt_shot = 0;
    while (keep_running)
    {
        shutter_btn.updateButtonState();
        shutter_btn.updateShotCounter();
        if ((shutter_btn.getShotCount() - currnt_shot) == 1)
        {
            currnt_shot = shutter_btn.getShotCount();
            shutterQueue.push(currnt_shot);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

// --- CAMERA THREAD (New Parallel Architecture) ---
void cameraThread()
{
    LOG_STT(":::::::::::: <--- CAMERA OPERATING START ---> ::::::::::::");
    
    // 1. START STREAM ONCE
    // We start the dual-pipeline immediately. It stays running the entire time.
    if (!sonyGSC.startVideoStream(640, 480)) {
        LOG_ERR("Failed to start camera stream!");
        keep_running = false; // Exit if camera fails
        return;
    }

    while(keep_running)
    {
        if(!shutterQueue.empty())
        {
            int signal_shutter = shutterQueue.front();
            shutterQueue.pop();
            
            if(signal_shutter > 0)
            {
                std::string path = sonyGSCPhotoMgr.getpathpicture();
                LOG_STT("Taking photo: ", path);
                
                // 2. CAPTURE IN PARALLEL
                // We do NOT stop the video. We just request a high-res frame.
                // The video feed will continue uninterrupted.
                if (sonyGSC.captureImage(path)) {
                    LOG_STT("Photo saved successfully.");
                } else {
                    LOG_ERR("Photo capture failed.");
                }
                
                // Optional: Tiny delay just to prevent button spamming
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // 3. CLEAN SHUTDOWN
    sonyGSC.stopVideoStream();
    LOG_STT("Camera thread stopped.");
}

int main()
{
    std::signal(SIGINT, signalHandler);
    initializeModules();
    
    std::thread button_handler(buttonThread);
    std::thread camera_handler(cameraThread);
    std::thread gui_handler(&Screen::guiThread, &appScreen);

    button_handler.join();
    camera_handler.join();
    gui_handler.join();

    return 0;
}