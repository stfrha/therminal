#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <fstream>
#include <time.h>

#include <wiringPi.h>

#include "logger.h"

using namespace std;

const string  Logger::c_logFileName = "therminal_log.txt"; 

bool Logger::logFileExists(void)
{
    ifstream f(c_logFileName.c_str());
    return f.good();
}

void Logger::createLogFile(void)
{
   
   // Latest status is on the form:
   // {pool temp: 06.5},{solar temp 33.9},{filter pump on/off},{solar pump on/off},{manual/auto},{date},{time}
   // Example: "06.2,33.9,on,off,auto,2019-07-14,23:37:45"

   std::ofstream outfile;

   outfile.open(c_logFileName, std::ios_base::app);
   outfile << "Pool Temperature,Solar Temperature,Filter Pump state,Solar Pump state,Working State,Date,Time" << endl; 
}

std::string Logger::getDate(void)
{
   time_t now;
   struct tm* timeInfo;
   char buf[sizeof "2011-10-08e"];

   time(&now);
   timeInfo = localtime(&now);
   strftime(buf, sizeof buf, "%Y-%m-%d", timeInfo);
   std::string nowStr = buf;
   return nowStr;
}

std::string Logger::getTime(void)
{
   time_t now;
   struct tm* timeInfo;
   char buf[sizeof "23:45:23e"];

   time(&now);
   timeInfo = localtime(&now);
   strftime(buf, sizeof buf, "%H:%M:%S", timeInfo);
   std::string nowStr = buf;
   return nowStr;
}

Logger::Logger()
{
}

void Logger::initializeLog(void)
{
   // Check if file exists and if not, create it 
   // So there will always be a file available
   if (!logFileExists())
   {
      createLogFile();
   }
}

void Logger::writeLogEntry(const std::string& status)
{
   std::ofstream outfile;

   outfile.open(c_logFileName, std::ios_base::app);
   outfile << status << endl; 
}

