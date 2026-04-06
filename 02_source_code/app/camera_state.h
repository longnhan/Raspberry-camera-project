#pragma once
#include <atomic>

struct CameraState {
    std::atomic<int> iso{0};
    std::atomic<int> shutterSpeed{0};
    std::atomic<float> aperture{0.0f};
    std::atomic<bool> captureTriggered{false};
};
