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

RelayControl::RelayControl()
{
   m_relayControl[solarPump] = 4;
   m_relayControl[filterPump] = 5;
   m_ledControl[solarPump] = 2;
   m_ledControl[filterPump] = 3;
}

int RelayControl::initializeRelay()
{
   wiringPiSetup();

   pinMode(solarPumpLed, OUTPUT);
   pinMode(filterPumpLed, OUTPUT);
   pinMode(solarPump, OUTPUT);
   pinMode(filterPump, OUTPUT);
}


void RelayControl::setRelay(bool solar, bool filter)
{
   digitalWrite(m_relayControl[solarPump], solar);
   digitalWrite(m_ledControl[solarPump], solar);
   digitalWrite(m_relayControl[filterPump], filter);
   digitalWrite(m_ledControl[filterPump], filter);
}

