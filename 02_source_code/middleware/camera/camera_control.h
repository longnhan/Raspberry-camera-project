#ifndef CAMERA_CONTROL_H
#define CAMERA_CONTROL_H

#include <memory>
#include <libcamera/libcamera.h>
#include <iostream>
#include "log.h"

class CameraControl {
public:
    CameraControl(int iso = 800, 
                  int shutterSpeed = 500000, 
                  int exposureMode = 1, 
                  float aperture = 1.8,
                  float flashPower = 1.0);
    virtual ~CameraControl();

    virtual bool initialize();
    virtual void setISO(int iso);
    virtual void setShutterSpeed(int speed);
    virtual void setExposure(int exposure);
    virtual bool captureImage(const std::string &image_path);
    virtual void release();

protected: // <--- CHANGED FROM PRIVATE TO PROTECTED
    std::unique_ptr<libcamera::CameraManager> cameraManager_;
    std::shared_ptr<libcamera::Camera> camera_;

    // Camera settings buffer
    int iso_;
    int shutterSpeed_;
    int exposureMode_;

protected: // Changed from private so derived apps can access metadata helpers
    libcamera::ControlList prepareControls();
    bool openCV_JPG_Conversion(void *mappedMemory, int width, int height, int stride, const std::string &image_path, size_t length);
    void addMetadata(const std::string &filePath);
};

#endif // CAMERA_CONTROL_H