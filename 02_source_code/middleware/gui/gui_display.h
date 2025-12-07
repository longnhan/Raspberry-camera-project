#pragma once

#include <SDL2/SDL.h>
#include <opencv2/opencv.hpp>

/**
 * @brief Low-level SDL2 handler for video rendering.
 * Located in middleware/gui/.
 * Now resolution-agnostic.
 */
class GUIDisplay {
public:
    GUIDisplay();
    ~GUIDisplay();

    /**
     * @brief Initializes SDL, creates the window, renderer, and video texture.
     * @param width The desired screen width (set by App layer).
     * @param height The desired screen height (set by App layer).
     * @return true on success, false otherwise.
     */
    bool initialize(int width, int height);

    /**
     * @brief Updates the video texture with the new frame data and renders it.
     * @param frame The BGR cv::Mat containing the latest video frame.
     */
    void renderFrame(const cv::Mat &frame);

    /**
     * @brief Draws simple UI elements (simplified, no parameters).
     */
    void drawUIOverlays(); 

    /**
     * @brief Finalizes the frame by calling SDL_RenderPresent().
     */
    void present(); 

private:
    SDL_Window *window_ = nullptr;
    SDL_Renderer *renderer_ = nullptr;
    SDL_Texture *videoTexture_ = nullptr;

    // Configured at runtime via initialize()
    int width_ = 0;
    int height_ = 0;
};