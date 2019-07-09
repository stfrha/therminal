#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

#include <time.h>

#include <wiringPi.h>

#include "logger.h"

using namespace std;

const string  Logger::c_logFileName = "therminal_log.xml"; 

bool Logger::logFileExists(void)
{
    ifstream f(c_logFileName.c_str());
    return f.good();
}


void Logger::readFile(void)
{
   m_tmpDoc = new pugi::xml_document;

   pugi::xml_parse_result result = m_tmpDoc->load_file(c_logFileName.c_str());

   m_tmpRootNode = m_tmpDoc->child("therminalLog");
}

void Logger::readSession(void)
{
   readFile();
   
   for (auto last_session = m_tmpRootNode.children("therminalSession").begin();
      last_session != m_tmpRootNode.children("therminalSession").end();
      ++last_session)
   {
      m_tmpSessionNode = (*last_session);
   }

//   m_tmpSessionNode = m_tmpRootNode.last_child("therminalSession");
}

void Logger::writeFile(void)
{
   m_tmpDoc->save_file(c_logFileName.c_str());
   
   delete m_tmpDoc;
   m_tmpDoc = NULL;
}

void Logger::createLogFile(void)
{
   pugi::xml_document doc;

   pugi::xml_node root = doc.append_child("therminalLog");
   
   doc.save_file(c_logFileName.c_str());
}


void Logger::createSessionEntry(void)
{
   readFile();
   
   m_tmpSessionNode = m_tmpRootNode.append_child("therminalSession");

   pugi::xml_attribute dateTimeAttr = m_tmpSessionNode.append_attribute("dateTime");
   
   dateTimeAttr.set_value(getTime().c_str());

   writeFile();
}

std::string Logger::getTime(void)
{
   time_t now;
   time(&now);
   char buf[sizeof "2011-10-08T07:07:07Z"];
   strftime(buf, sizeof buf, "%FT%TZ", gmtime(&now));
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

   createSessionEntry();
}


void Logger::creatLogEntry(float solarTemp, float poolTemp, bool solar, bool filter)
{
   readSession();
   
   pugi::xml_node entryNode = m_tmpSessionNode.append_child("logEntry");

   pugi::xml_attribute dateTimeAttr = entryNode.append_attribute("dateTime");
   dateTimeAttr.set_value(getTime().c_str());

   pugi::xml_attribute poolTempAttr = entryNode.append_attribute("poolTemp");
   poolTempAttr.set_value(poolTemp);

   pugi::xml_attribute solarTempAttr = entryNode.append_attribute("solarTemp");
   solarTempAttr.set_value(solarTemp);

   pugi::xml_attribute filterPumpAttr = entryNode.append_attribute("filterPump");
   filterPumpAttr.set_value(filter);
   
   pugi::xml_attribute solarPumpAttr = entryNode.append_attribute("solarPump");
   solarPumpAttr.set_value(solar);
   
   writeFile();
}

