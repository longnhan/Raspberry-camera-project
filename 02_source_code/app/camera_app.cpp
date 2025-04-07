#include "camera_app.h"


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
}

CameraApp::~CameraApp()
{
    release();
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
