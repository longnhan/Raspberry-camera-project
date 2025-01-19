// Use pattern Singleton
#ifndef LOGGER_H
#define LOGGER_H
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <thread>
#include <mutex>
#include <cstdlib>
 
// Fix path log file
#define ERRORPATH  (std::string(std::getenv("HOME")).append("/error_log.csv"))
#define DEBUGPATH  (std::string(std::getenv("HOME")).append("/debug_log.csv"))
#define STATUSPATH (std::string(std::getenv("HOME")).append("/status_log.csv"))
#define LOG_ERR(...) (logger::getInstance()->writeLog(logLevel::Error, __FILE__, __LINE__, __VA_ARGS__))
#define LOG_DBG(...) (logger::getInstance()->writeLog(logLevel::Debug, __FILE__, __LINE__, __VA_ARGS__))
#define LOG_SST(...) (logger::getInstance()->writeLog(logLevel::Status, __FILE__, __LINE__, __VA_ARGS__))
/***************************************/
// User reference
// Use macro LOG_ERR to write log error
// Use macro LOG_DBG to write log debug
// Use macro LOG_SST to write log status
// Example:
// int temperature = 45 ;
// LOG_ERR("Fail to open file");
// LOG_DBG("This function run very good");
// LOG_SST("Too hot, temperature is :", temperature, " Celsius!" )
/***************************************/
enum logLevel
{  
    Error,
    Debug,
    Status
};
class logger
{
private:
    // Variable ofstream to open log file
    std::ofstream errorFile;
    std::ofstream debugFile;
    std::ofstream statusFile;
 
    // Private constructor
    logger() ;
                                                        // ** Delete unnecessary default functions **
    logger(const logger&) = delete;                     // Delete copy constructor
    logger& operator=(const logger&) = delete;          // Delete copy assignment operator
    logger(logger&&) = delete;                          // Delete move constructor
    logger& operator=(const logger&&) = delete;         // Delete move assignment operator
    // Get time from system
    std::string getCurrentTime() ;
   

    // private variable
    static std::mutex mtx;                               // Mutex to access thread safety
    static logger* loggerPtr;                          // Static pointer to the logger instance
 public:
    // Destructor
    ~logger();  
 
    // Method native write log to log file
    void  writeLogIpm(logLevel logLevel_,std::string fileName, int row, std::string message);
   
    // Endpoint for variadic
    void writeLog(std::stringstream& os);
 
    // Recursive to handle each for each argument
    template <typename T,typename... Types>
    void writeLog(std::stringstream& os, T var1, Types... var2)
    {
        os << var1;
        writeLog(os, var2...);
 
    }
    // Main write log is called by macro LOG_*
    template <typename... Types>
    void writeLog(logLevel logLevel_,std::string fileName, int row,Types... args)
    {
        std::stringstream os;
 
        writeLog(os,args...);
        writeLogIpm(logLevel_,fileName,row,os.str());
    }
    static logger* getInstance();
 
};
 
 
#endif