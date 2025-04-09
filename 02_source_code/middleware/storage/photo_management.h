#ifndef _PHOTO_MANAGEMENT_H_
#define _PHOTO_MANAGEMENT_H_

#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <iostream>
#include <sys/stat.h> 

constexpr const char*  FOLDER_PATH = "/home/pi/media/";
constexpr const char* COUNTING_FILE = "/home/pi/media/.camera_save_photo_count";

class photo_management
{
public:
    photo_management();
    ~photo_management();
    std::string getpathpicture();
    std::string getpathvideo();
    std::string getpathfolder();

private:
    void read_photo_ordinal_num();
    void write_photo_ordinal_num();
    std::string cntFilePath;
    std::string user;
    std::fstream f_config;
    long serialpicture;
    long serialvideo;
};

#endif /*_PHOTO_MANAGEMENT_H_*/
