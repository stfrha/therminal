#include <string>

class Logger
{
public:
  
private:
   static const std::string  c_logFileName;
   
   bool logFileExists(void);
   void createLogFile(void);
         
public:
   Logger();
   
   void initializeLog(void);

   std::string getTime(void);
   std::string getDate(void);
   void writeLogEntry(const std::string& status);
};