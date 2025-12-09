#include "photo_management.h"

/**
 * @brief check available folder
 * 
 * @param path to the usb or sdcard to check existing
 * @return true 
 * @return false 
 */
bool canWriteFolder(const std::string& path)
{
    std::string test = path + ".__test__";
    std::ofstream f(test);

    if (!f.is_open())
        return false;

    f << "ok";
    f.close();
    std::remove(test.c_str());
    return true;
}

photo_management::photo_management()
    : user(std::getenv("USER")), serialpicture(0), serialvideo(0)
{
    std::string selectedPath;

    if (canWriteFolder(FOLDER_PATH_2))
        selectedPath = FOLDER_PATH_2; //usb insert
    else
        selectedPath = FOLDER_PATH_1; //raspberry internal storage

    folderPath = selectedPath;
    
    //save photo number
    cntFilePath = folderPath + ".camera_save_photo_count";

    struct stat st;
    if (stat(folderPath.c_str(), &st) != 0)
    {
        if (mkdir(folderPath.c_str(), 0755) != 0)
        {
            std::cerr << "Failed to create folder: " << folderPath << "\n";
        }
    }

    std::ifstream check(cntFilePath);
    if (!check.good())
    {
        std::ofstream create(cntFilePath);
        create << "picture 0000\nvideo 0000\n";
        create.close();
    }
    check.close();
    //read latest
    read_photo_ordinal_num();

    std::cout << "[photo_management] Using folder: " << folderPath << "\n";
    std::cout << "[photo_management] Count file: "  << cntFilePath << "\n";
}

photo_management::~photo_management() = default;


/**
 * @brief read latest cnt number at chosen path
 * 
 */
void photo_management::read_photo_ordinal_num()
{
    std::ifstream file(cntFilePath);
    if (!file.is_open()) {
        std::cerr << "Can't open count file for reading\n";
        return;
    }

    std::string type;
    long value;

    while (file >> type >> value)
    {
        if (type == "picture") serialpicture = value;
        else if (type == "video") serialvideo = value;
    }
}

/**
 * @brief update new cnt number for next photo
 * 
 */
void photo_management::write_photo_ordinal_num()
{
    std::ofstream file(cntFilePath, std::ios::trunc);
    if (!file.is_open()) {
        std::cerr << "Can't open count file for writing\n";
        return;
    }

    file << "picture " << std::setw(4) << std::setfill('0') << serialpicture << "\n";
    file << "video "   << std::setw(4) << std::setfill('0') << serialvideo   << "\n";
}

std::string photo_management::getpathpicture()
{
    std::ostringstream oss;
    oss << std::setw(4) << std::setfill('0') << serialpicture;

    ++serialpicture;
    write_photo_ordinal_num();

    return folderPath + "P" + oss.str() + ".jpg";
}


std::string photo_management::getpathvideo()
{
    std::ostringstream oss;
    oss << std::setw(4) << std::setfill('0') << serialvideo;

    ++serialvideo;
    write_photo_ordinal_num();

    return folderPath + "V" + oss.str() + ".mp4";
}

std::string photo_management::getpathfolder()
{
    return folderPath;
}
