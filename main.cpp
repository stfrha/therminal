
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

// Conditional variable to owns the mainIsIt flag 
pthread_cond_t g_cv;
pthread_mutex_t g_cvLock;

// Conditional variable to signal command finished
pthread_cond_t g_cvFinished;
pthread_mutex_t g_cvFinishedLock;

// Shows who owns execution, Comms or Main
bool mainIsIt = false;

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
      cout << "MAIN: Waiting for mutex..." << endl;

      // Get the mutex here. It will be released at pthread_cond_wait below
      // and then return to main when Comms sends the signal. 
      int r = pthread_mutex_lock(&g_cvLock);
      cout << "MAIN: CV mutex received, r = " << r << endl;
      
      
      if (mainIsIt)
      {
         // My turn

         cout << "MAIN: Executing command: " << g_messageQueue[0] << endl;
         cntrl.executeCommand(g_messageQueue[0]);
         g_messageQueue.erase(g_messageQueue.begin());

         pthread_mutex_unlock( &msgQueuMutex );
         
         cout << "MAIN: Signal that command is done." << endl;

         // Signal Comms that the command is executed and
         // the status is updates.
         int r = pthread_mutex_lock(&g_cvFinishedLock);
         cout << "COMMS: CV Finished mutex received, r = " << r << endl;
         mainIsIt = false;
         pthread_cond_signal(&g_cvFinished);
         pthread_mutex_unlock(&g_cvFinishedLock);
      }
      else
      {
         cout << "MAIN: Waiting for signal..." << endl;

         pthread_cond_wait(&g_cv, &g_cvLock);

         // When main was signaled (after wait above)
         // it was given the mutex. Now Comms needs it 
         // to start its wait, so we unlock it here
         pthread_mutex_unlock(&g_cvLock);

         cout << "MAIN: Signal received and mutex released." << endl;
      }
   }

   return 0;
}
