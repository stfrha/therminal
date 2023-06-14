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

Add how to autostart the app