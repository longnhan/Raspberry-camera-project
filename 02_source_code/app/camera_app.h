#ifndef _CAMERA_APP_H_
#define _CAMERA_APP_H_


/*
** @brief Camera application control logic camera middlware 
**
*/

#include <fstream>
#include "camera_control.h"
#include "photo_management.h"
#include "settings.h"
#include "log.h"

class CameraApp: public CameraControl
{
    public:
    // Constructor
        CameraApp(ISO iso = ISO()
        , ShutterSpeed shuttleSpeed = SHUTTLE_SPEED() 
        , ExposureMode exposureMode = EXPOSURE_MODE()
        , Aperture aperture = APERTURE()
        , FlashPower flashPower = FLASH_POWER());

        ~CameraApp();

        virtual bool initialize();

        // Set camera parameters
        virtual void setISO(ISO iso);
        virtual void setShutterSpeed(ShutterSpeed shuttleSpeed);
        virtual void setExposure(ExposureMode exposureMode);
    
        // Capture an image
        virtual bool captureImage(const std::string &image_path) override final;
    
        // Release resources
        virtual void release() override final;
    
    private:
        virtual void setISO(int iso) override final;
        virtual void setShutterSpeed(int speed) override final;
        virtual void setExposure(int exposure) override final;
    

};


#endif //_CAMERA_APP_H_
