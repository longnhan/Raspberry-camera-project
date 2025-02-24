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
#include <iostream>

CameraControl::CameraControl()
    : iso_(100), shutterSpeed_(10000), exposureMode_(0)
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

    // Print detected cameras
    LOG_DBG("[LOG_DEBUG] Detected cameras:");
    for (const auto &camera : cameraManager_->cameras())
    {
        LOG_DBG("[LOG_DEBUG]  - ",camera->id());
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

// Helper: Prepare a control list based on the current settings.
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
        return false;
    }

    // Generate a configuration for still capture.
    std::unique_ptr<libcamera::CameraConfiguration> config =
        camera_->generateConfiguration({ libcamera::StreamRole::StillCapture });
    if (!config || config->size() == 0) {
        std::cerr << "Failed to generate camera configuration" << std::endl;
        return false;
    }
    if (camera_->configure(config.get()) < 0) {
        std::cerr << "Failed to configure camera" << std::endl;
        return false;
    }

    // Use the first stream in the configuration.
    libcamera::Stream *stream = config->at(0).stream();

    // Create a frame buffer allocator for the camera.
    libcamera::FrameBufferAllocator allocator(camera_);
    if (allocator.allocate(stream) < 0) {
        std::cerr << "Failed to allocate frame buffers" << std::endl;
        return false;
    }

    // Create a request.
    std::unique_ptr<libcamera::Request> request = camera_->createRequest();
    if (!request) {
        std::cerr << "Failed to create request" << std::endl;
        return false;
    }

    // Get buffers allocated for the stream.
    const auto &buffers = allocator.buffers(stream);
    if (buffers.empty()) {
        std::cerr << "No buffers allocated for stream" << std::endl;
        return false;
    }
    // Attach the first buffer for capture.
    if (request->addBuffer(stream, buffers[0].get()) < 0) {
        std::cerr << "Failed to add buffer to request" << std::endl;
        return false;
    }

    // Prepare and assign controls to the request.
    request->controls() = prepareControls();

    // **Start the camera first** so that it's in the Running state.
    if (camera_->start() < 0) {
        std::cerr << "Failed to start camera" << std::endl;
        return false;
    }

    // Now queue the request.
    if (camera_->queueRequest(request.get()) < 0) {
        std::cerr << "Failed to queue request" << std::endl;
        camera_->stop();
        return false;
    }

    // Wait for the request to complete.
    // (In a real application, you’d normally use an event loop or callback)
    sleep(1);  // Wait 1 second to allow the capture to complete.

    // Stop the camera.
    camera_->stop();

    // Save image data to a file.
    std::ofstream outFile("captured_image.raw", std::ios::binary);
    if (!outFile) {
        std::cerr << "Failed to open output file" << std::endl;
        return false;
    }

    // Iterate over each buffer.
    for (const auto &buffer : buffers) {
        // Obtain the file descriptor.
        int fd = buffer->planes()[0].fd.get();
        size_t length = buffer->planes()[0].length;

        // Map the buffer memory to user space.
        void *mappedMemory = mmap(nullptr, length, PROT_READ, MAP_PRIVATE, fd, 0);
        if (mappedMemory == MAP_FAILED) {
            std::cerr << "Failed to map memory" << std::endl;
            return false;
        }

        outFile.write(static_cast<const char *>(mappedMemory), length);
        munmap(mappedMemory, length);
    }

    outFile.close();
    LOG_DBG("[LOG_DEBUG] Image captured and saved as 'captured_image.raw'");
    return true;
}

void CameraControl::release()
{
    camera_->stop();
    camera_->release();
    camera_.reset();
    cameraManager_->stop();
}
