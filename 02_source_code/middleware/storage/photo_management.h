#ifndef _PHOTO_MANAGEMENT_H_
#define _PHOTO_MANAGEMENT_H_

#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <iostream>
#include <sys/stat.h>

// Two possible storage locations
constexpr const char* FOLDER_PATH_1 = "/home/pi/media/";
constexpr const char* FOLDER_PATH_2 = "/media/pi/rudo_foto/";

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

    std::string folderPath;
    std::string cntFilePath;
    std::string user;

    long serialpicture;
    long serialvideo;
};

#endif /* _PHOTO_MANAGEMENT_H_ */
