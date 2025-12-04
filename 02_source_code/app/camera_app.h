#ifndef _CAMERA_APP_H_
#define _CAMERA_APP_H_


/*
** @brief Camera application control logic camera middlware 
**
*/

#include <fstream>
#include "camera_control.h"
#include "photo_management.h"
#include "cameraSetting.h"
#include <opencv2/opencv.hpp>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <queue> 
#include "log.h"

class CameraApp: public CameraControl
{
    public:
        CameraApp(ISO iso = ISO()
        , ShutterSpeed shuttleSpeed = SHUTTLE_SPEED() 
        , ExposureMode exposureMode = EXPOSURE_MODE()
        , Aperture aperture = APERTURE()
        , FlashPower flashPower = FLASH_POWER());

        virtual ~CameraApp();
        bool initialize();

        // --- Core Streaming Methods ---
        bool startVideoStream(int width, int height); 
        void stopVideoStream();
        // Synchronized accessor for the GUI Thread to get the frame
        bool getLatestPreviewFrame(cv::Mat &outputFrame);
        // Asynchronous handler (run by libcamera's internal thread)
        static void requestComplete(libcamera::Request *request);
        // Helper to get the instance from the static callback
        static CameraApp* getInstance();

        // Set camera parameters
        void setISO(int iso);
        void setShutterSpeed(int shutterSpeed);
        void setExposure(int exposureMode);
    
        // Capture an image
        bool captureImage(const std::string &image_path);
    
        // Release resources
        void release();
    
    private:
        // Core Streaming Resources
        std::unique_ptr<libcamera::CameraConfiguration> previewConfig_;
        std::shared_ptr<libcamera::Stream> previewStream_ = nullptr;
        
        // Synchronization resources for the frame buffer
        cv::Mat currentPreviewFrame_; 
        std::mutex videoMutex_;
        std::condition_variable videoCv_;
        std::atomic<bool> streamRunning_ = false;

        // Singleton instance pointer needed for the static libcamera callback
        static CameraApp* instance_;
        // virtual void setISO(int iso) override final;
        // virtual void setShutterSpeed(int speed) override final;
        // virtual void setExposure(int exposure) override final;
    

};

#endif //_CAMERA_APP_H_
