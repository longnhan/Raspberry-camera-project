#include "camera_app.h"
#include <chrono>

CameraApp* CameraApp::instance_ = nullptr;

CameraApp::CameraApp(ISO iso, 
                    ShutterSpeed shuttleSpeed,
                    ExposureMode exposureMode,
                    Aperture aperture,
                    FlashPower flashPower)
                   :CameraControl(cameraSetting::ISOConvert(iso), 
                                cameraSetting::ShutterSpeedConvert(shuttleSpeed),
                                cameraSetting::ExposureModeConvert(exposureMode),
                                cameraSetting::ApertureConvert(aperture),
                                cameraSetting::FlashPowerCOnvert(flashPower))

{
    PRINT_SETTINGS();
    instance_ = this;
}

CameraApp::~CameraApp()
{
    stopVideoStream();
    release();
    if (instance_ == this)
    {
        instance_ = nullptr;
    }
}

CameraApp* CameraApp::getInstance()
{
    return instance_;
}

bool CameraApp::initialize()
{
    return CameraControl::initialize();
}

// Set camera parameters (wrappers)
void CameraApp::setISO(int iso)
{
    CameraControl::setISO(iso);
}

void CameraApp::setShutterSpeed(int shutterSpeed)
{
    CameraControl::setShutterSpeed(shutterSpeed);
}

void CameraApp::setExposure(int exposureMode)
{
    CameraControl::setExposure(exposureMode);
}

// Capture an image (wrapper)
bool CameraApp::captureImage(const std::string &image_path)
{
    return CameraControl::captureImage(image_path);
}

// Release resources (wrapper)
void CameraApp::release()
{
    CameraControl::release();
}

// --- CORE STREAMING LOGIC ---
// Placeholder: Actual libcamera configuration and request queuing goes here.
bool CameraApp::startVideoStream(int width, int height) {
    if (streamRunning_) return true; // Already running

    // 1. Configure for Viewfinder role using camera_->generateConfiguration(...)
    // 2. Allocate FrameBuffers for the previewStream_
    // 3. Connect the requestComplete static callback: camera_->requestCompleted.connect(this, &CameraApp::requestComplete);
    // 4. Queue the initial set of requests
    
    streamRunning_ = true;
    LOG_STT("Video stream started.");
    return true;
}

void CameraApp::stopVideoStream() {
    if (!streamRunning_) return;

    // Disconnect callback, stop camera, and release buffers
    streamRunning_ = false;
    // Notify waiting GUI thread to exit cleanly
    videoCv_.notify_all(); 
    LOG_STT("Video stream stopped.");
}

// Synchronized accessor for the GUI Thread
bool CameraApp::getLatestPreviewFrame(cv::Mat &outputFrame) {
    std::unique_lock<std::mutex> lock(videoMutex_);
    
    // Wait until a frame is ready OR the stream is stopping
    videoCv_.wait(lock, [this]{ 
        return !currentPreviewFrame_.empty() || !streamRunning_; 
    });

    if (!streamRunning_) {
        return false;
    }
    
    // Copy the shared frame safely
    currentPreviewFrame_.copyTo(outputFrame);
    return true;
}

// Asynchronous handler run by libcamera's thread
void CameraApp::requestComplete(libcamera::Request *request) {
    CameraApp *self = CameraApp::getInstance();
    if (!self || !self->streamRunning_ || request->status() == libcamera::Request::RequestCancelled) {
        // Re-queue or handle cleanup if shutting down
        // self->camera_->queueRequest(request.release());
        return; 
    }

    // --- The essential pipeline happens here ---
    // 1. Map memory for the frame buffer.
    // 2. Use cv::cvtColor(NV12_Mat, tempBGR_Mat, cv::COLOR_YUV2BGR_NV12)
    
    // 3. Synchronized Frame Hand-off
    {
        std::lock_guard<std::mutex> lock(self->videoMutex_);
        // tempBGR_Mat.copyTo(self->currentPreviewFrame_);
        self->currentPreviewFrame_ = cv::Mat::zeros(480, 320, CV_8UC3); // Placeholder for conversion result
    }
    self->videoCv_.notify_one(); // Signal the GUI thread

    // 4. Unmap memory and re-queue the request: request->requeue(); 
    // self->camera_->queueRequest(request.release());
}