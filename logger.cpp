#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

#include <iostream>
#include <fstream>
#include <string.h>
#include <time.h>

#include <wiringPi.h>

#include "../pugixml/src/pugixml.hpp"

#include "logger.h"

using namespace std;

const string  TempSensors::c_logFileName = "thermianl_log.xml"; 

pugi::xml_node Logger::readFile(void)
{
   pugi::xml_document doc;

   pugi::xml_parse_result result = doc.load_file(c_logFileName.c_str());

   pugi::xml_node root = doc.child("therminalLog");
   
   return root;
}


void Logger::writeFile(pugi::xml_node node)
{
   
   
}

Logger::Logger()
{
}

void Logger::initializeLog()
{
   // Check if file exists and if not, create it 
   // So there will always be a file available
   
   
}


void Logger::creatLogEntry(float solarTemp, float poolTemp, bool solar, bool filter)
{
   
   
   
}

