#include "camera_app.h"
#include "cameraSetting.h" 
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

// --- STATIC MEMBER DEFINITION ---
CameraApp* CameraApp::instance_ = nullptr; 

namespace {
    int LocalISOConvert(int iso) { return iso; }
    int LocalShutterSpeedConvert(int speed) { return speed; }
    int LocalExposureModeConvert(int mode) { return mode; }
    float LocalApertureConvert(float aperture) { return aperture; }
    float LocalFlashPowerCOnvert(float power) { return power; }
}

CameraApp::CameraApp(ISO iso, ShutterSpeed shuttleSpeed, ExposureMode exposureMode, Aperture aperture, FlashPower flashPower)
    : CameraControl(LocalISOConvert(iso), LocalShutterSpeedConvert(shuttleSpeed), LocalExposureModeConvert(exposureMode), LocalApertureConvert(aperture), LocalFlashPowerCOnvert(flashPower))
{
    instance_ = this;
    // Ensure clean state on boot, but we will HOLD the lock afterwards
    if (camera_) {
         camera_->release();
    }
}

CameraApp::~CameraApp() {
    stopVideoStream();
    // Only release when the app is actually closing
    if (camera_) { 
        camera_->release(); 
    }
    if (instance_ == this) instance_ = nullptr;
}

CameraApp* CameraApp::getInstance() { return instance_; }
bool CameraApp::initialize() { return CameraControl::initialize(); }
void CameraApp::setISO(int iso) { CameraControl::setISO(iso); }
void CameraApp::setShutterSpeed(int shutterSpeed) { CameraControl::setShutterSpeed(shutterSpeed); }
void CameraApp::setExposure(int exposureMode) { CameraControl::setExposure(exposureMode); }

// Modified Release to prevent accidental hardware power-down
void CameraApp::release() { 
    if (camera_) camera_->release(); 
}

// --- CAPTURE WRAPPER ---
bool CameraApp::captureImage(const std::string &image_path) { 
    if (!camera_) return false;
    
    // We assume we ALREADY hold the lock from startVideoStream.
    // We do NOT call acquire() or release() here to prevent hardware teardown.
    LOG_DBG("[App] Capture requested (Holding existing lock).");
    
    bool ret = CameraControl::captureImage(image_path);
    
    LOG_DBG("[App] Capture completed.");
    return ret;
}

// =================================================================================
// --- CORE STREAMING LOGIC ---
// =================================================================================

static std::unique_ptr<libcamera::FrameBufferAllocator> g_allocator;
static std::atomic<int> request_count_for_stop_wait;
static std::condition_variable cv_request_wait;
static std::mutex mutex_request_wait;
static bool is_camera_acquired = false; 

bool CameraApp::startVideoStream(int width, int height) {
    LOG_DBG("[Stream] startVideoStream called.");

    if (!camera_ || streamRunning_) return streamRunning_ ? true : false;

    // FIX 1: Persistent Acquisition Logic
    if (!is_camera_acquired) {
        int ret = camera_->acquire();
        if (ret < 0) {
             LOG_DBG("[Stream] Camera already acquired. Proceeding...");
        } else {
             LOG_DBG("[Stream] Camera acquired.");
        }
        is_camera_acquired = true;
    }

    // 1. Generate Config
    previewConfig_ = camera_->generateConfiguration({ libcamera::StreamRole::Viewfinder });
    if (!previewConfig_) { 
        LOG_ERR("[Stream] Failed to generate config");
        return false; 
    }

    // 2. Setup Parameters
    libcamera::StreamConfiguration &streamConfig = previewConfig_->at(0);
    streamConfig.size = { 640, 480 }; 
    streamConfig.pixelFormat = libcamera::formats::NV12; 
    streamConfig.bufferCount = 4; // Back to 4 for stability

    // 3. Configure
    previewConfig_->validate(); 
    if (camera_->configure(previewConfig_.get()) < 0) { 
        LOG_ERR("[Stream] Failed to configure");
        return false; 
    }
    previewStream_ = streamConfig.stream();

    // 4. Allocate
    g_allocator = std::make_unique<libcamera::FrameBufferAllocator>(camera_);
    if (g_allocator->allocate(previewStream_) < 0) { 
        LOG_ERR("[Stream] Failed to allocate");
        return false; 
    }
    LOG_DBG("[Stream] Allocated buffers.");

    // 5. Memory Priming (Safety net against black screen)
    const auto &allocated_buffers = g_allocator->buffers(previewStream_);
    for (const auto &buffer : allocated_buffers) {
        int fd = buffer->planes()[0].fd.get();
        size_t length = buffer->planes()[0].length;
        void *map = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (map != MAP_FAILED) {
            memset(map, 0, length); 
            munmap(map, length);
        }
    }

    // 6. Create Fresh Requests
    for (auto *req : requests_to_recycle_) { delete req; }
    requests_to_recycle_.clear();

    const auto &buffers = g_allocator->buffers(previewStream_);
    if (buffers.empty()) {
         LOG_ERR("[Stream] No buffers found!");
         return false;
    }

    for (const auto &buffer : buffers) {
        libcamera::Request *request = camera_->createRequest().release(); 
        if (!request || request->addBuffer(previewStream_, buffer.get()) < 0) {
            LOG_ERR("[Stream] Failed request setup.");
            return false;
        }
        requests_to_recycle_.push_back(request);
    }
    LOG_DBG("[Stream] Generated ", requests_to_recycle_.size(), " requests.");

    // 7. Connect Callback
    camera_->requestCompleted.disconnect(this);
    camera_->requestCompleted.disconnect(CameraApp::requestComplete); 
    camera_->requestCompleted.connect(CameraApp::requestComplete);

    // 8. Start Camera
    if (camera_->start() < 0) { 
        LOG_ERR("[Stream] Failed to start camera.");
        return false; 
    }
    LOG_DBG("[Stream] Camera started.");

    // 9. Slow-Start Queueing (Staggered)
    // Drip-feed requests to prevent ISP pipeline stall
    streamRunning_ = true;
    request_count_for_stop_wait = requests_to_recycle_.size(); 
    
    if (requests_to_recycle_.size() >= 1) {
        LOG_DBG("[Stream] Queueing Request 1/4");
        camera_->queueRequest(requests_to_recycle_[0]);
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Delay
    }
    
    if (requests_to_recycle_.size() >= 2) {
        LOG_DBG("[Stream] Queueing Request 2/4");
        camera_->queueRequest(requests_to_recycle_[1]);
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Delay
    }

    LOG_DBG("[Stream] Queueing remaining requests.");
    for (size_t i = 2; i < requests_to_recycle_.size(); ++i) {
        camera_->queueRequest(requests_to_recycle_[i]);
    }
    
    return true;
}

void CameraApp::stopVideoStream() {
    LOG_DBG("[Stream] stopVideoStream called.");
    if (!streamRunning_) return;
    
    streamRunning_ = false;

    if (camera_) {
        LOG_DBG("[Stream] Stopping camera...");
        camera_->stop(); 
        
        // Wait for requests to drain
        std::unique_lock<std::mutex> lock(mutex_request_wait);
        if (cv_request_wait.wait_for(lock, std::chrono::seconds(2), 
                                     [] { return request_count_for_stop_wait == 0; })) 
        {
            LOG_DBG("[Stream] All requests returned.");
        } else {
            LOG_ERR("[Stream] Timeout waiting for requests.");
        }
        
        // Cleanup Requests
        for (auto *req : requests_to_recycle_) {
            delete req;
        }
        requests_to_recycle_.clear();

        camera_->requestCompleted.disconnect(CameraApp::requestComplete);
        
        // FIX 3: RETAIN LOCK
        // We do NOT release the camera here. This keeps the hardware awake.
        LOG_DBG("[Stream] Camera stopped (Lock retained)."); 
    }

    videoCv_.notify_all(); 
    
    if (g_allocator) {
        LOG_DBG("[Stream] Freeing allocator.");
        g_allocator.reset();
    }
    
    previewStream_ = nullptr;
    previewConfig_.reset();
}

bool CameraApp::getLatestPreviewFrame(cv::Mat &outputFrame) {
    std::unique_lock<std::mutex> lock(videoMutex_);
    
    if (!videoCv_.wait_for(lock, std::chrono::milliseconds(200), 
                           [this]{ return !currentPreviewFrame_.empty() || !streamRunning_; })) 
    {
        return false; 
    }

    if (!streamRunning_) return false;
    currentPreviewFrame_.copyTo(outputFrame);
    return true;
}

void CameraApp::requestComplete(libcamera::Request *request) {
    CameraApp *self = CameraApp::getInstance();
    
    if (!self || !self->streamRunning_ || request->status() == libcamera::Request::RequestCancelled) {
        if (self && --request_count_for_stop_wait == 0) {
            cv_request_wait.notify_all();
        }
        return; 
    }

    // Debug log to confirm frame arrival (Only for debugging first frame issues)
    // static int frame_debug_counter = 0;
    // if (frame_debug_counter < 5) { LOG_DBG("[Callback] Frame Received!"); frame_debug_counter++; }

    const libcamera::FrameBuffer *buffer = request->buffers().at(self->previewStream_);
    
    int fd = buffer->planes()[0].fd.get();
    size_t totalLength = 0;
    for (const auto &plane : buffer->planes()) totalLength += plane.length;

    void *map = mmap(NULL, totalLength, PROT_READ, MAP_SHARED, fd, 0);
    
    if (map != MAP_FAILED) {
        const libcamera::StreamConfiguration &config = self->previewStream_->configuration();
        int w = config.size.width;
        int h = config.size.height;
        int stride = config.stride;

        cv::Mat nv12Mat(h + h / 2, w, CV_8UC1, map, stride);
        cv::Mat bgrMat;
        cv::cvtColor(nv12Mat, bgrMat, cv::COLOR_YUV2BGR_NV12);
        
        cv::Mat resizedMat;
        cv::resize(bgrMat, resizedMat, cv::Size(480, 320)); 

        {
            std::lock_guard<std::mutex> lock(self->videoMutex_);
            self->currentPreviewFrame_ = resizedMat; 
        }
        self->videoCv_.notify_one(); 
        munmap(map, totalLength);
    }
    
    if (self->streamRunning_) {
        request->reuse(libcamera::Request::ReuseFlag::ReuseBuffers);
        self->camera_->queueRequest(request);
    } else {
        if (--request_count_for_stop_wait == 0) {
            cv_request_wait.notify_all();
        }
    }
}