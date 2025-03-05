#include "camera_control.h"
#include <libcamera/framebuffer_allocator.h>
#include <libcamera/request.h>
#include <libcamera/stream.h>
#include <libcamera/formats.h>
#include <libcamera/control_ids.h>
#include <libcamera/property_ids.h>
#include <fcntl.h>
#include <unistd.h>
#include <fstream>
#include <sys/mman.h>
#include <opencv2/opencv.hpp>

CameraControl::CameraControl(
	int iso, 
	int shutterSpeed, 
	int exposeureMode, 
	float aperture,
	float flashPower)
    : iso_(iso), 
      shutterSpeed_(shutterSpeed), 
      exposureMode_(exposeureMode)
{
    if (!initialize())
    {
        throw std::runtime_error("Camera initialization failed");
    }
}

CameraControl::~CameraControl() {
    release();
}

bool CameraControl::initialize()
{
    cameraManager_ = std::make_unique<libcamera::CameraManager>();
    if (cameraManager_->start())
    {
        std::cerr << "Failed to start camera manager" << std::endl;
        return false;
    }

    if (cameraManager_->cameras().empty())
    {
        std::cerr << "No cameras found" << std::endl;
        return false;
    }

    LOG_DBG("[LOG_DEBUG] Detected cameras:");
    for (const auto &camera : cameraManager_->cameras())
    {
        LOG_DBG("[LOG_DEBUG]  - ", camera->id());
    }

    camera_ = cameraManager_->cameras().front();
    if (!camera_)
    {
        std::cerr << "Failed to get camera" << std::endl;
        return false;
    }

    LOG_DBG("[LOG_DEBUG] Using camera: ", camera_->id());

    if (camera_->acquire())
    {
        std::cerr << "Failed to acquire camera" << std::endl;
        return false;
    }

    return true;
}

void CameraControl::setISO(int iso) {
    iso_ = iso;
}

void CameraControl::setShutterSpeed(int speed) {
    shutterSpeed_ = speed;
}

void CameraControl::setExposure(int exposure) {
    exposureMode_ = exposure;
}

libcamera::ControlList CameraControl::prepareControls() {
    libcamera::ControlList controls(camera_->controls());
    controls.set(libcamera::controls::AnalogueGain, iso_ / 100.0f);
    controls.set(libcamera::controls::ExposureTime, shutterSpeed_);
    controls.set(libcamera::controls::AeEnable, exposureMode_ == 0);
    return controls;
}

bool CameraControl::captureImage() {
    if (!camera_) {
        std::cerr << "Camera is not initialized!" << std::endl;
        LOG_DBG("[LOG_ERROR] Camera is not initialized!");
        return false;
    }

    std::unique_ptr<libcamera::CameraConfiguration> config =
        camera_->generateConfiguration({ libcamera::StreamRole::StillCapture });
    libcamera::StreamConfiguration &streamConfig = config->at(0);
    streamConfig.size = {1456, 1088};
    streamConfig.pixelFormat = libcamera::formats::NV12;
    streamConfig.bufferCount = 1;
    if (camera_->configure(config.get()) < 0) {
        std::cerr << "Failed to configure camera" << std::endl;
        LOG_DBG("[LOG_ERROR] Failed to configure camera");
        return false;
    }

    libcamera::Stream *stream = streamConfig.stream();
    int width = streamConfig.size.width;
    int height = streamConfig.size.height;
    std::string format = streamConfig.pixelFormat.toString();
    int stride = streamConfig.stride;
    LOG_DBG("[LOG_INFO] Stream configured - Format: ", format, " Width: ", width, " Height: ", height, " Stride: ", stride);

    libcamera::FrameBufferAllocator allocator(camera_);
    int allocated = allocator.allocate(stream);
    if (allocated < 0) {
        std::cerr << "Failed to allocate frame buffers" << std::endl;
        LOG_DBG("[LOG_ERROR] Failed to allocate frame buffers");
        return false;
    }
    LOG_DBG("[LOG_INFO] Frame buffers allocated successfully, count: ", allocated);

    std::unique_ptr<libcamera::Request> request = camera_->createRequest();
    if (!request) {
        std::cerr << "Failed to create request" << std::endl;
        LOG_DBG("[LOG_ERROR] Failed to create request");
        return false;
    }
    LOG_DBG("[LOG_INFO] Capture request created");

    const auto &buffers = allocator.buffers(stream);
    if (buffers.empty()) {
        std::cerr << "No buffers allocated for stream" << std::endl;
        LOG_DBG("[LOG_ERROR] No buffers allocated for stream");
        return false;
    }

    const auto &buffer = buffers[0];
    size_t yPlaneSize = height * stride;
    size_t uvPlaneSize = (height / 2) * stride;
    size_t totalSize = yPlaneSize + uvPlaneSize;

    size_t bufferTotalSize = 0;
    for (const auto &plane : buffer->planes()) {
        bufferTotalSize += plane.length;
    }
    LOG_DBG("[LOG_INFO] Buffer size before capture - Expected for NV12: ", totalSize, " Allocated: ", bufferTotalSize);
    if (bufferTotalSize < totalSize) {
        std::cerr << "Buffer too small for NV12 format! Expected: " << totalSize << ", Got: " << bufferTotalSize << std::endl;
        LOG_DBG("[LOG_ERROR] Buffer too small for NV12 format! Expected: ", totalSize, " Got: ", bufferTotalSize);
        return false;
    }

    if (request->addBuffer(stream, buffer.get()) < 0) {
        std::cerr << "Failed to add buffer to request" << std::endl;
        LOG_DBG("[LOG_ERROR] Failed to add buffer to request");
        return false;
    }
    LOG_DBG("[LOG_INFO] Buffer added to request");

    request->controls() = prepareControls();
    LOG_DBG("[LOG_INFO] Controls set - ISO: ", iso_, " ShutterSpeed: ", shutterSpeed_, " ExposureMode: ", exposureMode_);

    if (camera_->start() < 0) {
        std::cerr << "Failed to start camera" << std::endl;
        LOG_DBG("[LOG_ERROR] Failed to start camera");
        return false;
    }
    LOG_DBG("[LOG_INFO] Camera started");

    if (camera_->queueRequest(request.get()) < 0) {
        std::cerr << "Failed to queue request" << std::endl;
        LOG_DBG("[LOG_ERROR] Failed to queue request");
        camera_->stop();
        return false;
    }
    LOG_DBG("[LOG_INFO] Request queued");

    sleep(3); // Wait 5s for 1/2s exposure
    LOG_DBG("[LOG_INFO] Waiting 5 seconds for capture to complete");

    camera_->stop();
    LOG_DBG("[LOG_INFO] Camera stopped");

    int fd = buffer->planes()[0].fd.get();
    size_t length = bufferTotalSize; // Use the total size of all planes
    LOG_DBG("[LOG_INFO] Buffer details - FD: ", fd, " Length: ", length);

    void *mappedMemory = mmap(nullptr, length, PROT_READ, MAP_PRIVATE, fd, 0);
    if (mappedMemory == MAP_FAILED) {
        std::cerr << "Failed to map memory" << std::endl;
        LOG_DBG("[LOG_ERROR] Failed to map memory");
        return false;
    }
    LOG_DBG("[LOG_INFO] Memory mapped successfully");

    // Save raw NV12 buffer for debugging (optional)
    std::ofstream outFile("captured_image.raw", std::ios::binary);
    if (outFile) {
        outFile.write(static_cast<const char*>(mappedMemory), length);
        outFile.close();
        LOG_DBG("[LOG_INFO] Raw NV12 buffer saved as 'captured_image.raw'");
    } else {
        LOG_DBG("[LOG_ERROR] Failed to save raw NV12 buffer");
        munmap(mappedMemory, length);
        return false;
    }

    // Check raw values for Y-plane (optional debugging)
    const uint8_t *rawData = static_cast<const uint8_t*>(mappedMemory);
    uint8_t minVal = rawData[0];
    uint8_t maxVal = rawData[0];
    for (size_t i = 1; i < yPlaneSize && i < length; ++i) {
        if (rawData[i] < minVal) minVal = rawData[i];
        if (rawData[i] > maxVal) maxVal = rawData[i];
    }
    LOG_DBG("[LOG_INFO] Raw NV12 Y-plane - Min value: ", static_cast<int>(minVal), " Max value: ", static_cast<int>(maxVal));

    // Convert to color JPEG
    int totalRows = height + height / 2;
    cv::Mat nv12Mat(totalRows, width, CV_8UC1, mappedMemory, stride);
    cv::Mat bgrImage;
    cv::cvtColor(nv12Mat, bgrImage, cv::COLOR_YUV2BGR_NV12);

    std::vector<int> params;
    params.push_back(cv::IMWRITE_JPEG_QUALITY);
    params.push_back(95);
    if (!cv::imwrite("captured_image.jpg", bgrImage, params)) {
        std::cerr << "Failed to save JPEG image" << std::endl;
        LOG_DBG("[LOG_ERROR] Failed to save JPEG image to 'captured_image.jpg'");
        munmap(mappedMemory, length);
        return false;
    }
    LOG_DBG("[LOG_INFO] Color JPEG saved as 'captured_image.jpg'");

    munmap(mappedMemory, length);
    LOG_DBG("[LOG_INFO] Memory unmapped");

    return true;
}

void CameraControl::release()
{
    camera_->stop();
    camera_->release();
    camera_.reset();
    cameraManager_->stop();
}