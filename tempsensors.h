#ifndef TEMPSENSORS_H
#define TEMPSENSORS_H


#include <iostream>
#include <fstream>
#include <string>

class TempSensors
{
public:
   enum TempSensorId
   {
      solarSensor = 0,
      poolSensor = 1
   };
  
private:
   static const std::string  c_configFileName;
   std::string   m_sensorPaths[2];
   float    m_latestTemperature[2];
   
   bool configFileExists(void);
   bool readXmlFile(void);
   bool writeXmlFile(void);
   float sampleSensor(const std::string& sensorPath );
   bool configurationSequence(void);
   
   
public:
   TempSensors();
   
   // Returns 0 if success, otherwise some error number
   int initializeTempSensors();
   
   void sampleSensors();
   float getLatestTemperature(TempSensorId id);
};

#endif