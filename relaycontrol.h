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
      
   
public:
   RelayControl();
   
   // Returns 0 if success, otherwise some error number
   int initializeRelay();
   
   void setRelay(bool solar, bool filter);
};