#ifndef CAMERA_CONTROL_H
#define CAMERA_CONTROL_H

#include <memory>
#include <libcamera/camera.h>
#include <libcamera/camera_manager.h>
#include <libcamera/control_ids.h>
#include <iostream>
#include <log.h>

class CameraControl {
public:
    // Constructor with default parameters
    CameraControl(
                int iso = 800, 
                int shutterSpeed = 500000, 
                int exposureMode = 1, 
                float aperture = 1.8,
                float flashPower = 1.0);
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
    void addMetadata(const std::string &filePath);

    std::unique_ptr<libcamera::CameraManager> cameraManager_;
    std::shared_ptr<libcamera::Camera> camera_;

    // Camera settings buffer
    int iso_;
    int shutterSpeed_;
    int exposureMode_;
};

#endif // CAMERA_CONTROL_H
