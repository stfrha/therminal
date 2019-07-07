echo off

echo "Building..."

C:\SysGCC\raspberry\bin\arm-linux-gnueabihf-g++.exe -ggdb main.cpp tempsensors.cpp ../pugixml/src/pugixml.cpp -o therminal -lwiringPi 
REM C:\SysGCC\raspberry\bin\arm-linux-gnueabihf-g++.exe -ggdb test1.cpp -o test1  

echo "Building complete"


