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
   int highLed = 2;
   int lowLed = 3;

   pinMode(highLed, OUTPUT);
   pinMode(lowLed, OUTPUT);

   cout << "Hello world, testing wiringPi..." << endl;

   TempSensors ts;
   
   ts.initializeTempSensors();
   
   while(1)
   {
      ts.sampleSensors();

      digitalWrite(highLed, HIGH);
      digitalWrite(lowLed, LOW);

      int t = highLed;
      highLed = lowLed;
      lowLed = t;
      
      sleep(1);
   }

   return 0;
}
