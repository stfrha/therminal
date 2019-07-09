
// These are needed for some millisecond timing
#include <chrono>
#include <thread>
#include <time.h>
#include <sys/time.h>

#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

#include <iostream>
#include <fstream>
#include <string.h>

#include "tempsensors.h"
#include "relaycontrol.h"
#include "logger.h"

using namespace std;

int main(int argc, char *argv[])
{
   bool light = false;
   
   TempSensors ts;
   RelayControl rc;
   Logger log;

   cout << "Welcome to THERMINAL pool solar catcher mamangement!" << endl;

   cout << "Setting up HW..." << endl;
   
   rc.initializeRelays();   
   ts.initializeTempSensors();
   log.initializeLog();
   
   cout << "Starting measurements..." << endl;
   
   struct timeval tv;
   
   long usStartTime;
   long usNow;
   long usPeriod = 3000000;
   
   cout << "Target time is: " << usPeriod << endl;
  
  
   while(true)
   {
      // Save time at start of this step
      gettimeofday(&tv, NULL); 
      usStartTime = tv.tv_sec * 1000000 + tv.tv_usec;

      // Run monitor step...
      ts.sampleSensors();
      rc.setRelays(light, !light);
      log.creatLogEntry(
         ts.getLatestTemperature(TempSensors::poolSensor), 
         ts.getLatestTemperature(TempSensors::solarSensor),
         light,
         !light);

      cout << "Current temperatures: Solar: " << ts.getLatestTemperature(TempSensors::solarSensor)
         << ", Pool: " << ts.getLatestTemperature(TempSensors::poolSensor) << endl;
         
      light = !light;
      ///... until here

      gettimeofday(&tv, NULL); 
      usNow = tv.tv_sec * 1000000 + tv.tv_usec;

      cout << "Start time was: " << usStartTime << endl;
      cout << "Now is: " << usNow << endl;
      
      cout << "Will now wait for this many milliseconds: " << usPeriod - (usNow - usStartTime) << endl;
      
      std::this_thread::sleep_for(
         std::chrono::microseconds(
            usPeriod - (usNow - usStartTime)));
   }

   return 0;
}
