#pragma once

#include <SDL2/SDL.h>
#include <opencv2/opencv.hpp>

/**
 * @brief Low-level SDL2 handler for the 480x320 LCD and video rendering.
 * Located in middleware/gui/.
 */
class GUIDisplay {
public:
    GUIDisplay();
    ~GUIDisplay();

    // Screen dimensions based on 3.5" Waveshare
    static constexpr int SCREEN_WIDTH = 480;
    static constexpr int SCREEN_HEIGHT = 320;

    /**
     * @brief Initializes SDL, creates the window, renderer, and video texture.
     * @return true on success, false otherwise.
     */
    bool initialize();

    /**
     * @brief Updates the video texture with the new frame data and renders it.
     * @param frame The BGR cv::Mat containing the latest video frame.
     */
    void renderFrame(const cv::Mat &frame);

    /**
     * @brief Draws simple UI elements (simplified, no parameters).
     */
    void drawUIOverlays(); // FIX: Removed parameters

    /**
     * @brief Finalizes the frame by calling SDL_RenderPresent().
     */
    void present(); // FIX: Added missing method

private:
    SDL_Window *window_ = nullptr;
    SDL_Renderer *renderer_ = nullptr;
    SDL_Texture *videoTexture_ = nullptr;
};