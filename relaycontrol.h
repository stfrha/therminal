#ifndef RELAYCONTROL_H
#define RELAYCONTROL_H


#include <iostream>
#include <fstream>
#include <string>

class RelayControl
{
public:
   enum RelayId
   {
      solarPump = 0,
      filterPump = 1
   };
  
private:
   int m_relayControl[2];
   int m_ledControl[2];
   bool m_pumpStates[2];
      
   
public:
   RelayControl();
   
   // Returns 0 if success, otherwise some error number
   int initializeRelays();
   
   void setRelays(bool solar, bool filter);
   void setRelay(RelayId relay, bool state);
   bool getRelay(RelayId relay);
};

#endif

