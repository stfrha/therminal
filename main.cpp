#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

#include <iostream>
#include <fstream>
#include <string.h>
#include <time.h>
#include <wiringPi.h>

#include "tempsensors.h"

using namespace std;

int main(int argc, char *argv[])
{


   // Lets create a file
   ofstream fs;
   fs.open("frha_test.txt");
   fs << "Hello file system.\n";
   fs.close();

   wiringPiSetup();
   int solarPumpLed = 2;
   int filterPumpLed = 3;
   int solarPump = 4;
   int filterPump = 5;
   int light1 = HIGH;
   int light2 = LOW;

   cout << "Welcome to THERMINAL pool solar casther mamangement!" << endl;

   cout << "Setting up HW..." << endl;

   pinMode(solarPumpLed, OUTPUT);
   pinMode(filterPumpLed, OUTPUT);
   pinMode(solarPump, OUTPUT);
   pinMode(filterPump, OUTPUT);

   cout << "Will now wait for three seconds..." << endl;

   sleep(3);

   cout << "Continuing..." << endl;
   
   TempSensors ts;
   
   ts.initializeTempSensors();
   
   while(1)
   {
      ts.sampleSensors();

      digitalWrite(solarPumpLed, light1);
      digitalWrite(filterPumpLed, light2);
      digitalWrite(solarPump, light1);
      digitalWrite(filterPump, light2);

      if (light1 == HIGH)
      {
         light1 = LOW;
         light2 = HIGH;
      }
      else
      {
         light1 = HIGH;
         light2 = LOW;
      }
      
      sleep(1);
   }

   return 0;
}
