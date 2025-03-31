#include "camera_app.h"


CameraApp::CameraApp(ISO iso, ShutterSpeed shuttleSpeed, ExposureMode exposureMode, Aperture aperture, FlashPower flashPower)
                   :CameraControl(cameraSetting::ISOConvert(iso), cameraSetting::ShutterSpeedConvert(shuttleSpeed)
                   , cameraSetting::ExposureModeConvert(exposureMode), cameraSetting::ApertureConvert(aperture), cameraSetting::FlashPowerCOnvert(flashPower))

{
    PRINT_SETTINGS();
}



CameraApp::~CameraApp()
{
    CameraControl::~CameraControl();
}

bool CameraApp::initialize()
{
    return CameraControl::initialize();
}


// Set camera parameters
void CameraApp::setISO(int iso)  
{
    
    CameraControl::setISO(iso);
}

void CameraApp::setShutterSpeed(int speed)  
{
    CameraControl::setShutterSpeed(speed);
}

void CameraApp::setExposure(int exposure)  
{
    CameraControl::setExposure(exposure);
}


// Capture an image
bool CameraApp::captureImage(const std::string &image_path)  
{
    return CameraControl::captureImage(image_path);
}


// Release resources
void CameraApp::release()  
{
    CameraControl::release();
}


void CameraApp::setISO(ISO iso)
{
    SET_ISO(iso);
    CameraApp::setISO(VALUE_ISO());
}
void CameraApp::setShutterSpeed(ShutterSpeed shuttleSpeed)
{
    SET_SHUTTLE_SPEED(shuttleSpeed);
    CameraApp::setShutterSpeed(VALUE_SHUTTLE_SPEED());
}
void CameraApp::setExposure(ExposureMode exposureMode)
{
    SET_EXPOSURE_MODE(exposureMode);
    CameraApp::setExposure(VALUE_EXPOSURE_MODE());
}