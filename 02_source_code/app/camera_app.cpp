#include "camera_app.h"
#include "cameraSetting.h" 
#include <chrono>
#include <sys/mman.h> 
#include <unistd.h>
#include <libcamera/formats.h>
#include <libcamera/framebuffer_allocator.h>

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
    
    // FIX 1: Release the camera immediately after Base Class Init.
    // CameraControl::initialize() acquires the camera. We must release it here
    // so that startVideoStream() can successfully acquire it later.
    if (camera_) {
        camera_->release();
    }
}

CameraApp::~CameraApp() {
    stopVideoStream();
    release();
    if (instance_ == this) instance_ = nullptr;
}

CameraApp* CameraApp::getInstance() { return instance_; }
bool CameraApp::initialize() { return CameraControl::initialize(); }
void CameraApp::setISO(int iso) { CameraControl::setISO(iso); }
void CameraApp::setShutterSpeed(int shutterSpeed) { CameraControl::setShutterSpeed(shutterSpeed); }
void CameraApp::setExposure(int exposureMode) { CameraControl::setExposure(exposureMode); }
void CameraApp::release() { CameraControl::release(); }

// --- FIXED CAPTURE WRAPPER ---
bool CameraApp::captureImage(const std::string &image_path) { 
    if (!camera_) return false;
    
    // Own the camera
    camera_->acquire();
    bool ret = CameraControl::captureImage(image_path);
    // Release ownership
    camera_->release();
    
    return ret;
}

// =================================================================================
// --- CORE STREAMING LOGIC ---
// =================================================================================

static std::unique_ptr<libcamera::FrameBufferAllocator> g_allocator;

bool CameraApp::startVideoStream(int width, int height) {
    if (!camera_) {
        LOG_ERR("[Stream] Camera not initialized.");
        return false;
    }
    if (streamRunning_) return true;

    // FIX 1 (Continued): Now this acquire will succeed because we released in constructor.
    if (camera_->acquire() < 0) {
        LOG_ERR("[Stream] Failed to acquire camera. (Busy?)");
        return false;
    }

    // 1. Generate Config
    previewConfig_ = camera_->generateConfiguration({ libcamera::StreamRole::Viewfinder });
    if (!previewConfig_) {
        LOG_ERR("[Stream] Failed config gen.");
        camera_->release();
        return false;
    }

    // 2. Setup Parameters (NV12 + 640x480)
    libcamera::StreamConfiguration &streamConfig = previewConfig_->at(0);
    streamConfig.size = { 640, 480 }; 
    streamConfig.pixelFormat = libcamera::formats::NV12; 
    streamConfig.bufferCount = 4; 

    // 3. Configure
    previewConfig_->validate(); 
    if (camera_->configure(previewConfig_.get()) < 0) {
        LOG_ERR("[Stream] Failed configure.");
        camera_->release();
        return false;
    }
    previewStream_ = streamConfig.stream();

    // 4. Allocate
    g_allocator = std::make_unique<libcamera::FrameBufferAllocator>(camera_);
    if (g_allocator->allocate(previewStream_) < 0) {
        LOG_ERR("[Stream] Failed allocate.");
        camera_->release();
        return false;
    }

    // 5. Connect Callback
    // camera_->requestCompleted.disconnect(); 
    camera_->requestCompleted.connect(this, [](libcamera::Request *req) {
        CameraApp::requestComplete(req);
    });

    // 6. Start
    if (camera_->start() < 0) {
        LOG_ERR("[Stream] Failed start.");
        camera_->release();
        return false;
    }

    // 7. Queue Requests
    const auto &buffers = g_allocator->buffers(previewStream_);
    for (const auto &buffer : buffers) {
        std::unique_ptr<libcamera::Request> request = camera_->createRequest();
        if (!request || request->addBuffer(previewStream_, buffer.get()) < 0) {
            LOG_ERR("[Stream] Failed queue setup.");
            return false;
        }
        camera_->queueRequest(request.release());
    }

    streamRunning_ = true;
    LOG_STT("[Stream] Started.");
    return true;
}

void CameraApp::stopVideoStream() {
    if (!streamRunning_) return;
    
    streamRunning_ = false;

    if (camera_) {
        camera_->stop(); 
        // Fully Release so Capture can take over
        camera_->release();
    }

    videoCv_.notify_all(); 
    
    if (g_allocator) {
        g_allocator.reset();
    }
    
    previewStream_ = nullptr;
    previewConfig_.reset();

    LOG_STT("[Stream] Stopped & Released.");
}

bool CameraApp::getLatestPreviewFrame(cv::Mat &outputFrame) {
    std::unique_lock<std::mutex> lock(videoMutex_);
    videoCv_.wait(lock, [this]{ return !currentPreviewFrame_.empty() || !streamRunning_; });
    if (!streamRunning_) return false;
    currentPreviewFrame_.copyTo(outputFrame);
    return true;
}

void CameraApp::requestComplete(libcamera::Request *request) {
    if (request->status() == libcamera::Request::RequestCancelled) return;

    CameraApp *self = CameraApp::getInstance();
    if (!self || !self->streamRunning_) return;

    const libcamera::FrameBuffer *buffer = request->buffers().at(self->previewStream_);
    
    // FIX 2: Calculate TOTAL length of all planes to prevent memory corruption
    // NV12 might be exposed as multiple planes or one large buffer. 
    // Mapping full size ensures safety.
    int fd = buffer->planes()[0].fd.get();
    size_t totalLength = 0;
    for (const auto &plane : buffer->planes()) {
        totalLength += plane.length;
    }

    void *map = mmap(NULL, totalLength, PROT_READ, MAP_SHARED, fd, 0);
    
    if (map != MAP_FAILED) {
        const libcamera::StreamConfiguration &config = self->previewStream_->configuration();
        int w = config.size.width;
        int h = config.size.height;
        int stride = config.stride;

        // Process NV12
        cv::Mat nv12Mat(h + h / 2, w, CV_8UC1, map, stride);
        cv::Mat bgrMat;
        cv::cvtColor(nv12Mat, bgrMat, cv::COLOR_YUV2BGR_NV12);

        // Resize 640x480 -> 480x320
        cv::Mat resizedMat;
        cv::resize(bgrMat, resizedMat, cv::Size(480, 320)); 

        {
            std::lock_guard<std::mutex> lock(self->videoMutex_);
            resizedMat.copyTo(self->currentPreviewFrame_);
        }
        self->videoCv_.notify_one(); 
        munmap(map, totalLength); // Unmap total length
    }
    
    if (self->streamRunning_) {
        request->reuse(libcamera::Request::ReuseBuffers);
        self->camera_->queueRequest(request);
    }
}