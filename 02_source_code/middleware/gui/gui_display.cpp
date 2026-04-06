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

    int render_w = 0, render_h = 0;
    SDL_GetRendererOutputSize(renderer_, &render_w, &render_h);
    if (render_w == 0 || render_h == 0) {
        render_w = width_;
        render_h = height_;
    }

    // 1. Grid & Focus Area
    SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);
    
    // 30% Opacity lines for 3x3 Grid (255 * 0.3 = 76)
    SDL_SetRenderDrawColor(renderer_, 255, 255, 255, 76);
    
    int thirdW = render_w / 3;
    int thirdH = render_h / 3;

    // Vertical lines
    SDL_RenderDrawLine(renderer_, thirdW, 0, thirdW, render_h);
    SDL_RenderDrawLine(renderer_, 2 * thirdW, 0, 2 * thirdW, render_h);

    // Horizontal lines
    SDL_RenderDrawLine(renderer_, 0, thirdH, render_w, thirdH);
    SDL_RenderDrawLine(renderer_, 0, 2 * thirdH, render_w, 2 * thirdH);

    // Center Focus Square (10% of screen height, 40% opacity = 102)
    int squareSize = render_h / 10;
    SDL_Rect focusRect = {
        (render_w - squareSize) / 2,
        (render_h - squareSize) / 2,
        squareSize,
        squareSize
    };
    SDL_SetRenderDrawColor(renderer_, 255, 255, 255, 102);
    SDL_RenderDrawRect(renderer_, &focusRect);

    // 2. Shutter Blink Animation (top-most layer)
    bool isCapturing = state->captureTriggered.load();
    if (isCapturing && blinkStartTime_ == 0) {
        blinkStartTime_ = SDL_GetTicks();
        state->captureTriggered.store(false); 
    }

    if (blinkStartTime_ != 0) {
        uint32_t elapsed = SDL_GetTicks() - blinkStartTime_;
        if (elapsed <= 200) { // Blink lasts 200ms
            float progress;
            if (elapsed <= 100) { // Closing
                progress = elapsed / 100.0f;
            } else {              // Opening
                progress = (200 - elapsed) / 100.0f; 
            }
            
            int rectHeight = static_cast<int>((render_h / 2.0f) * progress);
            
            SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255); // Solid black
            
            // Top rect
            SDL_Rect topRect = {0, 0, render_w, rectHeight};
            SDL_RenderFillRect(renderer_, &topRect);
            
            // Bottom rect
            SDL_Rect btmRect = {0, render_h - rectHeight, render_w, rectHeight};
            SDL_RenderFillRect(renderer_, &btmRect);
        } else {
            blinkStartTime_ = 0; // Reset
        }
    }
}

void GUIDisplay::present() 
{
    if (renderer_)
    {
        SDL_RenderPresent(renderer_); 
    }
}