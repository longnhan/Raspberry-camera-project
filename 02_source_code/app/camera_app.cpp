#include "camera_app.h"
#include <chrono>
#include <sys/mman.h> 
#include <unistd.h>
#include <cstring> 
#include <libcamera/formats.h>
#include <libcamera/framebuffer_allocator.h>
#include <atomic> 
#include <condition_variable> 
#include <mutex>
#include <vector> 
#include <iostream>
#include <queue>
#include <libcamera/control_ids.h>

// --- STATIC MEMBER DEFINITION ---
CameraApp* CameraApp::instance_ = nullptr; 

namespace {
    int LocalISOConvert(int iso) { return iso; }
    int LocalShutterSpeedConvert(int speed) { return 1000000 / ((speed == 0) ? 1 : speed); }
    int LocalExposureModeConvert(int mode) { return mode; }
    float LocalApertureConvert(float aperture) { return aperture / 10.0f; }
    float LocalFlashPowerCOnvert(float power) { return (power == 0) ? power : (1.0f / power); }
}

// --- SYNC OBJECTS ---
static std::mutex capture_mutex;
static std::condition_variable capture_cv;
static bool capture_finished = false;
static std::string g_current_image_path;

CameraApp::CameraApp(ISO iso, ShutterSpeed shuttleSpeed, ExposureMode exposureMode, Aperture aperture, FlashPower flashPower)
    : CameraControl(LocalISOConvert(iso), LocalShutterSpeedConvert(shuttleSpeed), LocalExposureModeConvert(exposureMode), LocalApertureConvert(aperture), LocalFlashPowerCOnvert(flashPower))
{
    instance_ = this;
    if (camera_) camera_->release();
    if (sharedState_) {
        sharedState_->iso.store(LocalISOConvert(iso));
        sharedState_->shutterSpeed.store(shuttleSpeed); // keeping denominator for display
        sharedState_->aperture.store(LocalApertureConvert(aperture));
    }
}

CameraApp::~CameraApp() {
    stopVideoStream();
    if (camera_) camera_->release();
    if (instance_ == this) instance_ = nullptr;
}

CameraApp* CameraApp::getInstance() { return instance_; }
bool CameraApp::initialize() { return CameraControl::initialize(); }
void CameraApp::setISO(int iso) { 
    CameraControl::setISO(iso); 
    if (sharedState_) sharedState_->iso.store(iso);
}
void CameraApp::setShutterSpeed(int shutterSpeed) { 
    CameraControl::setShutterSpeed(LocalShutterSpeedConvert(shutterSpeed)); 
    if (sharedState_) sharedState_->shutterSpeed.store(shutterSpeed);
}
void CameraApp::setExposure(int exposureMode) { 
    CameraControl::setExposure(exposureMode); 
}

void CameraApp::release() { 
    if (camera_) camera_->release(); 
}

// =================================================================================
// --- CAPTURE LOGIC ---
// =================================================================================

bool CameraApp::captureImage(const std::string &image_path)
{ 
    if (!camera_ || !streamRunning_) return false;
    
    LOG_DBG("[App] Capture Requested.");
    
    {
        std::lock_guard<std::mutex> lock(capture_mutex);
        g_current_image_path = image_path;
        capture_finished = false;
        take_picture_request_ = true; 
        if (sharedState_) sharedState_->captureTriggered.store(true);
    }
    
    std::unique_lock<std::mutex> lock(capture_mutex);
    if (capture_cv.wait_for(lock, std::chrono::milliseconds(3000), []{ return capture_finished; }))
    {
        LOG_DBG("[App] Capture Success!");
        return true;
    }
    else
    {
        LOG_ERR("[App] Capture Timeout!");
        take_picture_request_ = false; 
        return false;
    }
}

// =================================================================================
// --- DUAL-STREAM SETUP (Full FOV + Stable Format) ---
// =================================================================================

static std::unique_ptr<libcamera::FrameBufferAllocator> g_allocator;
static std::queue<libcamera::FrameBuffer *> free_still_buffers; 
static std::mutex buffer_queue_mutex; 

bool CameraApp::startVideoStream(int width, int height)
{
    LOG_DBG("[Stream] startVideoStream (Dual-Pipeline) called.");

    if (!camera_) return false;
    if (streamRunning_) return true; 

    if (camera_->acquire() < 0) {
        LOG_ERR("[Stream] Acquire failed.");
        return false;
    }

    std::vector<libcamera::StreamRole> roles = { 
        libcamera::StreamRole::Viewfinder,   // Stream 0: Video
        libcamera::StreamRole::StillCapture  // Stream 1: Photo
    };
    
    previewConfig_ = camera_->generateConfiguration(roles);
    if (!previewConfig_ || previewConfig_->size() != 2) {
        LOG_ERR("[Stream] Failed to generate dual config.");
        return false;
    }

    // --- STREAM 0: VIDEO (Resolution for quality/FOV) ---
    libcamera::StreamConfiguration &videoConfig = previewConfig_->at(0);
    videoConfig.size = { 728, 544 }; 
    videoConfig.pixelFormat = libcamera::formats::YUYV; 
    videoConfig.bufferCount = 4;

    // --- STREAM 1: PHOTO ---
    libcamera::StreamConfiguration &stillConfig = previewConfig_->at(1);
    stillConfig.size = { 1456, 1088 }; 
    stillConfig.pixelFormat = libcamera::formats::NV12; 
    stillConfig.bufferCount = 2; 

    previewConfig_->validate(); 
    if (camera_->configure(previewConfig_.get()) < 0)
    {
        LOG_ERR("[Stream] Configure failed.");
        return false;
    }

    previewStream_ = videoConfig.stream();
    stillStream_   = stillConfig.stream();

    // Verify configuration
    LOG_DBG("[Stream] Video Configured: ", 
            previewStream_->configuration().size.width, "x", 
            previewStream_->configuration().size.height, " (YUYV)");

    g_allocator = std::make_unique<libcamera::FrameBufferAllocator>(camera_);
    if (g_allocator->allocate(previewStream_) < 0) return false;
    if (g_allocator->allocate(stillStream_) < 0) return false;

    // Map Memory
    auto map_buffers = [](libcamera::Stream *stream)
    {
        const auto &buffers = g_allocator->buffers(stream);
        for (const auto &buffer : buffers)
        {
            int fd = buffer->planes()[0].fd.get();
            size_t length = buffer->planes()[0].length;
            void *map = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
            if (map != MAP_FAILED)
            {
                memset(map, 0, length);
                munmap(map, length);
            }
        }
    };
    map_buffers(previewStream_);
    map_buffers(stillStream_);

    // Prepare Queue
    {
        std::lock_guard<std::mutex> lock(buffer_queue_mutex);
        while (!free_still_buffers.empty()) free_still_buffers.pop(); 
        for (const auto &buffer : g_allocator->buffers(stillStream_))
        {
            free_still_buffers.push(buffer.get());
        }
    }

    // Create Requests
    for (auto *req : requests_to_recycle_) delete req;
    requests_to_recycle_.clear();

    const auto &videoBuffers = g_allocator->buffers(previewStream_);
    for (const auto &buffer : videoBuffers)
    {
        libcamera::Request *request = camera_->createRequest().release();
        if (!request || request->addBuffer(previewStream_, buffer.get()) < 0) return false;
        request->controls().set(libcamera::controls::ExposureTime, shutterSpeed_);
        requests_to_recycle_.push_back(request);
    }

    // Start
    camera_->requestCompleted.disconnect(CameraApp::requestComplete); 
    camera_->requestCompleted.connect(CameraApp::requestComplete);

    if (camera_->start() < 0) return false;
    
    streamRunning_ = true;
    take_picture_request_ = false;

    for (libcamera::Request *request : requests_to_recycle_)
    {
        camera_->queueRequest(request);
    }
    
    LOG_DBG("[Stream] Started.");
    return true;
}

void CameraApp::stopVideoStream()
{
    if (!streamRunning_) return;
    streamRunning_ = false;

    if (camera_) {
        camera_->stop();
        for (auto *req : requests_to_recycle_) delete req;
        requests_to_recycle_.clear();
        camera_->requestCompleted.disconnect(CameraApp::requestComplete);
        camera_->release();
    }
    if (g_allocator) g_allocator.reset();
    previewConfig_.reset();
}

bool CameraApp::getLatestPreviewFrame(cv::Mat &outputFrame)
{
    std::unique_lock<std::mutex> lock(videoMutex_);
    if (!videoCv_.wait_for(lock, std::chrono::milliseconds(200), [this]{ return !currentPreviewFrame_.empty() || !streamRunning_; })) return false;
    if (!streamRunning_) return false;
    currentPreviewFrame_.copyTo(outputFrame);
    return true;
}

// =================================================================================
// --- CALLBACK ---
// =================================================================================

void CameraApp::requestComplete(libcamera::Request *request)
{
    CameraApp *self = CameraApp::getInstance();
    if (!self || !self->streamRunning_ || request->status() == libcamera::Request::RequestCancelled) return;

    // --- READ METADATA FOR EXIF ---
    const libcamera::ControlList &metadata = request->metadata();
    if (metadata.contains(libcamera::controls::AnalogueGain.id())) {
        auto gain = metadata.get(libcamera::controls::AnalogueGain);
        if (gain) {
            self->iso_ = static_cast<int>(*gain * 100.0f);
            if (self->sharedState_) self->sharedState_->iso.store(self->iso_);
        }
    }

    libcamera::FrameBuffer *videoBuffer = nullptr;

    // --- HANDLE VIDEO STREAM (YUYV -> BGR -> 480x320) ---
    if (request->buffers().find(self->previewStream_) != request->buffers().end())
    {
        videoBuffer = request->buffers().at(self->previewStream_);
        
        int fd = videoBuffer->planes()[0].fd.get();
        size_t totalLength = 0;
        for (const auto &plane : videoBuffer->planes()) totalLength += plane.length;

        void *map = mmap(NULL, totalLength, PROT_READ, MAP_SHARED, fd, 0);
        
        if (map != MAP_FAILED)
        {
            const libcamera::StreamConfiguration &config = self->previewStream_->configuration();
            int w = config.size.width;  // 728
            int h = config.size.height; // 544
            int stride = config.stride;

            cv::Mat yuyvMat(h, w, CV_8UC2, map, stride);
            
            cv::Mat bgrMat;
            cv::cvtColor(yuyvMat, bgrMat, cv::COLOR_YUV2BGR_YUYV);
            
            cv::Mat resizedMat;
            cv::resize(bgrMat, resizedMat, cv::Size(480, 320), 0, 0, cv::INTER_LINEAR); 

            {
                std::lock_guard<std::mutex> lock(self->videoMutex_);
                self->currentPreviewFrame_ = resizedMat; 
            }
            self->videoCv_.notify_one(); 
            munmap(map, totalLength);
        }
    }

    // --- HANDLE STILL STREAM (NV12) ---
    if (request->buffers().find(self->stillStream_) != request->buffers().end())
    {
        LOG_DBG("[Callback] High-Res Frame Received!");
        const libcamera::FrameBuffer *buffer = request->buffers().at(self->stillStream_);
        
        int fd = buffer->planes()[0].fd.get();
        size_t totalLength = 0;
        for (const auto &plane : buffer->planes()) totalLength += plane.length;
        
        void *map = mmap(NULL, totalLength, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        
        if (map != MAP_FAILED)
        {
            const libcamera::StreamConfiguration &config = self->stillStream_->configuration();
            int w = config.size.width;
            int h = config.size.height;
            int stride = config.stride;
            size_t required_size = (size_t)(h + h / 2) * stride;

            if (totalLength >= required_size)
            {
                try
                {
                    std::string path;
                    {
                        std::lock_guard<std::mutex> lock(capture_mutex);
                        path = g_current_image_path;
                    }
                    
                    if (!path.empty()) {
                        self->openCV_JPG_Conversion(map, w, h, stride, path, totalLength);
                        self->addMetadata(path);
                        map = nullptr; // openCV_JPG_Conversion handles unmapping internally
                    }
                } 
                catch (...)
                {
                    LOG_ERR("[Callback] Error converting image");
                }
            }
            if (map != MAP_FAILED && map != nullptr) {
                munmap(map, totalLength);
            }
        }

        {
            std::lock_guard<std::mutex> lock(buffer_queue_mutex);
            free_still_buffers.push((libcamera::FrameBuffer*)buffer);
        }
        
        {
            std::lock_guard<std::mutex> lock(capture_mutex);
            capture_finished = true;
        }
        capture_cv.notify_all();
    }

    // --- RE-QUEUE ---
    if (self->streamRunning_) {
        request->reuse(libcamera::Request::ReuseFlag(0));
        request->controls().set(libcamera::controls::ExposureTime, self->shutterSpeed_);
        request->controls().set(libcamera::controls::AnalogueGain, self->iso_ / 100.0f);
        
        if (videoBuffer) {
            request->addBuffer(self->previewStream_, videoBuffer);
        }

        if (self->take_picture_request_)
        {
            std::lock_guard<std::mutex> lock(buffer_queue_mutex);
            if (!free_still_buffers.empty())
            {
                libcamera::FrameBuffer *stillBuf = free_still_buffers.front();
                free_still_buffers.pop();
                
                if (request->addBuffer(self->stillStream_, stillBuf) == 0)
                {
                    LOG_DBG("[Callback] Attached Still Buffer.");
                    self->take_picture_request_ = false; 
                }
            }
        }
        self->camera_->queueRequest(request);
    }
}