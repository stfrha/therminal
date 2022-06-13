#include <string>

class Logger
{
public:
  
private:
   static const std::string  c_logFileName;
   const int c_logFileSize;
   
   bool logFileExists(void);
   void createLogFile(void);
   void fetchOldestEntry(std::string& oldEntry);
   std::string getActiveArchiveFile(void);
   std::ifstream::pos_type filesize(const char* filename);
   void writeEntryToArchive(const std::string& entry);


   
   
   
         
public:
   Logger(int logFileSize);
   
   void initializeLog(void);

   std::string getTime(void);
   std::string getDate(void);
   void writeLogEntry(const std::string& status);
};