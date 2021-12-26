#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

#include <iostream>
#include <fstream>
#include <string.h>
#include <time.h>

#include <wiringPi.h>

#include "relaycontrol.h"

using namespace std;

RelayControl::RelayControl()
{
   m_relayControl[solarPump] = 4;
   m_relayControl[filterPump] = 5;
   m_ledControl[solarPump] = 2;
   m_ledControl[filterPump] = 3;
   m_pumpStates[solarPump] = false;
   m_pumpStates[filterPump] = false;
}

int RelayControl::initializeRelays()
{
   wiringPiSetup();

   pinMode(m_ledControl[solarPump], OUTPUT);
   pinMode(m_ledControl[filterPump], OUTPUT);
   pinMode(m_relayControl[solarPump], OUTPUT);
   pinMode(m_relayControl[filterPump], OUTPUT);
   
   setRelays(m_pumpStates[solarPump], m_pumpStates[filterPump]);
}


void RelayControl::setRelays(bool solar, bool filter)
{
   setRelay(solarPump, solar);
   setRelay(filterPump, filter);
}

void RelayControl::setRelay(RelayId relay, bool state)
{
   digitalWrite(m_relayControl[relay], !state);
   digitalWrite(m_ledControl[relay], !state);
   m_pumpStates[relay] = state;

}

bool RelayControl::getRelay(RelayId relay)
{
   return m_pumpStates[relay];
}

