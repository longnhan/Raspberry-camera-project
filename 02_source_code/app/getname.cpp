#include "getname.h"

GetName::GetName()
    : user(std::getenv("USER")), serialpicture(0), serialvideo(0)
{
    configpath = std::string("/home/") + user + "/.camera_config";

    // create file if not exist 
    std::ifstream checkFile(configpath);
    if (!checkFile.good()) {
        std::ofstream createFile(configpath);
        createFile << "picture 0000\nvideo 0000\n";
        createFile.close();
    }
    checkFile.close();

    readSerial(); 
}

GetName::~GetName() = default;

void GetName::readSerial()
{
    std::ifstream file(configpath);
    if (!file.is_open()) {
        std::cerr << "Can't open config file for reading\n";
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.compare(0, 7, "picture") == 0) {
            serialpicture = std::stol(line.substr(8));
        } else if (line.compare(0, 5, "video") == 0) {
            serialvideo = std::stol(line.substr(6));
        }
    }
    file.close();
}

void GetName::writeSerial()
{
    std::ofstream file(configpath, std::ios::trunc); 
    if (!file.is_open()) {
        std::cerr << "Can't open config file for writing\n";
        return;
    }

    file << "picture " << std::setw(4) << std::setfill('0') << serialpicture << "\n";
    file << "video "   << std::setw(4) << std::setfill('0') << serialvideo << "\n";

    file.close();
}

std::string GetName::getpathpicture()
{
    std::ostringstream oss;
    oss << std::setw(4) << std::setfill('0') << serialpicture;
    ++serialpicture;
    writeSerial(); // Cập nhật serial mới vào file

    return getpathfolder() + "P" + oss.str() + ".jpg";
}

std::string GetName::getpathvideo()
{
    std::ostringstream oss;
    oss << std::setw(4) << std::setfill('0') << serialvideo;
    ++serialvideo;
    writeSerial(); 

    return getpathfolder() + "V" + oss.str() + ".mp4";
}

std::string GetName::getpathfolder()
{
    return std::string("/home/") + user + "/media/";
}

