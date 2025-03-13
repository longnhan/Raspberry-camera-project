#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include <iostream>
#include <shared_mutex>
#include <thread>
#include "log.h"
class Settings;
// share settings
extern Settings settings_;

// macro to setvalue
#define  SET_ALL_CAMERA_CONFIG(a,b,c,d,e) (settings_.setAllCameraConfig(a,b,c,d,e))

#define  SET_EXPOSURE_MODE(a)    (settings_.setExposureMode(a))
#define  SET_APERTURE(a)         (settings_.setAperture(a))
#define  SET_SHUTTLE_SPEED(a)    (settings_.setShutterSpeed(a))
#define  SET_ISO(a)              (settings_.setISO(a))
#define  SET_FLASH_POWER(a)      (settings_.setFlashPower(a))

// macro to get enum value
#define  EXPOSURE_MODE()         (settings_.getExposureMode())
#define  APERTURE()              (settings_.getAperture())
#define  SHUTTLE_SPEED()         (settings_.getShutterSpeed())
#define  ISO()                   (settings_.getISO())
#define  FLASH_POWER()           (settings_.getFlashPower())

// macro to get value
#define  VALUE_EXPOSURE_MODE()   (settings_.getValueExposureMode())
#define  VALUE_APERTURE()        (settings_.getValueAperture())
#define  VALUE_SHUTTLE_SPEED()   (settings_.getValueShutterSpeed())
#define  VALUE_ISO()             (settings_.getValueISO())
#define  VALUE_FLASH_POWER()     (settings_.getValueFlashPower())
#define  PRINT_SETTINGS()        (settings_.printSettings())
// define user mode
enum ExposureMode
{
    Disable   = 0,
    Auto      = 1,
    Manual    = 2,
    Night     = 3,
    Backlight = 4
};

// define Aperture
enum Aperture
{
    F1_4 = 14, // value x10
    F2_0 = 20,
    F2_8 = 28,
    F4_0 = 40,
    F5_6 = 56,
    F8_0 = 80,
    F11  = 110, 
    F16  = 160
};

// Shutter Speed
enum ShutterSpeed
{
    SS1   = 1,       // 1/1
    SS1_2 = 2,       // 1/2
    SS1_4 = 4,       // 1/4
    SS1_8 = 8,       // 1/8
    SS1_15 = 15,     // 1/15
    SS1_30 = 30,     // 1/30
    SS1_60 = 60,     // 1/60
    SS1_125 = 125,   // 1/125
    SS1_250 = 250,   // 1/250
    SS1_500 = 500,   // 1/500 
    SS1_1000 = 1000, // 1/1000
    SS1_2000 = 2000  // 1/2000
};

// define value ISO
enum ISO
{
    ISO_12800 = 12800,
    ISO_6400  = 6400,
    ISO_3200  = 3200,
    ISO_1600  = 1600,
    ISO_800   = 800,
    ISO_400   = 400,
    ISO_200   = 200,
    ISO_100   = 100,
    ISO_50    = 50
};

// Define flash power
enum FlashPower
{
    FP1_1  = 1,   // 1/1
    FP1_2  = 2,   // 1/2
    FP1_4  = 4,   // 1/4
    FP1_8  = 8,   // 1/8
    FP1_16 = 16,  // 1/16
    FP1_32 = 32,  // 1/32
    FP1_64 = 64   // 1/64
};

class Settings
{
public:
    // inline make funtion static avoid error redefination
    inline static int ExposureModeConvert(ExposureMode exposureMode) {return static_cast<int>(exposureMode); }
    inline static float ApertureConvert(Aperture aperture) {return aperture / 10.0f;}
    inline static int ShutterSpeedConvert(ShutterSpeed shutterSpeed) {return 1000000 / ((shutterSpeed == 0) ? 1 : shutterSpeed ); }
    inline static int ISOConvert(ISO iso) {return static_cast<int>(iso);}
    inline static float FlashPowerCOnvert(FlashPower flashPower) {return (flashPower==0) ? flashPower : (1.0f / flashPower);}

    Settings(ExposureMode exposureMode, Aperture aperture, ShutterSpeed shutterSpeed, ISO iso, FlashPower flashPower);
    ~Settings() = default;

    // Setters
    void setAllCameraConfig(ExposureMode exposureMode, Aperture aperture, ShutterSpeed shutterSpeed, ISO iso, FlashPower flashPower);
    void setExposureMode(ExposureMode exposureMode);
    void setAperture(Aperture aperture);
    void setShutterSpeed(ShutterSpeed shutterSpeed);
    void setISO(ISO iso);
    void setFlashPower(FlashPower flashPower);

    // Getters
    ExposureMode getExposureMode() const;
    Aperture getAperture() const;
    ShutterSpeed getShutterSpeed() const;
    ISO getISO() const;
    FlashPower getFlashPower() const;

    // Get current value
    int getValueExposureMode() const;
    float getValueAperture() const;
    int getValueShutterSpeed() const;
    int getValueISO() const;
    float getValueFlashPower() const;
    void printSettings() const;

private:
    ExposureMode exposureMode_;
    Aperture aperture_;
    ShutterSpeed shutterSpeed_;
    ISO iso_;
    FlashPower flashPower_;

    mutable std::shared_mutex sharedMutex;
};



#endif // _SETTINGS_H_

