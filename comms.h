#include "controller.h"

class Comms
{
private:
   Controller* m_cntrl;


   static void* stepThread(void* threadId);
   static void* serverThread(void* threadId);
   static void handleMessage(int socketFd, char* buffer, int length);

public:   
   Comms();
   
   void initializeComms(Controller* cntrl);
   
   // Should probably have a destructor that stops all sockets
   

};
