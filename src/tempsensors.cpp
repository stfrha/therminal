#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

#include <time.h>

#include <wiringPi.h>

#include "../../pugixml/src/pugixml.hpp"

#include "tempsensors.h"

using namespace std;
using namespace pugi;

const string  TempSensors::c_configFileName = "temperature_sensor_config.xml"; 

bool TempSensors::configFileExists(void)
{
    ifstream f(c_configFileName.c_str());
    return f.good();
}

bool TempSensors::readXmlFile(void)
{
   pugi::xml_document doc;

   doc.load_file(c_configFileName.c_str());

   pugi::xml_node root = doc.child("tempSensorConfig");
   
   pugi::xml_node solarSensorNode = root.child("solarSensor");
   pugi::xml_node poolSensorNode = root.child("poolSensor");
   
   if ((solarSensorNode == NULL) || (poolSensorNode == NULL))
   {
      return false;
   }

   m_sensorPaths[solarSensor] = solarSensorNode.attribute("deviceId").as_string("undefined");
   m_sensorPaths[poolSensor] = poolSensorNode.attribute("deviceId").as_string("undefined");

   if ((m_sensorPaths[solarSensor] == "undefined") || (m_sensorPaths[poolSensor] == "undefined"))
   {
      return false;
   }
   
   return true;
}

bool TempSensors::writeXmlFile(void)
{
   pugi::xml_document doc;

   pugi::xml_node root = doc.append_child("tempSensorConfig");
   
   pugi::xml_node solarSensorNode = root.append_child("solarSensor");
   pugi::xml_node poolSensorNode = root.append_child("poolSensor");

   pugi::xml_attribute solarAttr = solarSensorNode.append_attribute("deviceId");
   pugi::xml_attribute poolAttr = poolSensorNode.append_attribute("deviceId");
   
   solarAttr.set_value(m_sensorPaths[solarSensor].c_str());
   poolAttr.set_value(m_sensorPaths[poolSensor].c_str());

   doc.save_file(c_configFileName.c_str());
   
   return true; 
}

bool TempSensors::sampleSensor(const string&  sensorPath, float& tempRet )
{
   char buf[100];
   int fd =-1;
   char *temp;
   float value;

   // Open the file in the path.
   if((fd = open(sensorPath.c_str(),O_RDONLY)) < 0)
   {
      cout << "open temperature sensor file error" << endl;
      return false;
   }
   
   // Read the file
   if(read(fd,buf,sizeof(buf)) < 0)
   {
      cout << "read temperature sensor file error" << endl;
      
      close(fd);
      
      return false;
   }

   close(fd);
   
   // Returns the first index of 't'.
   temp = strchr(buf,'t');
   // Read the string following "t=".
   sscanf(temp,"t=%s",temp);
   // atof: changes string to float.
   value = atof(temp)/1000;
   
   tempRet = value;
   return true;
}


bool TempSensors::configurationSequence(void)
{
   char path[50] = "/sys/bus/w1/devices/";
   string localSensorPaths[2];
   int sensorIndex = 0;
   DIR *dirp;
   struct dirent *direntp;
   
   cout << "Running configuration sequence!" << endl;
   cout << "===============================" << endl;
   cout << "Please wait..." << endl;
  
   // Check if /sys/bus/w1/devices/ exists.
   if((dirp = opendir(path)) == NULL)
   {
      cout << "Error: Devices could not be found." << endl;
      return 1;
   }
   
   // Reads the directories or files in the current directory.
   while((direntp = readdir(dirp)) != NULL)
   {
      // If 28- is the substring of d_name,
      // then copy d_name to rom and print rom.  
      if(strstr(direntp->d_name,"28-"))
      {
         localSensorPaths[sensorIndex++] = string(path) + direntp->d_name;
         // strcpy(rom,direntp->d_name);
         cout << "Found sensor number " << sensorIndex << endl;
      }
   }
   closedir(dirp);
   
   if (sensorIndex == 0)
   {
      cout << "Could not find any sensors." << endl;
      return false;
   }

   if (sensorIndex == 1)
   {
      cout << "Could only find one sensor." << endl;
      return false;
   }
   
   if (sensorIndex > 2)
   {
      cout << "Found more than two sensors." << endl;
      return false;
   }
   
   // Append the String rom and "/w1_slave" to path
   // path becomes to "/sys/bus/w1/devices/28-xxxxxxxxx/w1_slave"
   localSensorPaths[0] += "/w1_slave";
   localSensorPaths[1] += "/w1_slave";
   
   cout << "Sensor 1 path: " << localSensorPaths[0] << endl;
   cout << "Sensor 2 path: " << localSensorPaths[1] << endl;
   
   
   
   float baseTemp[2];
   float currTemp[2];
   
   sampleSensor(localSensorPaths[0], baseTemp[0]);
   sampleSensor(localSensorPaths[1], baseTemp[1]);
   
   int poolSensorIndex = -1;
   
   cout << "Current temperatures: Sensor 1: " << baseTemp[0] << ", Sensor 2: " << baseTemp[1] << endl;
   
   cout << "Warm up the pool sensor by at least four degrees!" << endl;
   cout << "Waiting for temperature change..." << endl;

   do
   {
      sleep(1);
      
      sampleSensor(localSensorPaths[0], currTemp[0]);
      sampleSensor(localSensorPaths[1],currTemp[1]);
      
      cout << "Current temperatures: Sensor 1: " << currTemp[0] << ", Sensor 2: " << currTemp[1] << endl;
      
      for (int i = 0; i < 2; i++)
      {
         if ((currTemp[i] > baseTemp[i] + 4.0))
         {
            poolSensorIndex = i;
         }
      }
   } while (poolSensorIndex == -1);
   
   cout << "Pool sensor is sensor number: " << poolSensorIndex + 1 << endl;
   
   if (poolSensorIndex == 0)
   {
      m_sensorPaths[poolSensor] = localSensorPaths[0];
      m_sensorPaths[solarSensor] = localSensorPaths[1];
   }
   else
   {
      m_sensorPaths[poolSensor] = localSensorPaths[1];
      m_sensorPaths[solarSensor] = localSensorPaths[0];
   }
   
   return true;
}




TempSensors::TempSensors()
{
   m_sensorPowerControlGpio[solarSensor] = 8;
   m_sensorPowerControlGpio[poolSensor] = 9;
}


void TempSensors::resetTempSensors()
{
   digitalWrite(m_sensorPowerControlGpio[solarSensor], false);
   digitalWrite(m_sensorPowerControlGpio[poolSensor], false);

   usleep(500000);
   
   digitalWrite(m_sensorPowerControlGpio[solarSensor], true);
   digitalWrite(m_sensorPowerControlGpio[poolSensor], true);

   usleep(250000);
}

int TempSensors::initializeTempSensors()
{
   // Set power control pins to output_iterator   
   pinMode(m_sensorPowerControlGpio[solarSensor], OUTPUT);
   pinMode(m_sensorPowerControlGpio[poolSensor], OUTPUT);

   digitalWrite(m_sensorPowerControlGpio[solarSensor], true);
   digitalWrite(m_sensorPowerControlGpio[poolSensor], true);

   usleep(250000);
   
   // These two lines mount the device:
   system("sudo modprobe w1-gpio");
   system("sudo modprobe w1-therm");
   
   bool success = false;
   // Check if config file exists
   if (configFileExists())
   {
      // Parse config file
      success = readXmlFile();
      
      if (!success)
      {
         cout << "Error parsing configuration XML file" << endl;
      }
   }
   else
   {
      cout << "Temperature sensor file could not be found!" << endl;
   }

   if (!success)
   {
      // Run configuration sequence
      if (!configurationSequence())
      {
         cout << "Could not configure sensors. Cannot continue." << endl;
         return 1;
      }
     
      // Create config file
      writeXmlFile();
   }
   
   return 0;
}


void TempSensors::sampleSensors()
{
   for (int i = 0; i < 2; i++)
   {
      float t = 0;
      while ((!sampleSensor(m_sensorPaths[i], t)) || (t == 0))
      {
         cout << "Temp exactly zero, resetting." << endl;
         resetTempSensors();
      }
      
      m_latestTemperature[i] = t;
   }
}

float TempSensors::getLatestTemperature(TempSensorId id)
{
      return m_latestTemperature[id];
}
