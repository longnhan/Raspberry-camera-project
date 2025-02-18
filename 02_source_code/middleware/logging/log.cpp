#include <iostream>
#include "log.h"
 
// Constructor
logger::logger()                
{   
    // Create directory log file
    umask(0);
    if(mkdir(LOG_DIR.c_str(),0777) == 0  )
    {
        std::cerr << "Create directory " << LOG_DIR << " successfull" << std::endl;
    }
    else
    {
        std::cerr << "Directory " << LOG_DIR << " exist or can not create" << std::endl;
    }

    // Create sub directory log file
    umask(0);
    if(mkdir(LOG_SUB_DIR.c_str(),0777)==0 )
    {
        std::cerr << "Create directory " << LOG_SUB_DIR << " successfull" << std::endl;
    }
    else
    {
        std::cerr << "Directory " << LOG_SUB_DIR << " exist or can not create" << std::endl;
    }
    // Open log file
    errorFile.open(ERRORFILE,std::ios::app);
    debugFile.open(DEBUGFILE,std::ios::app);
    statusFile.open(STATUSFILE,std::ios::app);
    // Check error open file 
    if((!errorFile)||(!debugFile)||(!statusFile))
    {
        std::cerr << "can not open log file "<<std::endl;
        exit(1);
    }
   
    errorFile << "Serial,Time,File,Row,Message" << std::endl;
    errorFile.flush();
    debugFile << "Serial,Time,File,Row,Message" << std::endl;
    debugFile.flush();
    statusFile << "Serial,Time,File,Row,Message" << std::endl;
    statusFile.flush();
}
// Destructor
logger::~logger()
{  
    // Push data remaining to log before close file
    // Error file
    errorFile.flush();
    errorFile.close();
    // Debug file
    debugFile.flush();
    debugFile.close();
    // Status file
    statusFile.flush();
    statusFile.close();
   
    delete loggerPtr;
}
std::string logger::getDirLog()
{
    
    std::string file = "log_file_" ;
    // This lambda run one time per startup
    static  std::string time = [this]()
    {
         std::string time = this->getCurrentTime();
        
        time = time.erase(time.length()-4,4);
        // Change space in time to '_'
        time[10] = '_';
        return time;
    }();
    
    file += time;
    return file;
            
}        
std::string logger::getCurrentTime()
{
    // Get current time
    auto now = std::chrono::system_clock::now();
 
    // Convert to system time
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
 
    // Convert to tm (local time)
    std::tm* local_time = std::localtime(&now_time);
 
    // Storage time as format HH:MM:SS
    std::stringstream os;
    os << std::put_time(local_time, "%Y-%m-%d %H:%M:%S");
 
    // push milisecond to my storage
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    os << "." << std::setw(3) << std::setfill('0') << milliseconds.count() ;
    return os.str();
}                          
logger* logger::getInstance()
{
    // Prevent data race when multithread access
    std::lock_guard<std::mutex> lock(mtx);
    if(loggerPtr == nullptr)
    {
        loggerPtr =  new logger;
    }
   
    return loggerPtr;
}
//
void logger::writeLog(std::stringstream& os)
{
    // Endpoint for variadic
   
}
 
// Method native write log to log file
void logger::writeLogIpm(logLevel logLevel_,std::string fileName, int row, std::string message)
{
    static std::uint64_t serial = 0;
    ++ serial;
    std::stringstream ostr {};
    // Store data of one row
    ostr << serial << "," << getCurrentTime() << "," << fileName << "," << row << "," <<message << std::endl;
    // Write log to Log file
    switch(logLevel_)
    {
        case logLevel::Error:
        {
            errorFile << ostr.str();
            errorFile.flush();
            exit(1);
        }
        case logLevel::Debug:
        {
            #if defined(ENABLE_LOG_TERMINAL)
                std::cerr << message << std::endl;
            #endif

            #if defined(ENABLE_LOG_FILE)
            debugFile << ostr.str();
            debugFile.flush();
            #endif

            break;
        }
        case logLevel::Status:
        {
            statusFile << ostr.str();
            statusFile.flush();
            break;
        }
    }
 
 
    return;                  
}
void logger::clearLog()
{
    // Get path log
    std::filesystem::path logpath = LOG_DIR ;

    try {
        // Delete all log if log file exist
        if (std::filesystem::exists(logpath)) 
        {
            std::filesystem::remove_all(logpath);
            std::cerr << "Delete all log in : " << logpath << std::endl;
        } 
        else 
        {
            std::cerr << "No log to delete " << std::endl;
        }
    } 
    catch (const std::filesystem::filesystem_error& e) 
    {
        std::cerr << "Error deleting file: " << e.what() << std::endl;
    }
}

