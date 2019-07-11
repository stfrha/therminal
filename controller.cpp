#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

#include <time.h>

#include "controller.h"

using namespace std;

Controller::Controller() :
   m_light(false),
   m_state(automatic)
{
   
}

void Controller::initializeController(void)
{
   cout << "Setting up HW..." << endl;
   
   m_rc.initializeRelays();   
   m_ts.initializeTempSensors();
   m_log.initializeLog();
}


/* Messages are:

STEP  - Execute step of measurements and pump control
AUTO  - Go to automatic control
MANL  - Go to manual control
S_ON  - Force Solar pump on, ignored in AUTO
SOFF  - Force Solar pump off, ignored in AUTO
F_ON  - Force Filter pump on, ignored in AUTO
FOFF  - Force Filter pump off, ignored in AUTO

*/


void Controller::executeCommand(std::string command)
{
   if (command == "STEP")
   {
      executeStep();
   }
   else if (command == "AUTO")
   {
      m_state = automatic;
      cout << "State is now automatic." << endl;
   }
   else if (command == "MANL")
   {
      m_state = manual;
      cout << "State is now manual." << endl;
   }
   else if (command == "S_ON")
   {
      if (m_state == manual)
      {
         m_rc.setRelay(RelayControl::solarPump, true);
         cout << "Solar pump is on." << endl;
      }
   }
   else if (command == "SOFF")
   {
      if (m_state == manual)
      {
         m_rc.setRelay(RelayControl::solarPump, false);
         cout << "Solar pump is off." << endl;
      }
   }
   else if (command == "F_ON")
   {
      if (m_state == manual)
      {
         m_rc.setRelay(RelayControl::filterPump, true);
         cout << "Filter pump is on." << endl;
      }
   }
   else if (command == "FOFF")
   {
      if (m_state == manual)
      {
         m_rc.setRelay(RelayControl::filterPump, false);
         cout << "Filter pump is off." << endl;
      }
   }
   else 
   {
      cout << "In future, will execute command: " << command << endl;
   }
}

void Controller::executeStep(void)
{
   // Run monitor step...
   m_ts.sampleSensors();
   
   if (m_state == automatic)
   {
      m_rc.setRelays(m_light, !m_light);
      m_light = !m_light;
   }
   
   m_log.creatLogEntry(
      m_ts.getLatestTemperature(TempSensors::poolSensor), 
      m_ts.getLatestTemperature(TempSensors::solarSensor),
      m_rc.getRelay(RelayControl::solarPump),
      m_rc.getRelay(RelayControl::filterPump));

   cout << "Current temperatures: Solar: " << m_ts.getLatestTemperature(TempSensors::solarSensor)
      << ", Pool: " << m_ts.getLatestTemperature(TempSensors::poolSensor)
      << ", Solar pump: " << m_rc.getRelay(RelayControl::solarPump)
      << ", Filter pump: " << m_rc.getRelay(RelayControl::filterPump) << endl;
      
   //... until here
}