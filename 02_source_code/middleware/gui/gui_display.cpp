#include "gui_display.h"
#include <iostream>
#include <iomanip>
#include <sstream>

GUIDisplay::GUIDisplay() = default;

GUIDisplay::~GUIDisplay()
{
    if (videoTexture_)
    {
        SDL_DestroyTexture(videoTexture_);
    }
    if (renderer_)
    {
        SDL_DestroyRenderer(renderer_);
    }
    if (window_)
    {
        SDL_DestroyWindow(window_);
    }
    SDL_Quit();
    std::cout << "[GUIDisplay] Cleaned up SDL resources." << std::endl;
}

bool GUIDisplay::initialize(int width, int height)
{
    // Store configuration
    width_ = width;
    height_ = height;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0)
    {
        std::cerr << "[GUIDisplay] SDL could not initialize! Error: " << SDL_GetError() << std::endl;
        return false;
    }

    window_ = SDL_CreateWindow("Rudo Camera", 
                               SDL_WINDOWPOS_UNDEFINED, 
                               SDL_WINDOWPOS_UNDEFINED, 
                               width_, 
                               height_, 
                               SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS | SDL_WINDOW_FULLSCREEN_DESKTOP); // <-- AGGRESSIVE FLAGS
    if (!window_)
    {
        std::cerr << "[GUIDisplay] Window could not be created! Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Create hardware-accelerated renderer
    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer_)
    {
        std::cerr << "[GUIDisplay] Renderer could not be created! Error: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Create the persistent texture matching the requested size
    videoTexture_ = SDL_CreateTexture(renderer_, 
                                       SDL_PIXELFORMAT_BGR24,
                                       SDL_TEXTUREACCESS_STREAMING, 
                                       width_, 
                                       height_);
    if (!videoTexture_)
    {
        std::cerr << "[GUIDisplay] Texture could not be created! Error: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Hide mouse cursor
    SDL_ShowCursor(SDL_DISABLE);

    std::cout << "[GUIDisplay] Initialized at " << width_ << "x" << height_ << "." << std::endl;
    return true;
}

void GUIDisplay::renderFrame(const cv::Mat &frame)
{
    if (!renderer_ || !videoTexture_ || frame.empty())
    {
        return;
    }
    
    // Check if the frame size matches the expected size before updating the texture
    if (frame.cols != width_ || frame.rows != height_)
    {
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

void GUIDisplay::drawUIOverlays(std::shared_ptr<CameraState> state) 
{
    if (!renderer_ || !state) return;

    // 1. Shutter Blink Animation
    bool isCapturing = state->captureTriggered.load();
    if (isCapturing && blinkStartTime_ == 0) {
        blinkStartTime_ = SDL_GetTicks();
        state->captureTriggered.store(false); 
    }

    if (blinkStartTime_ != 0) {
        uint32_t elapsed = SDL_GetTicks() - blinkStartTime_;
        if (elapsed < 150) { // Blink lasts 150ms
            SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer_, 255, 255, 255, 200); // White flash
            SDL_Rect screenRect = {0, 0, width_, height_};
            SDL_RenderFillRect(renderer_, &screenRect);
        } else {
            blinkStartTime_ = 0; // Reset
        }
    }

    // 2. 3x3 Rule-of-Thirds Grid
    SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer_, 255, 255, 255, 100); // Semi-transparent white
    
    int thirdW = width_ / 3;
    int thirdH = height_ / 3;

    // Vertical lines
    SDL_RenderDrawLine(renderer_, thirdW, 0, thirdW, height_);
    SDL_RenderDrawLine(renderer_, 2 * thirdW, 0, 2 * thirdW, height_);

    // Horizontal lines
    SDL_RenderDrawLine(renderer_, 0, thirdH, width_, thirdH);
    SDL_RenderDrawLine(renderer_, 0, 2 * thirdH, width_, 2 * thirdH);

    // 3. Real-time Metadata Sidebar Background
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 150); // Semi-transparent black sidebar
    SDL_Rect sidebarRect = {width_ - 120, 0, 120, height_};
    SDL_RenderFillRect(renderer_, &sidebarRect);

    // Note: SDL2 lacks native text rendering without SDL_ttf.
    // Text rendering for ISO/SS/Aperture will be pushed directly onto the frame 
    // via OpenCV in renderFrame(), or we rely entirely on the visual sidebar indicators.
}

void GUIDisplay::present() 
{
    if (renderer_)
    {
        SDL_RenderPresent(renderer_); 
    }
}