#include "gui_display.h"
#include <iostream>

GUIDisplay::GUIDisplay() = default;

GUIDisplay::~GUIDisplay() {
    if (videoTexture_) {
        SDL_DestroyTexture(videoTexture_);
    }
    if (renderer_) {
        SDL_DestroyRenderer(renderer_);
    }
    if (window_) {
        SDL_DestroyWindow(window_);
    }
    SDL_Quit();
    std::cout << "[GUIDisplay] Cleaned up SDL resources." << std::endl;
}

bool GUIDisplay::initialize() {
    // This is the essential part that needs to work for the LCD
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
        std::cerr << "[GUIDisplay] SDL could not initialize! Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Create window sized 480x320
    window_ = SDL_CreateWindow("Rudo Camera", 
                               SDL_WINDOWPOS_UNDEFINED, 
                               SDL_WINDOWPOS_UNDEFINED, 
                               SCREEN_WIDTH, 
                               SCREEN_HEIGHT, 
                               SDL_WINDOW_SHOWN);
    if (!window_) {
        std::cerr << "[GUIDisplay] Window could not be created! Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Create hardware-accelerated renderer
    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer_) {
        std::cerr << "[GUIDisplay] Renderer could not be created! Error: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Create the persistent texture for the video stream (using BGR format which OpenCV produces)
    videoTexture_ = SDL_CreateTexture(renderer_, 
                                       SDL_PIXELFORMAT_BGR24, // Matches the format converted by OpenCV
                                       SDL_TEXTUREACCESS_STREAMING, 
                                       SCREEN_WIDTH, 
                                       SCREEN_HEIGHT);
    if (!videoTexture_) {
        std::cerr << "[GUIDisplay] Texture could not be created! Error: " << SDL_GetError() << std::endl;
        return false;
    }

    std::cout << "[GUIDisplay] SDL and display resources initialized." << std::endl;
    return true;
}

void GUIDisplay::renderFrame(const cv::Mat &frame) {
    if (!renderer_ || !videoTexture_ || frame.empty()) {
        return;
    }
    
    // 1. Update the texture with the new frame data
    SDL_UpdateTexture(videoTexture_, 
                      NULL, 
                      frame.data, 
                      frame.cols * frame.elemSize());

    // 2. Clear the screen
    SDL_SetRenderDrawColor(renderer_, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(renderer_);

    // 3. Copy the video texture to the renderer (filling the entire screen)
    SDL_RenderCopy(renderer_, videoTexture_, NULL, NULL); 
}

// Implementation for the simplified drawUIOverlays (FIX 1)
void GUIDisplay::drawUIOverlays() {
    // Only static overlays (e.g., crosshair) would go here.
}

// Implementation for the missing present method (FIX 2)
void GUIDisplay::present() {
    if (renderer_) {
        // This is the essential SDL call to swap buffers and display the result
        SDL_RenderPresent(renderer_); // FIX: Missing implementation of SDL_RenderPresent
    }
}