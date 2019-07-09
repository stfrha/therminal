#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

#include <iostream>
#include <fstream>
#include <string.h>
#include <time.h>

#include "tempsensors.h"

using namespace std;

int main(int argc, char *argv[])
{
   bool light = false;
   
   TempSensors ts;
   RelayControl rc;

   cout << "Welcome to THERMINAL pool solar cather mamangement!" << endl;

   cout << "Setting up HW..." << endl;
   
   rc.initializeRelay();   
   ts.initializeTempSensors();
   
   cout << "Starting measurements..." << endl;
   
   while(true)
   {
      ts.sampleSensors();
      rc.setRelay(light, !light);

      light = !light;
      
      sleep(1);
   }

   return 0;
}
