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
                // Determine denominator for shutter speed text
                int ss_val = state->shutterSpeed.load();
                std::string ss_str = "1/" + std::to_string(ss_val);
                
                std::string iso_str = "ISO " + std::to_string(state->iso.load());
                
                std::ostringstream ap_stream;
                ap_stream << "F" << std::fixed << std::setprecision(1) << state->aperture.load();
                std::string ap_str = ap_stream.str();
                
                // Draw text in top right area
                int x_offset = previewFrame.cols - 110; // match sidebar width
                cv::putText(previewFrame, iso_str, cv::Point(x_offset, 30), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 255, 255), 1, cv::LINE_AA);
                cv::putText(previewFrame, ss_str,  cv::Point(x_offset, 60), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 255, 255), 1, cv::LINE_AA);
                cv::putText(previewFrame, ap_str,  cv::Point(x_offset, 90), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 255, 255), 1, cv::LINE_AA);
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