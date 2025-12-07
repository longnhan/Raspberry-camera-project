#include "screen.h"
#include "main.h" 
#include "camera_app.h" 
#include "gui_display.h" 
#include <iostream>

// Defined in main.cpp
extern std::atomic<bool> keep_running; 

Screen::Screen(GUIDisplay* display, CameraApp* app) 
    : guiDisplay_(display), cameraApp_(app) 
{
}

void Screen::handleSDLEvents(SDL_Event &event) {
    if (event.type == SDL_QUIT) {
        LOG_STT("SDL Quit signal received.");
        keep_running = false;
    }
    else if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_ESCAPE) {
             keep_running = false;
        }
    }
}

void Screen::guiThread()
{
    // FIX: Pass the specific screen resolution here (App Layer configuration)
    // 480x320 for Waveshare 3.5" LCD
    if (!guiDisplay_->initialize(480, 320)) {
        LOG_ERR("[LOG_ERROR] GUI initialization failed. Cannot start rendering loop.");
        keep_running = false;
        return;
    }

    if (!cameraApp_->startVideoStream(480, 320)) { 
        LOG_ERR("Failed to start video stream.");
        keep_running = false;
        return;
    }

    cv::Mat previewFrame; 
    SDL_Event event;
    
    while (keep_running) {
        while (SDL_PollEvent(&event)) {
            handleSDLEvents(event);
        }

        if (cameraApp_->getLatestPreviewFrame(previewFrame)) {
            guiDisplay_->renderFrame(previewFrame); 
            guiDisplay_->drawUIOverlays(); 
            guiDisplay_->present();
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    
    cameraApp_->stopVideoStream(); 
    LOG_STT(":::::::::::: <--- GUI RENDERER END ---> ::::::::::::");
}