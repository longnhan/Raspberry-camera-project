#include "camera_control.h"
#include <iostream>

CameraControl::CameraControl() {
    cameraManager = std::make_shared<libcamera::CameraManager>();
}

CameraControl::~CameraControl() {
    cameraManager->stop();
}

bool CameraControl::init() {
    if (cameraManager->start()) 
    {
        std::cerr << "Failed to start CameraManager" << std::endl;
        return false;
    }
    return true;
}

void CameraControl::detectAndPrintCamera() 
{
    if (cameraManager->cameras().empty()) 
    {
        std::cerr << "No cameras detected." << std::endl;
        return;
    }
    for (const auto &camera : cameraManager->cameras()) 
    {
        std::cout << "Detected camera: " << camera->id() << std::endl;
    }
}