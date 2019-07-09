#include <iostream>
#include <fstream>
#include <string>

#include "../pugixml/src/pugixml.hpp"

class Logger
{
public:
  
private:
   static const std::string  c_logFileName;
   
   // Temporary objects that hold the document and root node
   // These only contains defined data after a readFile() has 
   // been executed and until writeFile() has been executed.
   pugi::xml_document* m_tmpDoc;    // Valid after readFile()
   pugi::xml_node m_tmpRootNode;    // Valid after readFile()
   pugi::xml_node m_tmpSessionNode; // Valid after readSession()
   
   bool logFileExists(void);
   void readFile(void);
   void readSession(void);
   void writeFile(void);
   void createLogFile(void);
   void createSessionEntry(void);
   std::string getTime(void);
         
public:
   Logger();
   
   void initializeLog(void);
      
   void creatLogEntry(float solarTemp, float poolTemp, bool solar, bool filter);
};