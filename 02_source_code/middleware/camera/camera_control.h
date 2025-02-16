#ifndef _CAMERA_CONTROL_H_
#define _CAMERA_CONTROL_H_

#include <libcamera/camera_manager.h>
#include <libcamera/camera.h>
#include <memory>

class CameraControl {
public:
    CameraControl();
    ~CameraControl();
    
    bool init();
    void detectAndPrintCamera();

private:
    std::shared_ptr<libcamera::CameraManager> cameraManager;
};

#endif /*_CAMERA_CONTROL_H_*/