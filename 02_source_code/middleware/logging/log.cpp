#include <iostream>
#include "log.h"
 
// Default static variable
std::mutex logger::mtx;
logger* logger::loggerPtr = nullptr;
// Constructor
logger::logger():
                errorFile(ERRORPATH,std::ios::app),
                debugFile(DEBUGPATH,std::ios::app),
                statusFile(STATUSPATH,std::ios::app)
{  
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
    std::stringstream ostr;
    // Store data of one row
    ostr << serial << "," << getCurrentTime() << "," << fileName << "," << row << "," <<message << std::endl;
    // Write log to Log file
    switch(logLevel_)
    {
        case logLevel::Error:
        {
            errorFile << ostr.str();
            errorFile.flush();
            break;
        }
        case logLevel::Debug:
        {
            debugFile << ostr.str();
            debugFile.flush();
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
