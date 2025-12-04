#include "screen.h"
#include "main.h" // Assuming main.h contains 'keep_running' and CameraApp declaration
#include "camera_app.h" // Need full definition of CameraApp 
#include "gui_display.h" // Need full definition of GUIDisplay 
#include "button_handler.h" // Needed to process touch events (future)
#include <iostream>

// Defined in main.cpp
extern std::atomic<bool> keep_running; 

Screen::Screen(GUIDisplay* display, CameraApp* app) 
    : guiDisplay_(display), cameraApp_(app) 
{
    // The main app is initialized before this point, so we just store the pointers.
}

void Screen::handleSDLEvents(SDL_Event &event) {
    // This is the future home for translating touch/mouse events into app commands.
    // Example: if (event.type == SDL_FINGERDOWN || event.type == SDL_MOUSEBUTTONDOWN)
    
    if (event.type == SDL_QUIT) {
        LOG_STT("SDL Quit signal received.");
        keep_running = false;
    }
    // Add logic here to check coordinates for touch buttons if you use touch input
    // Example: buttonHandler.processTouch(event.x, event.y);
}

void Screen::guiThread()
{
    // ... initialization ...
    if (!guiDisplay_->initialize()) {
        LOG_ERR("[LOG_ERROR] GUI initialization failed. Cannot start rendering loop.");
        keep_running = false;
        return;
    }

    // Start the continuous stream before entering the rendering loop
    // Assume 480x320 resolution for the preview stream
    if (!cameraApp_->startVideoStream(480, 320)) { 
        LOG_ERR("Failed to start video stream.");
        keep_running = false;
        return;
    }

    cv::Mat previewFrame; 
    
    while (keep_running) {
        // ... poll events ...

        // Wait for and get the latest frame. This call blocks.
        if (cameraApp_->getLatestPreviewFrame(previewFrame)) {
            
            guiDisplay_->renderFrame(previewFrame); 
            
            // Only draw a simplified overlay (or nothing)
            guiDisplay_->drawUIOverlays(); 

            guiDisplay_->present();
        }
    }
    
    cameraApp_->stopVideoStream(); // Stop the stream cleanly
    LOG_STT(":::::::::::: <--- GUI RENDERER END ---> ::::::::::::");
}