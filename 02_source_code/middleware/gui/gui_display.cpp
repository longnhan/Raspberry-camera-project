#include "gui_display.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>

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
    
    // 50% Opacity lines for 3x3 Grid (255 * 0.5 = 128)
    SDL_SetRenderDrawColor(renderer_, 255, 255, 255, 128);
    
    int thirdW = render_w / 3;
    int thirdH = render_h / 3;

    // Vertical lines (double thickness)
    SDL_RenderDrawLine(renderer_, thirdW, 0, thirdW, render_h);
    SDL_RenderDrawLine(renderer_, thirdW + 1, 0, thirdW + 1, render_h);
    SDL_RenderDrawLine(renderer_, 2 * thirdW, 0, 2 * thirdW, render_h);
    SDL_RenderDrawLine(renderer_, 2 * thirdW + 1, 0, 2 * thirdW + 1, render_h);

    // Horizontal lines (double thickness)
    SDL_RenderDrawLine(renderer_, 0, thirdH, render_w, thirdH);
    SDL_RenderDrawLine(renderer_, 0, thirdH + 1, render_w, thirdH + 1);
    SDL_RenderDrawLine(renderer_, 0, 2 * thirdH, render_w, 2 * thirdH);
    SDL_RenderDrawLine(renderer_, 0, 2 * thirdH + 1, render_w, 2 * thirdH + 1);

    // Center Focus Reticle (12.5% of screen height)
    int squareSize = render_h / 8;
    int cx = render_w / 2;
    int cy = render_h / 2;
    int span = squareSize / 2;
    int len = squareSize / 3;

    // Helper lambda to draw the reticle with double thickness lines
    auto drawReticle = [&](int offsetX, int offsetY, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
        SDL_SetRenderDrawColor(renderer_, r, g, b, a);
        int ox = cx + offsetX;
        int oy = cy + offsetY;

        // Draw each line twice for thickness
        for(int k=0; k<=1; k++) {
            // Top-Left corner
            SDL_RenderDrawLine(renderer_, ox - span, oy - span + k, ox - span + len, oy - span + k);
            SDL_RenderDrawLine(renderer_, ox - span + k, oy - span, ox - span + k, oy - span + len);

            // Top-Right corner
            SDL_RenderDrawLine(renderer_, ox + span - len, oy - span + k, ox + span, oy - span + k);
            SDL_RenderDrawLine(renderer_, ox + span + k, oy - span, ox + span + k, oy - span + len);

            // Bottom-Left corner
            SDL_RenderDrawLine(renderer_, ox - span, oy + span + k, ox - span + len, oy + span + k);
            SDL_RenderDrawLine(renderer_, ox - span + k, oy + span - len, ox - span + k, oy + span);

            // Bottom-Right corner
            SDL_RenderDrawLine(renderer_, ox + span - len, oy + span + k, ox + span, oy + span + k);
            SDL_RenderDrawLine(renderer_, ox + span + k, oy + span - len, ox + span + k, oy + span);

            // Center Crosshair (slightly larger, 6px radius)
            int cross = 6;
            SDL_RenderDrawLine(renderer_, ox - cross, oy + k, ox + cross, oy + k);
            SDL_RenderDrawLine(renderer_, ox + k, oy - cross, ox + k, oy + cross);
        }
    };

    // Draw shadow first (40% opacity black, +1 offset), then white (40% opacity)
    drawReticle(1, 1, 0, 0, 0, 102); 
    drawReticle(0, 0, 255, 255, 255, 102);

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
            if (elapsed <= 100) { // Closing (progress 0 -> 1)
                progress = elapsed / 100.0f;
            } else {              // Opening (progress 1 -> 0)
                progress = (200 - elapsed) / 100.0f; 
            }
            
            float maxR = std::sqrt((render_w/2.0f)*(render_w/2.0f) + (render_h/2.0f)*(render_h/2.0f));
            float currentR = maxR * (1.0f - progress);
            
            SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255); // Solid black
            
            for (int y = 0; y < render_h; ++y) {
                int dy = y - cy;
                float dy2 = static_cast<float>(dy * dy);
                float r2 = currentR * currentR;
                
                if (dy2 >= r2) {
                    // Line is fully outside the clear circle, draw entirely black
                    SDL_RenderDrawLine(renderer_, 0, y, render_w, y);
                } else {
                    // Line intersects the circle
                    int dx = static_cast<int>(std::sqrt(r2 - dy2));
                    int leftEdge = cx - dx;
                    int rightEdge = cx + dx;
                    
                    if (leftEdge > 0) {
                        SDL_RenderDrawLine(renderer_, 0, y, leftEdge, y);
                    }
                    if (rightEdge < render_w) {
                        SDL_RenderDrawLine(renderer_, rightEdge, y, render_w, y);
                    }
                }
            }
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