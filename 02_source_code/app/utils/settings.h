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
#define  SetExposureMode(a)   (settings_.setExposureMode(a))
#define  SetAperture(a)       (settings_.setAperture(a))
#define  SetShutterSpeed(a)   (settings_.setShutterSpeed(a))
#define  SetISO(a)            (settings_.setISO(a))
#define  SetFlashPower(a)     (settings_.setFlashPower(a))

// macro to get enum value
#define  ExposureMode()       (settings_.getExposureMode())
#define  Aperture()           (settings_.getAperture())
#define  ShutterSpeed()       (settings_.getShutterSpeed())
#define  ISO()                (settings_.getISO())
#define  FlashPower()         (settings_.getFlashPower())

// macro to get value
#define  ValueExposureMode()  (settings_.getValueExposureMode())
#define  ValueAperture()      (settings_.getValueAperture())
#define  ValueShutterSpeed()  (settings_.getValueShutterSpeed())
#define  ValueISO()           (settings_.getValueISO())
#define  ValueFlashPower()    (settings_.getValueFlashPower())
#define  PrintSettings()      (settings_.printSettings())
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
    Settings(ExposureMode exposureMode, Aperture aperture, ShutterSpeed shutterSpeed, ISO iso, FlashPower flashPower);
    ~Settings() = default;

    // Setters
    void setValue(ExposureMode exposureMode, Aperture aperture, ShutterSpeed shutterSpeed, ISO iso, FlashPower flashPower);
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

