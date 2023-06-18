Need the following repos to build:

git@github.com:stfrha/WiringPi.git
  - Run ./build to build
git@github.com:stfrha/pugixml.git
  
Install apache2 webserver and php


sudo apt-get install apache2 -y
sudo apt-get install php libapache2-mod-php -y

Run the following command lines:
sudo cp -a /home/pi/git/therminal/php/. /var/www/html/
sudo rm /var/www/html/index.html

Install temp sensors
sudo raspi-config
Enable support for OneWire, Interface Options/OneWire

sudo nano /boot/config.txt

Next At the bottom of this file enter the following.
dtoverlay=w1-gpio

sudo reboot

sudo modprobe w1-gpio
sudo modprobe w1-therm

Add the following line at the bottom of the /etc/rc.local file, but before the "exit 0" statement


su - pi -c "/home/pi/git/therminal/startupscript.sh &"


Starting the app for the first time, the SW needs to differentiate between the two sensors. It ask to raise the temp on the pool sensor (by holding it in the hand). 

