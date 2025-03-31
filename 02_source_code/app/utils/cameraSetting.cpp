#include "cameraSetting.h"

// Constructor
cameraSetting::cameraSetting(
                            ExposureMode exposureMode,
                            Aperture aperture,
                            ShutterSpeed shutterSpeed,
                            ISO iso,
                            FlashPower flashPower)
    : exposureMode_(exposureMode), 
    aperture_(aperture), 
    shutterSpeed_(shutterSpeed), 
    iso_(iso), 
    flashPower_(flashPower)
{
}

// Update value
void cameraSetting::setAllCameraConfig(
                                    ExposureMode exposureMode, 
                                    Aperture aperture, 
                                    ShutterSpeed shutterSpeed, 
                                    ISO iso, 
                                    FlashPower flashPower)
{
    std::unique_lock lock(sharedMutex);
    exposureMode_ = exposureMode;
    aperture_ = aperture;
    shutterSpeed_ = shutterSpeed;
    iso_ = iso;
    flashPower_ = flashPower;
}

// Setters
void cameraSetting::setExposureMode(ExposureMode exposureMode)
{
    std::unique_lock lock(sharedMutex);
    exposureMode_ = exposureMode;
}

void cameraSetting::setAperture(Aperture aperture)
{
    std::unique_lock lock(sharedMutex);
    aperture_ = aperture;
}

void cameraSetting::setShutterSpeed(ShutterSpeed shutterSpeed)
{
    std::unique_lock lock(sharedMutex);
    shutterSpeed_ = shutterSpeed;
}

void cameraSetting::setISO(ISO iso)
{
    std::unique_lock lock(sharedMutex);
    iso_ = iso;
}

void cameraSetting::setFlashPower(FlashPower flashPower)
{
    std::unique_lock lock(sharedMutex);
    flashPower_ = flashPower;
}

// Getters
ExposureMode cameraSetting::getExposureMode() const
{
    std::shared_lock lock(sharedMutex);
    return exposureMode_;
}

Aperture cameraSetting::getAperture() const
{
    std::shared_lock lock(sharedMutex);
    return aperture_;
}

ShutterSpeed cameraSetting::getShutterSpeed() const
{
    std::shared_lock lock(sharedMutex);
    return shutterSpeed_;
}

ISO cameraSetting::getISO() const
{
    std::shared_lock lock(sharedMutex);
    return iso_;
}

FlashPower cameraSetting::getFlashPower() const
{
    std::shared_lock lock(sharedMutex);
    return flashPower_;
}

// get current value 
int cameraSetting::getValueExposureMode() const
{
    return static_cast<int>(exposureMode_);
}

float cameraSetting::getValueAperture() const
{
    return aperture_ / 10.0f;
}

int cameraSetting::getValueShutterSpeed() const
{
    return 1000000 / ((shutterSpeed_ == 0) ? 1 : shutterSpeed_ );
}

int cameraSetting::getValueISO() const
{
    return static_cast<int>(iso_);
}

float cameraSetting::getValueFlashPower() const
{
    return (flashPower_==0) ? flashPower_ : (1.0f / flashPower_);
}

void cameraSetting::printSettings() const
{
    switch(exposureMode_)
    {
        case ExposureMode::Disable:
        {
            LOG_DBG("Current Exposure mode:   Disable");
            break;
        }
        case ExposureMode::Auto:
        {
            LOG_DBG("Current Exposure mode:   Auto");
            break;
        } 
        case ExposureMode::Manual:
        {
            LOG_DBG("Current Exposure mode:   Manual");
            break;
        } 
        case ExposureMode::Night:
        {
            LOG_DBG("Current Exposure mode:   Night");
            break;
        }  
        case ExposureMode::Backlight:
        {
            LOG_DBG("Current Exposure mode:   Backlight");
            break;
        }                
    }

    LOG_DBG("Current Aperture:        F",(static_cast<int>(aperture_))/10.0f);

    LOG_DBG("Current Shutter Speed:   1/",shutterSpeed_);

    LOG_DBG("Current ISO:             ",iso_);

    LOG_DBG("Current Flash power:     1/",flashPower_);
}

cameraSetting settings_(Disable, F5_6, SS1_2, ISO_800, FP1_2);

