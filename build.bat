echo off

echo "Building..."

C:\SysGCC\raspberry\bin\arm-linux-gnueabihf-g++.exe -ggdb main.cpp comms.cpp tempsensors.cpp relaycontrol.cpp logger.cpp controller.cpp ../pugixml/src/pugixml.cpp -o therminal -lwiringPi  -lpthread
REM C:\SysGCC\raspberry\bin\arm-linux-gnueabihf-g++.exe -ggdb test1.cpp -o test1  

echo "Building complete"


