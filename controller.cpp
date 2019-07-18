#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sstream>
#include <iomanip>
#include <time.h>

// For threads
#include <pthread.h>

#include "controller.h"

using namespace std;

pthread_mutex_t globalStatusMutex = PTHREAD_MUTEX_INITIALIZER;

// Latest status is on the form:
// {pool temp: 06.5},{solar temp 33.9},{filter pump on/off},{solar pump on/off},{manual/auto}
// Example: "06.2,33.9,on,off,auto"

string g_latestStatus;



Controller::Controller() :
   m_state(automatic)
{
   
}

void Controller::initializeController(void)
{
   cout << "Setting up HW..." << endl;
   
   m_rc.initializeRelays();   
   m_ts.initializeTempSensors();
   m_log.initializeLog();
   
   pthread_t thread;

   result = pthread_create(&thread, NULL, monitorThread, this);
   if (result)
   {
      cout << "Monitor thread could not be created, " << result << endl;
      exit(1);
   }

}

void* Controller::monitorThread(void* cntrlPointer)
{
   Controller* instance = (Controller*) cntrlPointer;   
   
   while(true)
   {
      instance->executeStep();
      sleep(12);
   }
}


/* Messages are:

STEP  - Execute step of measurements and pump control
AUTO  - Go to automatic control
MANL  - Go to manual control
S_ON  - Force Solar pump on, ignored in AUTO
SOFF  - Force Solar pump off, ignored in AUTO
F_ON  - Force Filter pump on, ignored in AUTO
FOFF  - Force Filter pump off, ignored in AUTO
SREQ  - Request status string

*/


// The following method will be called by and executed in
// another thread than the main
// This method is the only place (after initialization)
// where the m_state member and the relays are set. Therefore
// there is no need to mutex protect them. 

void Controller::executeCommand(std::string command)
{
   if (command == "SREQ")
   {
      // Send status;
      prepareStatusMessage();
   }
   else if (command == "AUTO")
   {
      m_state = automatic;
      prepareStatusMessage();

      cout << "State is now automatic." << endl;
   }
   else if (command == "MANL")
   {
      m_state = manual;
      prepareStatusMessage();

      cout << "State is now manual." << endl;
   }
   else if (command == "S_ON")
   {
      if (m_state == manual)
      {
         m_rc.setRelay(RelayControl::solarPump, true);
         prepareStatusMessage();

         cout << "Solar pump is on." << endl;
      }
   }
   else if (command == "SOFF")
   {
      if (m_state == manual)
      {
         m_rc.setRelay(RelayControl::solarPump, false);
         prepareStatusMessage();

         cout << "Solar pump is off." << endl;
      }
   }
   else if (command == "F_ON")
   {
      if (m_state == manual)
      {
         m_rc.setRelay(RelayControl::filterPump, true);
         prepareStatusMessage();

         cout << "Filter pump is on." << endl;
      }
   }
   else if (command == "FOFF")
   {
      if (m_state == manual)
      {
         m_rc.setRelay(RelayControl::filterPump, false);
         prepareStatusMessage();

         cout << "Filter pump is off." << endl;
      }
   }
   else 
   {
      cout << "In future, will execute command: " << command << endl;
   }
}

string Controller::generateStatusMessage(int precision)
{
   // Latest status is on the form:
   // {pool temp: 06.5};{solar temp 33.9};{filter pump on/off};{solar pump on/off};{manual/auto};{date};{time}
   // Example: "06.2;33.9;on;off;auto;2019-07-14;23:37:45"

   ostringstream statStream;
   
   statStream << setprecision(precision) << m_ts.getLatestTemperature(TempSensors::poolSensor) << ";";
   statStream << m_ts.getLatestTemperature(TempSensors::solarSensor) << ";";
   statStream << m_rc.getRelay(RelayControl::filterPump) << ";";
   statStream << m_rc.getRelay(RelayControl::solarPump) << ";";

   if (m_state == automatic)
   {
      statStream << "auto";
   }
   else
   {
      statStream << "manual";
   }
   
   statStream << ";" << m_log.getDate() << ";" << m_log.getTime();
   
   return statStream.str();
}


void Controller::prepareStatusMessage(void)
{
   string stat = generateStatusMessage(3);

   // This method is called from multiple threads
   // The global status needs thus be protected
   // by mutex.
   pthread_mutex_lock(&globalStatusMutex);
   g_latestStatus = generateStatusMessage(3);
   pthread_mutex_unlock(&globalStatusMutex);
}

// The following method will be called by and executed in
// another thread than the main
void Controller::executeStep(void)
{
   m_ts.sampleSensors();
   
   if (m_state == automatic)
   {
      m_rc.setRelays(false, false);
   }

   // Generate one log entry to log file and one
   // status message to send on socket
   m_log.writeLogEntry(generateStatusMessage(5));
   prepareStatusMessage();

   cout << "Current temperatures: Solar: " << m_ts.getLatestTemperature(TempSensors::solarSensor)
      << ", Pool: " << m_ts.getLatestTemperature(TempSensors::poolSensor)
      << ", Solar pump: " << m_rc.getRelay(RelayControl::solarPump)
      << ", Filter pump: " << m_rc.getRelay(RelayControl::filterPump) << endl;
}