#ifndef CAMERA_APP_H
#define CAMERA_APP_H

#include "camera_control.h" // Fixed include filename
#include "cameraSetting.h" 
#include <opencv2/opencv.hpp>
#include <libcamera/camera.h> 
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <vector>
#include <memory> 

class CameraApp : public CameraControl {
public:
    static CameraApp* getInstance();
    
    CameraApp(ISO iso, ShutterSpeed shuttleSpeed, ExposureMode exposureMode, Aperture aperture, FlashPower flashPower);
    ~CameraApp();

    bool initialize() override;
    void setISO(int iso) override;
    void setShutterSpeed(int shutterSpeed) override;
    void setExposure(int exposureMode) override;
    
    // Capture Image (Parallel Mode)
    bool captureImage(const std::string &image_path) override;
    
    // Video Streaming
    bool startVideoStream(int width, int height);
    void stopVideoStream();
    bool getLatestPreviewFrame(cv::Mat &outputFrame);

    void release() override; // Helper to force release

    // Internal Callback
    static void requestComplete(libcamera::Request *request);

private:
    static CameraApp* instance_;
    
    // Stream State
    bool streamRunning_ = false;
    
    // Configuration
    std::unique_ptr<libcamera::CameraConfiguration> previewConfig_;
    
    std::vector<libcamera::Request *> requests_to_recycle_;
    
    // Dual Streams
    libcamera::Stream *previewStream_ = nullptr; // Stream 0 (Video)
    libcamera::Stream *stillStream_ = nullptr;   // Stream 1 (Photo)
    
    // Capture State
    std::atomic<bool> take_picture_request_ = false; 

    // Frame Sync
    cv::Mat currentPreviewFrame_;
    std::mutex videoMutex_;
    std::condition_variable videoCv_;
};

#endif // CAMERA_APP_H