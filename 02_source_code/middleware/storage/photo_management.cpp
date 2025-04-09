#include "photo_management.h"

photo_management::photo_management()
    : user(std::getenv("USER")), serialpicture(0), serialvideo(0)
{
    cntFilePath = std::string(COUNTING_FILE);

    // Check and create the folder if it doesn't exist
    struct stat st;
    if (stat(FOLDER_PATH, &st) != 0)
    {
        if (mkdir(FOLDER_PATH, 0755) != 0)
        {
            std::cerr << "Failed to create folder: " << FOLDER_PATH << "\n";
        }
    }

    // create counting file if not exist 
    std::ifstream checkFile(cntFilePath);
    if (!checkFile.good())
    {
        std::ofstream createFile(cntFilePath);
        // init count value for image and video
        createFile << "picture 0000\nvideo 0000\n";
        createFile.close();
    }
    checkFile.close();

    read_photo_ordinal_num(); 
}

photo_management::~photo_management() = default;

/**
 * @brief Get current ordinal photo number from file
 * 
 */
void photo_management::read_photo_ordinal_num()
{
    std::ifstream file(cntFilePath);
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

/**
 * @brief Update the new photo ordinal number into file
 * 
 */
void photo_management::write_photo_ordinal_num()
{
    std::ofstream file(cntFilePath, std::ios::trunc); 
    if (!file.is_open()) {
        std::cerr << "Can't open config file for writing\n";
        return;
    }

    file << "picture " << std::setw(4) << std::setfill('0') << serialpicture << "\n";
    file << "video "   << std::setw(4) << std::setfill('0') << serialvideo << "\n";

    file.close();
}

/**
 * @brief Create picture name as format P[number].jpg
 * Combine folder path with picture name
 * 
 * @return std::string picture full path
 */
std::string photo_management::getpathpicture()
{
    std::ostringstream oss;
    oss << std::setw(4) << std::setfill('0') << serialpicture;
    ++serialpicture;
    write_photo_ordinal_num();

    return getpathfolder() + "P" + oss.str() + ".jpg";
}

/**
 * @brief Create video name as format V[number].mp4
 * Combine folder path with video name
 * 
 * @return std::string video full path
 */
std::string photo_management::getpathvideo()
{
    std::ostringstream oss;
    oss << std::setw(4) << std::setfill('0') << serialvideo;
    ++serialvideo;
    write_photo_ordinal_num(); 

    return getpathfolder() + "V" + oss.str() + ".mp4";
}

/**
 * @brief Return picture folder's path
 * 
 * @return std::string folder's path
 */
std::string photo_management::getpathfolder()
{
    return std::string(FOLDER_PATH);
}
