#include <iostream>
#include <fstream>
#include <string>

class Logger
{
public:
  
private:
   static const std::string  c_logFileName;
   
   pugi::xml_node readFile(void);
   void writeFile(pugi::xml_node node);
         
public:
   Logger();
      
   void creatLogEntry(float solarTemp, float poolTemp, bool solar, bool filter);
};