#ifndef _GETNAME_H_
#define _GETNAME_H_

#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <iostream>

class GetName
{
public:
    GetName();
    ~GetName();
    std::string getpathpicture();
    std::string getpathvideo();
    std::string getpathfolder();

private:
    void readSerial();
    void writeSerial();
    std::string configpath;
    std::string user;
    std::fstream f_config;
    long serialpicture;
    long serialvideo;
};

#endif