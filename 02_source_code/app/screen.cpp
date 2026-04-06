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
                
                // Refresh exactly once every second
                if (currentTicks - lastTextUpdate >= 1000 || lastTextUpdate == 0) {
                    int ss_val = state->shutterSpeed.load();
                    ss_str = "1/" + std::to_string(ss_val);
                    iso_str = "ISO 1000"; // Fixed as per requirements
                    ap_str = "f/2.8"; // Fixed as per requirements
                    lastTextUpdate = currentTicks;
                }
                
                // Helper lambda for text outline
                auto drawTextWithOutline = [&](cv::Mat& img, const std::string& text, cv::Point pt, double scale, int thickness) {
                    // Draw 1-px black shadow
                    cv::putText(img, text, cv::Point(pt.x+1, pt.y+1), cv::FONT_HERSHEY_SIMPLEX, scale, cv::Scalar(0, 0, 0), thickness, cv::LINE_AA);
                    // Draw white text
                    cv::putText(img, text, pt, cv::FONT_HERSHEY_SIMPLEX, scale, cv::Scalar(255, 255, 255), thickness, cv::LINE_AA);
                };

                int screen_w = previewFrame.cols;
                int screen_h = previewFrame.rows;

                // 1. Top Status Indicators
                int top_y = 30;
                drawTextWithOutline(previewFrame, "AUTO WB",   cv::Point(20, top_y), 0.4, 1);
                drawTextWithOutline(previewFrame, "AUTO EV",   cv::Point(screen_w/2 - 30, top_y), 0.4, 1);
                drawTextWithOutline(previewFrame, "AUTO ISO",  cv::Point(screen_w - 200, top_y), 0.4, 1);

                // 2. Graphical Battery Indicator Top Right (75% Green)
                int batX = screen_w - 50;
                int batY = 15;
                int batW = 35;
                int batH = 15;
                int nubW = 3;
                int nubH = 6;
                cv::Scalar shadow(0, 0, 0);
                cv::Scalar white(255, 255, 255);
                cv::Scalar vibrantGreen(0, 255, 0); // BGR format, so (0, 255, 0)
                
                // Shadow
                cv::rectangle(previewFrame, cv::Rect(batX+1, batY+1, batW, batH), shadow, 1, cv::LINE_AA);
                cv::rectangle(previewFrame, cv::Rect(batX + batW + 1, batY + (batH - nubH)/2 + 1, nubW, nubH), shadow, cv::FILLED, cv::LINE_AA);
                
                // Fill (representing 75% charge)
                int fillW = static_cast<int>(batW * 0.75f);
                cv::rectangle(previewFrame, cv::Rect(batX, batY, fillW, batH), vibrantGreen, cv::FILLED, cv::LINE_AA);
                
                // Outline
                cv::rectangle(previewFrame, cv::Rect(batX, batY, batW, batH), white, 1, cv::LINE_AA);
                // Nub
                cv::rectangle(previewFrame, cv::Rect(batX + batW, batY + (batH - nubH)/2, nubW, nubH), white, cv::FILLED, cv::LINE_AA);

                // 3. Middle Left Lightning Bolt (Flash)
                int mx = 20;
                int my = screen_h / 2;
                std::vector<cv::Point> boltPts = {
                    cv::Point(mx + 5, my - 10),
                    cv::Point(mx - 5, my + 2),
                    cv::Point(mx + 2, my + 2),
                    cv::Point(mx - 2, my + 15),
                    cv::Point(mx + 8, my),
                    cv::Point(mx + 1, my),
                    cv::Point(mx + 5, my - 10)
                };
                
                // Draw drop shadow
                std::vector<cv::Point> boltPtsShadow;
                for (auto& p : boltPts) boltPtsShadow.push_back(cv::Point(p.x + 1, p.y + 1));
                cv::fillPoly(previewFrame, std::vector<std::vector<cv::Point>>{boltPtsShadow}, shadow, cv::LINE_AA);
                // Draw white bolt
                cv::fillPoly(previewFrame, std::vector<std::vector<cv::Point>>{boltPts}, white, cv::LINE_AA);

                // 4. Bottom Navigation & Mode Icons
                int bottom_y = screen_h - 20;
                drawTextWithOutline(previewFrame, "MF", cv::Point(20, bottom_y), 0.5, 1);
                drawTextWithOutline(previewFrame, "M",  cv::Point(screen_w/2 - 10, bottom_y), 0.6, 2);

                // 5. Exposure Values Bottom Right
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