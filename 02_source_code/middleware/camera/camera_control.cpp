#include "camera_control.h"
#include <libcamera/libcamera.h>
#include <fcntl.h>
#include <unistd.h>
#include <fstream>
#include <sys/mman.h>
#include <thread>
#include <chrono>

#include <opencv2/opencv.hpp>

#include <libexif/exif-data.h> 
#include <libexif/exif-utils.h> 
#include <libexif/exif-ifd.h> 
#include <libexif/exif-tag.h> 

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
        std::cerr << "Camera initialization failed in constructor" << std::endl;
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

bool CameraControl::captureImage(const std::string &image_path)
{
    if (!camera_)
    {
        std::cerr << "Camera is not initialized!" << std::endl;
        LOG_DBG("[LOG_ERROR] Camera is not initialized!");
        return false;
    }

    // Stop any existing stream (like viewfinder) before starting still capture
    camera_->stop();

    std::unique_ptr<libcamera::CameraConfiguration> config =
        camera_->generateConfiguration({ libcamera::StreamRole::StillCapture });
    
    if (!config) {
        LOG_DBG("[LOG_ERROR] Failed to generate configuration");
        return false;
    }

    libcamera::StreamConfiguration &streamConfig = config->at(0);
    streamConfig.size = {1456, 1088}; // Max resolution for IMX296
    streamConfig.pixelFormat = libcamera::formats::NV12;
    streamConfig.bufferCount = 1;
    
    if (camera_->configure(config.get()) < 0)
    {
        std::cerr << "Failed to configure camera" << std::endl;
        LOG_DBG("[LOG_ERROR] Failed to configure camera");
        return false;
    }

    libcamera::Stream *stream = streamConfig.stream();
    int width = streamConfig.size.width;
    int height = streamConfig.size.height;
    int stride = streamConfig.stride;

    libcamera::FrameBufferAllocator allocator(camera_);
    int allocated = allocator.allocate(stream);
    if (allocated < 0)
    {
        std::cerr << "Failed to allocate frame buffers" << std::endl;
        return false;
    }

    std::unique_ptr<libcamera::Request> request = camera_->createRequest();
    if (!request)
    {
        std::cerr << "Failed to create request" << std::endl;
        return false;
    }

    const auto &buffers = allocator.buffers(stream);
    const auto &buffer = buffers[0];

    if (request->addBuffer(stream, buffer.get()) < 0)
    {
        std::cerr << "Failed to add buffer to request" << std::endl;
        return false;
    }

    request->controls() = prepareControls();

    if (camera_->start() < 0)
    {
        std::cerr << "Failed to start camera" << std::endl;
        return false;
    }

    if (camera_->queueRequest(request.get()) < 0)
    {
        std::cerr << "Failed to queue request" << std::endl;
        camera_->stop();
        return false;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    // -------------------------------------------------------

    camera_->stop();

    // Process the buffer
    int fd = buffer->planes()[0].fd.get();
    size_t length = 0;
    for (const auto &plane : buffer->planes()) length += plane.length;

    void *mappedMemory = mmap(nullptr, length, PROT_READ, MAP_PRIVATE, fd, 0);
    if (mappedMemory == MAP_FAILED)
    {
        std::cerr << "Failed to map memory" << std::endl;
        return false;
    }

    // Convert to color JPEG
    openCV_JPG_Conversion(mappedMemory, width, height, stride, image_path, length);

    // Add metadata to the saved JPEG
    addMetadata(image_path);

    return true;
}

void CameraControl::addMetadata(const std::string &filePath)
{
    // Load existing EXIF data (if any) from the file
    ExifData *exifData = exif_data_new_from_file(filePath.c_str());
    if (!exifData)
    {
        exifData = exif_data_new();
        if (!exifData) return;
    }

    // Helper to set string entry
    auto setExifEntry = [](ExifData *ed, ExifTag tag, ExifIfd ifd, const std::string &value) {
        ExifEntry *entry = exif_content_get_entry(ed->ifd[ifd], tag);
        if (!entry) {
            entry = exif_entry_new();
            entry->tag = tag;
            exif_content_add_entry(ed->ifd[ifd], entry);
            exif_entry_initialize(entry, tag);
        }
        if (entry->data) free(entry->data);
        entry->format = EXIF_FORMAT_ASCII;
        entry->components = value.length() + 1;
        entry->size = value.length() + 1;
        entry->data = (unsigned char *)malloc(entry->size);
        if (entry->data) memcpy(entry->data, value.c_str(), value.length() + 1);
    };

    // Helper to set SHORT
    auto setExifShort = [](ExifData *ed, ExifTag tag, ExifIfd ifd, unsigned short value) {
        ExifEntry *entry = exif_content_get_entry(ed->ifd[ifd], tag);
        if (!entry) {
            entry = exif_entry_new();
            entry->tag = tag;
            exif_content_add_entry(ed->ifd[ifd], entry);
            exif_entry_initialize(entry, tag);
        }
        if (entry->data) free(entry->data);
        entry->format = EXIF_FORMAT_SHORT;
        entry->components = 1;
        entry->size = sizeof(unsigned short);
        entry->data = (unsigned char *)malloc(entry->size);
        if (entry->data) exif_set_short(entry->data, exif_data_get_byte_order(ed), value);
    };

    // Helper to set RATIONAL
    auto setExifRational = [](ExifData *ed, ExifTag tag, ExifIfd ifd, double value) {
        ExifEntry *entry = exif_content_get_entry(ed->ifd[ifd], tag);
        if (!entry) {
            entry = exif_entry_new();
            entry->tag = tag;
            exif_content_add_entry(ed->ifd[ifd], entry);
            exif_entry_initialize(entry, tag);
        }
        if (entry->data) free(entry->data);
        entry->format = EXIF_FORMAT_RATIONAL;
        entry->components = 1;
        entry->size = sizeof(ExifRational);
        entry->data = (unsigned char *)malloc(entry->size);
        
        ExifRational rational;
        rational.numerator = static_cast<unsigned int>(value * 1000000); 
        rational.denominator = 1000000;
        if (rational.numerator == 0 && value != 0) {
            rational.numerator = 1;
            rational.denominator = static_cast<unsigned int>(1.0 / value);
        }
        if (entry->data) exif_set_rational(entry->data, exif_data_get_byte_order(ed), rational);
    };

    setExifShort(exifData, EXIF_TAG_ISO_SPEED_RATINGS, EXIF_IFD_EXIF, static_cast<unsigned short>(iso_));
    double exposureTime = shutterSpeed_ / 1000000.0; 
    if (exposureTime <= 0) exposureTime = 0.000001;
    setExifRational(exifData, EXIF_TAG_EXPOSURE_TIME, EXIF_IFD_EXIF, exposureTime);
    setExifShort(exifData, EXIF_TAG_EXPOSURE_MODE, EXIF_IFD_EXIF, static_cast<unsigned short>(exposureMode_));
    setExifRational(exifData, EXIF_TAG_FNUMBER, EXIF_IFD_EXIF, 2.8);
    setExifEntry(exifData, EXIF_TAG_MODEL, EXIF_IFD_0, "Compute Module 4 with IMX296");
    setExifEntry(exifData, EXIF_TAG_SOFTWARE, EXIF_IFD_0, "Rudo Camera App");

    // Save and write back
    unsigned char *exifDataBuf = nullptr;
    unsigned int exifDataLen = 0;
    exif_data_save_data(exifData, &exifDataBuf, &exifDataLen);

    if (exifDataBuf && exifDataLen > 0) {
        std::ifstream inFile(filePath, std::ios::binary);
        if (inFile) {
            std::vector<char> jpegData((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
            inFile.close();

            if (jpegData.size() >= 2 && (unsigned char)jpegData[0] == 0xFF && (unsigned char)jpegData[1] == 0xD8) {
                std::ofstream outFile(filePath, std::ios::binary | std::ios::trunc);
                if (outFile) {
                    outFile.write("\xFF\xD8", 2);
                    outFile.write("\xFF\xE1", 2);
                    unsigned short app1Len = exifDataLen + 2;
                    unsigned char lenBytes[2] = {(unsigned char)((app1Len >> 8) & 0xFF), (unsigned char)(app1Len & 0xFF)};
                    outFile.write((char*)lenBytes, 2);
                    outFile.write((char*)exifDataBuf, exifDataLen);
                    outFile.write(jpegData.data() + 2, jpegData.size() - 2);
                }
            }
        }
        free(exifDataBuf);
    }
    exif_data_unref(exifData);
}

bool CameraControl::openCV_JPG_Conversion(void *mappedMemory, int width, int height, int stride, const std::string &image_path, size_t length)
{
    // NV12 has 1.5x height rows in total
    int totalRows = height + height / 2;
    cv::Mat nv12Mat(totalRows, width, CV_8UC1, mappedMemory, stride);
    cv::Mat bgrImage;
    
    // Use COLOR_YUV2BGR_NV12 for NV12 format
    cv::cvtColor(nv12Mat, bgrImage, cv::COLOR_YUV2BGR_NV12);

    std::vector<int> params;
    params.push_back(cv::IMWRITE_JPEG_QUALITY);
    params.push_back(95);
    
    bool success = cv::imwrite(image_path, bgrImage, params);
    if (success)
    {
        LOG_DBG("[LOG_INFO] Saved image: ", image_path);
    }
    else
    {
        std::cerr << "Failed to save image" << std::endl;
    }

    munmap(mappedMemory, length);
    return success;
}

void CameraControl::release()
{
    if (camera_)
    {
        camera_->stop();
        camera_->release();
        camera_.reset();
    }
    if (cameraManager_)
    {
        cameraManager_->stop();
    }
}