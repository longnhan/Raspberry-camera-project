#ifndef _CAMERA_APP_H_
#define _CAMERA_APP_H_


/*
** @brief Camera application control logic camera middlware 
**
*/

#include <fstream>
#include "camera_control.h"
#include "photo_management.h"
#include "cameraSetting.h"
#include "log.h"

class CameraApp: public CameraControl
{
    public:
        CameraApp(ISO iso = ISO()
        , ShutterSpeed shuttleSpeed = SHUTTLE_SPEED() 
        , ExposureMode exposureMode = EXPOSURE_MODE()
        , Aperture aperture = APERTURE()
        , FlashPower flashPower = FLASH_POWER());

        virtual ~CameraApp();
        bool initialize();

        // Set camera parameters
        void setISO(int iso);
        void setShutterSpeed(int shutterSpeed);
        void setExposure(int exposureMode);
    
        // Capture an image
        bool captureImage(const std::string &image_path);
    
        // Release resources
        void release();
    
    private:
        // virtual void setISO(int iso) override final;
        // virtual void setShutterSpeed(int speed) override final;
        // virtual void setExposure(int exposure) override final;
    

};

#endif //_CAMERA_APP_H_
