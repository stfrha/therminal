#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <fstream>
#include <time.h>
#include <string>
#include <experimental/filesystem>
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>

namespace fs = std::experimental::filesystem;

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

Logger::Logger(int logFileSize) :
   c_logFileSize(logFileSize)
{
   // First check if archive directory exists:
   
   struct stat info;

   if( stat( "archive", &info ) != 0 )
   {
      cout << "Cannot access archive directory" << endl;
      
      fs::create_directory("archive");
   }
   else if( info.st_mode & S_IFDIR )  // S_ISDIR() doesn't exist on my windows 
   {
      cout << "archive is found and is a directory" << endl;
   }
   else
   {
      cout << "archive exists but is is not a directory" << endl;
      exit(0);
   }

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

void Logger::fetchOldestEntry(std::string& oldEntry)
{
   std::ifstream infile;
   std::ofstream outfile;
   std::string tempString;

   infile.open(c_logFileName, std::ios_base::in);
   outfile.open("temp_file.txt", std::ios_base::app);
   
   // Gobble up header
   std::getline(infile, tempString);
   outfile << tempString << endl;
   
   // Read oldest entry
   std::getline(infile, oldEntry);
   
   // Now loop, writing the last of the file to temporary filebuf
   while (infile.peek() != EOF)
   {
      std::getline(infile, tempString);
      outfile << tempString << endl;
   }
   
   infile.close();
   outfile.close();
   
   // Now swap files
   int result = rename("temp_file.txt", c_logFileName.c_str());
   
   if (result != 0)
   {
      cout << "Error: Could not rename log file!!" << endl;
   }
   
   remove("temp_file.txt");
}

std::string  Logger::getActiveArchiveFile(void)
{
   // Archive files are named: "archive_log_n.txt"
   // where n is an iterator. The highest n of all available
   // files is the active archive. If this is too big, a new
   // file is created with n+1 in its name
   
   int maxN = 0;

   // cout << "Iterating archive files..." << endl;
   
   for (const auto & entry : fs::directory_iterator("archive/"))
   {
      std::string entryStr = fs::absolute(entry.path()).string();

      // cout << "Found file: " << entryStr << endl;
      
      std::string fileName = entryStr.substr(entryStr.find_last_of("/") + 1);
      
      // cout << "Found file name: " << fileName << endl;

      // Split file name to get the value n.
      std::string numString = fileName.substr(12, fileName.length() - 16);

      int n = atoi(numString.c_str());
      
      if (n > maxN)
      {
         maxN = n;
      }
   }
   
   // cout << "Highest number of archive files are: " << maxN << endl;
   
   string activeArchiveFileName = "archive_log_" + std::to_string(maxN) + ".txt"; 
   
   // Now test the file size of activeArchiveFileName
   if (filesize(("archive/" + activeArchiveFileName).c_str()) > c_logFileSize)
   {
      activeArchiveFileName = "archive_log_" + std::to_string(maxN + 1) + ".txt"; 
      cout << "Had to increase the archive file index to: " << maxN + 1 << endl;
   }
    
   return "archive/" + activeArchiveFileName;
}

std::ifstream::pos_type Logger::filesize(const char* filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg(); 
}

void Logger::writeEntryToArchive(const std::string& entry)
{
   std::ofstream outfile;

   outfile.open(getActiveArchiveFile(), std::ios_base::app);
   outfile << entry << endl; 
}

void Logger::writeLogEntry(const std::string& status)
{
   // Test if active log-file is close at being too big.
   // I.e. if more than 15000 entries, or 652500 bytes.
   if (filesize(c_logFileName.c_str()) > c_logFileSize)
   {
      // If so, find oldest entry...
      std::string oldestEntry;
      fetchOldestEntry(oldestEntry);
      
      // cout << "Oldest entry: " << oldestEntry << endl;

      // ...remove it from file...

      
      // ...and write to archive
      writeEntryToArchive(oldestEntry);
   }
 
   // Then add new entry
   std::ofstream outfile;

   outfile.open(c_logFileName, std::ios_base::app);
   outfile << status << endl; 
}

