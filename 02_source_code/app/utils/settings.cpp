#include "settings.h"

// Constructor
Settings::Settings(ExposureMode exposureMode = Manual, Aperture aperture = F5_6, ShutterSpeed shutterSpeed = SS1_2, ISO iso = ISO_800, FlashPower flashPower = FP1_2)
    : exposureMode_(exposureMode), aperture_(aperture), shutterSpeed_(shutterSpeed), iso_(iso), flashPower_(flashPower)
{
}

// Update value
void Settings::setValue(ExposureMode exposureMode = Manual, Aperture aperture = F5_6, ShutterSpeed shutterSpeed = SS1_2, ISO iso = ISO_800, FlashPower flashPower = FP1_2)
{
    std::unique_lock lock(sharedMutex);
    exposureMode_ = exposureMode;
    aperture_ = aperture;
    shutterSpeed_ = shutterSpeed;
    iso_ = iso;
    flashPower_ = flashPower;
}

// Setters
void Settings::setExposureMode(ExposureMode exposureMode)
{
    std::unique_lock lock(sharedMutex);
    exposureMode_ = exposureMode;
}

void Settings::setAperture(Aperture aperture)
{
    std::unique_lock lock(sharedMutex);
    aperture_ = aperture;
}

void Settings::setShutterSpeed(ShutterSpeed shutterSpeed)
{
    std::unique_lock lock(sharedMutex);
    shutterSpeed_ = shutterSpeed;
}

void Settings::setISO(ISO iso)
{
    std::unique_lock lock(sharedMutex);
    iso_ = iso;
}

void Settings::setFlashPower(FlashPower flashPower)
{
    std::unique_lock lock(sharedMutex);
    flashPower_ = flashPower;
}

// Getters
ExposureMode Settings::getExposureMode() const
{
    std::shared_lock lock(sharedMutex);
    return exposureMode_;
}

Aperture Settings::getAperture() const
{
    std::shared_lock lock(sharedMutex);
    return aperture_;
}

ShutterSpeed Settings::getShutterSpeed() const
{
    std::shared_lock lock(sharedMutex);
    return shutterSpeed_;
}

ISO Settings::getISO() const
{
    std::shared_lock lock(sharedMutex);
    return iso_;
}

FlashPower Settings::getFlashPower() const
{
    std::shared_lock lock(sharedMutex);
    return flashPower_;
}

// get current value 
int Settings::getValueExposureMode() const
{
    return static_cast<int>(exposureMode_);
}

float Settings::getValueAperture() const
{
    return aperture_ / 10.0f;
}

int Settings::getValueShutterSpeed() const
{
    return 1000000 / shutterSpeed_;
}

int Settings::getValueISO() const
{
    return static_cast<int>(iso_);
}

float Settings::getValueFlashPower() const
{
    return 1.0f / flashPower_;
}

void Settings::printSettings() const
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

Settings settings_(Manual, F5_6, SS1_2, ISO_800, FP1_2);

