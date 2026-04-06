#include "screen.h"
#include "main.h" 
#include "camera_app.h" 
#include "gui_display.h" 
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

extern std::atomic<bool> keep_running; 

Screen::Screen(GUIDisplay* display, CameraApp* app) 
    : guiDisplay_(display), cameraApp_(app) 
{
}

void Screen::handleSDLEvents(SDL_Event &event)
{
    if (event.type == SDL_QUIT)
    {
        LOG_STT("SDL Quit signal received.");
        keep_running = false;
    }
    else if (event.type == SDL_KEYDOWN)
    {
        if (event.key.keysym.sym == SDLK_ESCAPE)
        {
             keep_running = false;
        }
    }
}

void Screen::guiThread()
{
    if (!guiDisplay_->initialize(480, 320))
    {
        LOG_ERR("[LOG_ERROR] GUI initialization failed. Cannot start rendering loop.");
        keep_running = false;
        return;
    }

    if (!cameraApp_->startVideoStream(480, 320))
    { 
        LOG_ERR("Failed to start video stream.");
        keep_running = false;
        return;
    }

    cv::Mat previewFrame; 
    SDL_Event event;
    
    while (keep_running) {
        while (SDL_PollEvent(&event))
        {
            handleSDLEvents(event);
        }

        if (cameraApp_->getLatestPreviewFrame(previewFrame))
        {
            auto state = cameraApp_->getSharedState();
            if (state && !previewFrame.empty()) {
                static uint32_t lastTextUpdate = 0;
                static std::string ss_str, iso_str, ap_str;
                uint32_t currentTicks = SDL_GetTicks();
                
                if (currentTicks - lastTextUpdate >= 100) {
                    int ss_val = state->shutterSpeed.load();
                    ss_str = "1/" + std::to_string(ss_val);
                    iso_str = "ISO " + std::to_string(state->iso.load());
                    
                    std::ostringstream ap_stream;
                    ap_stream << "f/" << std::fixed << std::setprecision(1) << state->aperture.load();
                    ap_str = ap_stream.str();
                    lastTextUpdate = currentTicks;
                }
                
                // Helper lambda for text outline
                auto drawTextWithOutline = [&](cv::Mat& img, const std::string& text, cv::Point pt, double scale, int thickness) {
                    // Draw black outline
                    cv::putText(img, text, cv::Point(pt.x+1, pt.y+1), cv::FONT_HERSHEY_SIMPLEX, scale, cv::Scalar(0, 0, 0), thickness+1, cv::LINE_AA);
                    cv::putText(img, text, cv::Point(pt.x-1, pt.y-1), cv::FONT_HERSHEY_SIMPLEX, scale, cv::Scalar(0, 0, 0), thickness+1, cv::LINE_AA);
                    cv::putText(img, text, cv::Point(pt.x-1, pt.y+1), cv::FONT_HERSHEY_SIMPLEX, scale, cv::Scalar(0, 0, 0), thickness+1, cv::LINE_AA);
                    cv::putText(img, text, cv::Point(pt.x+1, pt.y-1), cv::FONT_HERSHEY_SIMPLEX, scale, cv::Scalar(0, 0, 0), thickness+1, cv::LINE_AA);
                    // Draw white text
                    cv::putText(img, text, pt, cv::FONT_HERSHEY_SIMPLEX, scale, cv::Scalar(255, 255, 255), thickness, cv::LINE_AA);
                };

                // 1. Battery Indicator Top Right
                int screen_w = previewFrame.cols;
                int screen_h = previewFrame.rows;
                drawTextWithOutline(previewFrame, "100%", cv::Point(screen_w - 60, 30), 0.5, 1);
                
                // 2. Exposure Values Bottom Right
                int bottom_y = screen_h - 20;
                drawTextWithOutline(previewFrame, ap_str,  cv::Point(screen_w - 70, bottom_y), 0.5, 1);
                drawTextWithOutline(previewFrame, ss_str,  cv::Point(screen_w - 70, bottom_y - 25), 0.5, 1);
                drawTextWithOutline(previewFrame, iso_str, cv::Point(screen_w - 70, bottom_y - 50), 0.5, 1);
            }

            guiDisplay_->renderFrame(previewFrame);  
            guiDisplay_->drawUIOverlays(state); 
            guiDisplay_->present();
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    
    cameraApp_->stopVideoStream(); 
    LOG_STT(":::::::::::: <--- GUI RENDERER END ---> ::::::::::::");
}