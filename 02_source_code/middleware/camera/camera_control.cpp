#include "camera_control.h"
#include <libcamera/libcamera.h>
#include <fcntl.h>
#include <unistd.h>
#include <fstream>
#include <sys/mman.h>

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
#ifdef RAW_IMAGE_SAVING
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
#endif /*RAW_IMAGE_SAVING*/

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

    // Add metadata to the saved JPEG
    addMetadata("captured_image.jpg");

    return true;
}

void CameraControl::addMetadata(const std::string &filePath)
{
    // Load existing EXIF data (if any) from the file
    ExifData *exifData = exif_data_new_from_file(filePath.c_str());
    if (!exifData)
    {
        exifData = exif_data_new();
        if (!exifData)
        {
            std::cerr << "Failed to create new EXIF data" << std::endl;
            return;
        }
    }

    // Helper to create or update an EXIF entry
    auto setExifEntry = [](ExifData *ed, ExifTag tag, ExifIfd ifd, const std::string &value) {
        ExifEntry *entry = exif_content_get_entry(ed->ifd[ifd], tag);
        if (!entry)
        {
            entry = exif_entry_new();
            entry->tag = tag;
            exif_content_add_entry(ed->ifd[ifd], entry);
            exif_entry_initialize(entry, tag);
        }
        free(entry->data);
        entry->format = EXIF_FORMAT_ASCII;
        entry->components = value.length() + 1;
        entry->size = value.length() + 1;
        entry->data = (unsigned char *)malloc(entry->size);
        if (!entry->data)
        {
            std::cerr << "Failed to allocate memory for EXIF entry" << std::endl;
            return;
        }
        memcpy(entry->data, value.c_str(), value.length() + 1);
    };

    // Helper to set SHORT type entries
    auto setExifShort = [](ExifData *ed, ExifTag tag, ExifIfd ifd, unsigned short value) {
        ExifEntry *entry = exif_content_get_entry(ed->ifd[ifd], tag);
        if (!entry)
        {
            entry = exif_entry_new();
            entry->tag = tag;
            exif_content_add_entry(ed->ifd[ifd], entry);
            exif_entry_initialize(entry, tag);
        }
        free(entry->data);
        entry->format = EXIF_FORMAT_SHORT;
        entry->components = 1;
        entry->size = sizeof(unsigned short);
        entry->data = (unsigned char *)malloc(entry->size);
        if (!entry->data)
        {
            std::cerr << "Failed to allocate memory for EXIF entry" << std::endl;
            return;
        }
        exif_set_short(entry->data, exif_data_get_byte_order(ed), value);
    };

    // Helper to set RATIONAL type entries
    auto setExifRational = [](ExifData *ed, ExifTag tag, ExifIfd ifd, double value) {
        ExifEntry *entry = exif_content_get_entry(ed->ifd[ifd], tag);
        if (!entry)
        {
            entry = exif_entry_new();
            entry->tag = tag;
            exif_content_add_entry(ed->ifd[ifd], entry);
            exif_entry_initialize(entry, tag);
        }
        free(entry->data);
        entry->format = EXIF_FORMAT_RATIONAL;
        entry->components = 1;
        entry->size = sizeof(ExifRational);
        entry->data = (unsigned char *)malloc(entry->size);
        if (!entry->data)
        {
            std::cerr << "Failed to allocate memory for EXIF entry" << std::endl;
            return;
        }
        ExifRational rational;
        rational.numerator = static_cast<unsigned int>(value * 1000000); // Microseconds for precision
        rational.denominator = 1000000;
        if (rational.numerator == 0 && value != 0) {
            rational.numerator = 1; // Avoid division by zero or invalid rational
            rational.denominator = static_cast<unsigned int>(1.0 / value);
        }
        exif_set_rational(entry->data, exif_data_get_byte_order(ed), rational);
    };

    // ISO Speed Ratings
    setExifShort(exifData, EXIF_TAG_ISO_SPEED_RATINGS, EXIF_IFD_0, static_cast<unsigned short>(iso_));

    // Shutter Speed (Exposure Time, stored as a rational)
    double exposureTime = shutterSpeed_ / 1000000.0; // Convert microseconds to seconds
    if (exposureTime <= 0) exposureTime = 0.000001; // Avoid zero or negative values
    setExifRational(exifData, EXIF_TAG_EXPOSURE_TIME, EXIF_IFD_0, exposureTime);

    // Exposure Mode
    setExifShort(exifData, EXIF_TAG_EXPOSURE_MODE, EXIF_IFD_0, static_cast<unsigned short>(exposureMode_));

    // Aperture (FNumber, stored as a rational)
    float aperture = 2.8f; // Default value since not stored as member variable
    setExifRational(exifData, EXIF_TAG_FNUMBER, EXIF_IFD_0, static_cast<double>(aperture));

    // Flash (assuming flashPower as a placeholder, not stored)
    float flashPower = 0.0f; // Default value since not stored as member variable
    setExifShort(exifData, EXIF_TAG_FLASH, EXIF_IFD_0, flashPower > 0 ? 1 : 0);

    // Artist tag
    setExifEntry(exifData, EXIF_TAG_ARTIST, EXIF_IFD_0, "Rudo Camera project");

    // Software
    setExifEntry(exifData, EXIF_TAG_SOFTWARE, EXIF_IFD_0, "Raspbian GNU/Linux 11 (bullseye)");

    // Camera Model (including sensor)
    setExifEntry(exifData, EXIF_TAG_MODEL, EXIF_IFD_0, "Compute Module 4 with IMX296 GSC");

    // Copyright tag
    setExifEntry(exifData, EXIF_TAG_COPYRIGHT, EXIF_IFD_0, "© 2025 Rudo Camera project - All Rights Reserved");

    // Description tag
    setExifEntry(exifData, EXIF_TAG_IMAGE_DESCRIPTION, EXIF_IFD_0, "This is a test image");

    // DateTimeOriginal tag
    // setExifEntry(exifData, EXIF_TAG_DATE_TIME_ORIGINAL, EXIF_IFD_0, "2023:10:01 12:00:00");

    // Lens Model
    setExifEntry(exifData, EXIF_TAG_LENS_MODEL, EXIF_IFD_EXIF, "Nikon AIS 24MM F2.8 Lens");

    // Save the EXIF data to a buffer
    unsigned char *exifDataBuf = nullptr;
    unsigned int exifDataLen = 0;
    exif_data_save_data(exifData, &exifDataBuf, &exifDataLen);
    if (!exifDataBuf || exifDataLen == 0)
    {
        std::cerr << "Failed to serialize EXIF data" << std::endl;
        exif_data_unref(exifData);
        return;
    }

    // Read the original JPEG file
    std::ifstream inFile(filePath, std::ios::binary);
    if (!inFile)
    {
        std::cerr << "Failed to read JPEG file for EXIF update" << std::endl;
        free(exifDataBuf);
        exif_data_unref(exifData);
        return;
    }
    std::vector<char> jpegData((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
    inFile.close();

    if (jpegData.size() < 2 || (unsigned char)jpegData[0] != 0xFF || (unsigned char)jpegData[1] != 0xD8)
    {
        std::cerr << "Invalid JPEG file: No SOI marker found" << std::endl;
        free(exifDataBuf);
        exif_data_unref(exifData);
        return;
    }

    // Write the new JPEG file with EXIF data
    std::ofstream outFile(filePath, std::ios::binary | std::ios::trunc);
    if (!outFile)
    {
        std::cerr << "Failed to write JPEG file with EXIF data" << std::endl;
        free(exifDataBuf);
        exif_data_unref(exifData);
        return;
    }

    // Write JPEG header (SOI) and APP1 marker
    outFile.write("\xFF\xD8", 2); // JPEG Start of Image (SOI)
    outFile.write("\xFF\xE1", 2); // APP1 marker

    // Write APP1 length (big-endian, including length field itself)
    unsigned short app1Len = exifDataLen + 2; // Length includes the 2-byte length field
    unsigned char lenBytes[2];
    lenBytes[0] = (app1Len >> 8) & 0xFF; // High byte
    lenBytes[1] = app1Len & 0xFF;        // Low byte
    outFile.write((char*)lenBytes, 2);

    // Write EXIF data
    outFile.write((char*)exifDataBuf, exifDataLen);

    // Write the rest of the original JPEG data (skip the original SOI)
    outFile.write(jpegData.data() + 2, jpegData.size() - 2);

    outFile.close();
    free(exifDataBuf);
    exif_data_unref(exifData);

    std::cout << "Metadata added successfully." << std::endl;
}

void CameraControl::release()
{
    camera_->stop();
    camera_->release();
    camera_.reset();
    cameraManager_->stop();
}