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

// --- CAMERA THREAD (Aggressive Delays) ---
void cameraThread()
{
    LOG_STT(":::::::::::: <--- CAMERA OPERATING START ---> ::::::::::::");
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
                
                // 1. STOP STREAM
                sonyGSC.stopVideoStream();
                // FIX: Increase delay to 2000ms (2.0 seconds)
                std::this_thread::sleep_for(std::chrono::milliseconds(2000));
                
                // 2. CAPTURE
                sonyGSC.captureImage(path);
                // FIX: Increase delay to 2000ms (2.0 seconds)
                std::this_thread::sleep_for(std::chrono::milliseconds(2000));
                
                // 3. RESTART STREAM
                sonyGSC.startVideoStream(640, 480);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
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