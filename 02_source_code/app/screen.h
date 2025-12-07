#pragma once

#include <SDL2/SDL.h>
#include <thread>
#include <atomic>

class GUIDisplay;
class CameraApp; 

/**
 * @brief Manages the dedicated GUI thread and high-level display logic.
 * Located in app/
 */
class Screen{
public:
    Screen(GUIDisplay* display, CameraApp* app);
    ~Screen() = default;

    /**
     * @brief The entry point for the dedicated GUI rendering thread.
     * This function contains the main rendering and event loop.
     */
    void guiThread();

    /**
     * @brief Processes SDL events (touch/keyboard) and forwards them to the app logic.
     */
    void handleSDLEvents(SDL_Event &event);

private:
    GUIDisplay* guiDisplay_ = nullptr;
    CameraApp* cameraApp_ = nullptr;
};