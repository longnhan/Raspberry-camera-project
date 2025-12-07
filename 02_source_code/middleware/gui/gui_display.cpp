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

bool GUIDisplay::initialize(int width, int height) {
    // Store configuration
    width_ = width;
    height_ = height;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
        std::cerr << "[GUIDisplay] SDL could not initialize! Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // FIX: Use SDL_WINDOW_FULLSCREEN_DESKTOP along with BORDERLESS.
    // This forces the window to occupy the entire display area (480x320), 
    // overriding window managers or desktop environment borders.
    window_ = SDL_CreateWindow("Rudo Camera", 
                               SDL_WINDOWPOS_UNDEFINED, 
                               SDL_WINDOWPOS_UNDEFINED, 
                               width_, 
                               height_, 
                               SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS | SDL_WINDOW_FULLSCREEN_DESKTOP); // <-- AGGRESSIVE FLAGS
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
    
    // Create the persistent texture matching the requested size
    videoTexture_ = SDL_CreateTexture(renderer_, 
                                       SDL_PIXELFORMAT_BGR24,
                                       SDL_TEXTUREACCESS_STREAMING, 
                                       width_, 
                                       height_);
    if (!videoTexture_) {
        std::cerr << "[GUIDisplay] Texture could not be created! Error: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Hide mouse cursor
    SDL_ShowCursor(SDL_DISABLE);

    std::cout << "[GUIDisplay] Initialized at " << width_ << "x" << height_ << "." << std::endl;
    return true;
}

void GUIDisplay::renderFrame(const cv::Mat &frame) {
    if (!renderer_ || !videoTexture_ || frame.empty()) {
        return;
    }
    
    // Check if the frame size matches the expected size before updating the texture
    if (frame.cols != width_ || frame.rows != height_) {
        std::cerr << "[GUIDisplay] ERROR: Frame size (" << frame.cols << "x" << frame.rows 
                  << ") does not match expected texture size (" << width_ << "x" << height_ << ")! Check CameraApp resize logic." << std::endl;
        return;
    }
    
    // Update texture
    SDL_UpdateTexture(videoTexture_, 
                      NULL, 
                      frame.data, 
                      frame.cols * frame.elemSize());

    // Clear screen
    SDL_SetRenderDrawColor(renderer_, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(renderer_);

    // Copy texture to full screen (NULL, NULL ensures max stretch)
    SDL_RenderCopy(renderer_, videoTexture_, NULL, NULL); 
}

void GUIDisplay::drawUIOverlays() {
    // UI logic
}

void GUIDisplay::present() {
    if (renderer_) {
        SDL_RenderPresent(renderer_); 
    }
}