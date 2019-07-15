
// These are needed for some millisecond timing
#include <time.h>
#include <sys/time.h>

#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include <pthread.h>

#include "comms.h"
#include "controller.h"

using namespace std;

// The message queue
pthread_mutex_t msgQueuMutex = PTHREAD_MUTEX_INITIALIZER;
vector<string> g_messageQueue;

// Conditional variable to indicate message in queue
pthread_cond_t g_cv;
pthread_mutex_t g_cvLock;

/* Messages are:

STEP  - Execute step of measurements and pump control
AUTO  - Go to automatic control
MANL  - Go to manual control
S_ON  - Force Solar pump on, ignored in AUTO
SOFF  - Force Solar pump off, ignored in AUTO
F_ON  - Force Filter pump on, ignored in AUTO
FOFF  - Force Filter pump off, ignored in AUTO

*/



int main(int argc, char *argv[])
{
   Comms comms;
   Controller cntrl;

   cout << "Welcome to THERMINAL pool solar catcher mamangement!" << endl;

   cntrl.initializeController();
   comms.initializeComms();

/*   
   struct timeval tv;
   
   long usStartTime;
   long usNow;
   long usPeriod = 3000000;
*/ 
   while(true)
   {
      
      pthread_mutex_lock(&g_cvLock);
      pthread_cond_wait(&g_cv, &g_cvLock);
      pthread_mutex_unlock(&g_cvLock);
      
      pthread_mutex_lock( &msgQueuMutex );
      
      while (g_messageQueue.size() > 0)
      {
         cntrl.executeCommand(g_messageQueue[0]);
         g_messageQueue.erase(g_messageQueue.begin());
      }
      /* 
      string cmd = g_messageQueue
      int l = g_messageQueue.size();
      cout << "In main loop, queue has: " << l << " entries." << endl;

      for (int i = 0; i < l; i++)
      {
         cout << "Message " << i << ": " << g_messageQueue[i] << endl;
      }
      */

      pthread_mutex_unlock( &msgQueuMutex );

/*      
      // Save time at start of this step
      gettimeofday(&tv, NULL); 
      usStartTime = tv.tv_sec * 1000000 + tv.tv_usec;
*/

/*      
      
      gettimeofday(&tv, NULL); 
      usNow = tv.tv_sec * 1000000 + tv.tv_usec;

      std::this_thread::sleep_for(
         std::chrono::microseconds(
            usPeriod - (usNow - usStartTime)));
*/            
   }

   return 0;
}
