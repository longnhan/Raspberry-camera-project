#ifndef _CAMERA_APP_H_
#define _CAMERA_APP_H_

#include "camera_control.h"
#include "cameraSetting.h" 
#include <opencv2/opencv.hpp>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <queue> 
#include "log.h" 

class CameraApp : public CameraControl
{
public:
    CameraApp(ISO iso = ISO(), 
              ShutterSpeed shuttleSpeed = SHUTTLE_SPEED(),
              ExposureMode exposureMode = EXPOSURE_MODE(),
              Aperture aperture = APERTURE(),
              FlashPower flashPower = FLASH_POWER());
    
    virtual ~CameraApp();

    bool initialize();

    // --- Core Streaming Methods ---
    bool startVideoStream(int width, int height); 
    void stopVideoStream();
    
    bool getLatestPreviewFrame(cv::Mat &outputFrame);
    
    // Libcamera callback
    static void requestComplete(libcamera::Request *request);
    static CameraApp* getInstance();

    // Wrappers
    void setISO(int iso);
    void setShutterSpeed(int shutterSpeed);
    void setExposure(int exposureMode);
    bool captureImage(const std::string &image_path);
    void release();

private:
    static CameraApp* instance_; 

    // Core Streaming Resources
    std::unique_ptr<libcamera::CameraConfiguration> previewConfig_;
    
    // FIX: Must be a raw pointer.
    libcamera::Stream *previewStream_ = nullptr; 
    
    // Synchronization resources
    cv::Mat currentPreviewFrame_; 
    std::mutex videoMutex_;
    std::condition_variable videoCv_;
    std::atomic<bool> streamRunning_ = false;
};

#endif //_CAMERA_APP_H_