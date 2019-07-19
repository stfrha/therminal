
#ifndef CONTROLLER_H
#define CONTROLLER_H


#include <iostream>
#include <fstream>
#include <string>
#include <time.h>

#include "tempsensors.h"
#include "relaycontrol.h"
#include "logger.h"


class Controller
{
public:
   enum ControllerState
   {
      automatic,
      manual
   };

   enum SolarState
   {
      start,      // Run solar pump for three minutes to get steady state
      running,    // As long as tempDiff > 1 deg, keep solar pump on
      heating,    // Stop pump until tempDiff > 15 deg, go to purging
      purging,    // Run pump until tempDiff < 2 deg, go to running
      night       // If tempDiff < 0, shut down pump, go to start if tempDiff > 2
   };
   
private:
   TempSensors m_ts;
   RelayControl m_rc;
   Logger m_log;
   ControllerState m_state;
   
   // Variables for automatic control
   SolarState m_solarState;
   int m_stepsSinceChange;
   
   
   // Constants for automatic control
   int c_minStepsSinceChange;    // No change in solar pump is done faster than this
   float c_tempDiffToStartSolarPump;
   float c_tempDiffToStopSolarPump;
   int c_timeToStartFilter; 
   int c_timeToStopFilter; 
       
   void executeStep(void);
   void prepareStatusMessage(void);
   std::string generateStatusMessage(int precision);
   
   static void* monitorThread(void* threadId);

       
         
public:
   Controller();
   
   void initializeController(void);

   void executeCommand(std::string command);
};


#endif