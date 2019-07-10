echo off

echo "Building..."

C:\SysGCC\raspberry\bin\arm-linux-gnueabihf-g++.exe -ggdb server.c -o server
REM C:\SysGCC\raspberry\bin\arm-linux-gnueabihf-g++.exe -ggdb test1.cpp -o test1  

echo "Building complete"


