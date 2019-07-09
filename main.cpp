#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

#include <iostream>
#include <fstream>
#include <string.h>
#include <time.h>

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
   
   while(true)
   {
      ts.sampleSensors();
      rc.setRelays(light, !light);
      log.creatLogEntry(
         ts.getLatestTemperature(TempSensors::poolSensor), 
         ts.getLatestTemperature(TempSensors::solarSensor),
         light,
         !light);

      light = !light;
      
      sleep(1);
   }

   return 0;
}
