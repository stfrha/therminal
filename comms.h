
class Comms
{
private:
   static void* stepThread(void* threadId);
   static void* serverThread(void* threadId);
   static void handleMessage(int socketFd, char* buffer, int length);

public:   
   Comms();
   
   void initializeComms(void);
   
   // Should probably have a destructor that stops all sockets
   

};
