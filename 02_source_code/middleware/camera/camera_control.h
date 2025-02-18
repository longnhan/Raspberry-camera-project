#ifndef CAMERA_CONTROL_H
#define CAMERA_CONTROL_H

#include <memory>
#include <libcamera/camera.h>
#include <libcamera/camera_manager.h>
#include <libcamera/control_ids.h>
#include <iostream>

class CameraControl {
public:
    CameraControl();
    ~CameraControl();

    // Initialize libcamera and set up the camera
    bool initialize();

    // Set camera parameters
    void setISO(int iso);
    void setShutterSpeed(int speed);
    void setExposure(int exposure);

    // Capture an image
    bool captureImage();

    // Release resources
    void release();

private:
    // Helper: Prepare a control list from current settings.
    libcamera::ControlList prepareControls();

    std::unique_ptr<libcamera::CameraManager> cameraManager_;
    std::shared_ptr<libcamera::Camera> camera_;

    // Camera settings buffer
    int iso_;           // ISO sensitivity
    int shutterSpeed_;  // Shutter speed in microseconds
    int exposureMode_;  // 0 = Auto, 1 = Manual
};

#endif // CAMERA_CONTROL_H
