
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

#include "comms.h"
#include "controller.h"

using namespace std;


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
   comms.initializeComms(&cntrl);

/*   
   struct timeval tv;
   
   long usStartTime;
   long usNow;
   long usPeriod = 3000000;
*/ 
   while(true)
   {
      sleep(1);
      
   }

   return 0;
}
