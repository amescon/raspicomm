# RasPiComm #

This is the main readme file for the software of the raspicomm project.  
If you want to know what the raspicomm is, you may want to take a look at the offical [blog](https://amesberger.wordpress.com/).

## Getting Started ##

### Setup your distribution ###

*RaspiComm needs a distro that supports the i2c and the spi bus.*
e.g. 
[Adafruit Occidentalis v0.2](http://learn.adafruit.com/adafruit-raspberry-pi-educational-linux-distro/occidentalis-v0-dot-2)

#### Distributions ####
- Adafruit Occidentalis ([download the image](http://adafruit-raspberry-pi.s3.amazonaws.com/Occidentalisv02.zip))  
- Raspbian v3.2.27+ ([download the image](http://downloads.raspberrypi.org/images/raspbian/2012-12-16-wheezy-raspbian/2012-12-16-wheezy-raspbian.zip))  
Additional Steps are required, see below  

1. Download the Image [here](http://adafruit-raspberry-pi.s3.amazonaws.com/Occidentalisv02.zip)  
2. Write it to the SD card  
   You can find further instructions for setting up your SD-Card [here](http://elinux.org/RPi_Easy_SD_Card_Setup).  
3. Boot from the SD card  
4. Configure your distribution on first boot

#### Additional steps required for Raspbian ####

1. Check that you're running the Kernel Version 3.2.27+:  
   ``uname -r``
2. Install the i2c-tools:  
   sudo apt-get install i2c-tools
3. Edit /etc/modules and the following modules:  
   - i2c-bcm2708
   - i2c-dev
4. Load the modules:  
   ``sudo modprobe i2c-dev``  
   ``sudo modprobe i2c-bcm2708``  

*Thanks to Forum member CoolWater for the instructions!*

### Setup your raspicomm ###

Download the raspicomm source code, compile it, package it, install the packages with the following commands:  

``git clone https://github.com/amescon/raspicomm.git``  
``cd raspicomm``  
``sudo ./makeall.sh``

#### Selecting the raspberry pi revision ####
When installing the raspicomm api, you are asked to specify the raspberry pi revision you are using, by entering '1' or '2'.

If you're not sure which version you've got, take a look at the following pictures.
e.g. Revision 2 sports a mounting hole in the top right corner, right below the leds.

Picture of *Revision 1* from wikimedia.org:  
<img width="320px" src="http://upload.wikimedia.org/wikipedia/commons/9/90/Front_of_Raspberry_Pi.jpg" />  
Click [here](http://upload.wikimedia.org/wikipedia/commons/9/90/Front_of_Raspberry_Pi.jpg) to open the image.

Picture of *Revision 2* from raspberrypi.org:  
<img width="320px" src="http://www.raspberrypi.org/wp-content/uploads/2012/09/sony-rasp-pi-1024x728.jpg" />  
Click [here](http://www.raspberrypi.org/wp-content/uploads/2012/09/sony-rasp-pi-1024x728.jpg) to open the image.

### Test your installation ###
 - Verify if your real time clock is working by invoking:  
   ``sudo hwclock -r``  

 - Use the raspicommdemo tool  
   e.g. use ``sudo raspicommdemo --setoutputs`` to set the outputs. Both LEDs red & green should light up.  
   ``sudo raspicommdemo --unsetoutputs`` disables the outputs again.

 - You may want to take a look at the other commandline options of the raspicommdemo.

### Further information ###

For further information you can

- checkout the demo source code ('demo/demo.c') or from the github page or
- read the Api Overview below.

## RasPiComm API Overview ##

The RasPiComm sourcecode contains the following folders:

- raspicomm - the raspicomm api
- daemon - raspicomm ethernet daemon
- steprocker - library that implements the tmcl (=trinamic motion control language) as a sample implementation of a rs485 device.
- display - library for using an lcd display as a sample i2c device.
- demo - a sample/demo application that uses the forementioned libraries.
- package - folder that holds scripts to create .deb packages.

*Attention: Executing the raspicomm api requires superuser privileges!*

## raspicomm - main library ##

Output: libraspicomm.so  

The 'raspicomm' folder contains the sourcecode needed to build the main raspicomm api as a linux library.

For sample code, checkout the source for the demo application.

## demo - a sample raspicomm application##

Depends on: libraspicomm.so, libraspicommsteprocker.so, libraspicommdisplay.so  
Output: raspicommdemo  

A demo executable that uses the raspicomm lib, steprocker lib and display lib from a cmdline interface.

Features:

- Setting the outputs
- Getting the outputs
- Clearing the outputs
- Getting the state of the buttons
- Moving the steprocker motor
- Reading motor parameters
- Writing to the lcd-display

Invoke it like:
`sudo raspicommdemo --setoutput`

## daemon - raspicomm ethernet daemon ##

Output: raspicommd  
Depends on: libraspicomm.so  

The 'daemon' folder contains the sourcecode for the raspicomm ethernet daemon. The ethernet daemon allows two-way communication from the spi port to a socket, which is useful if you want to remotely control the spi port.

Upon startup, it opens a port and listens for a connection. The default port is 3333.

Start the daemon by calling: `sudo /etc/init.d/raspicommd.sh start`

You can configure the port that the daemon uses by editing /etc/raspicommd.conf and restarting the daemon.
For example:  
``sudo vi /etc/raspicommd.conf``  
``..edit..``  
``sudo /etc/init.d/raspicommd.sh restart``

The android based raspicomm steprocker app uses the ethernet daemon to remotely control the steprocker.

## steprocker - tmcl library ##

Output: libraspicommsteprocker.so  
Depends on: libraspicomm.so  

The 'steprocker' folder contains the sourcecode needed to build steprocker library, a simple c-library that implements the tmcl protocol.

## display - i2c lcd display library ##

Output: libraspicommdisplay.so  
Depends on: libraspicomm.so  

The 'display' folder contains the sourcecode for a library that accesses a lcd display using the i2c bus.
It serves as a sample for implementation of accessing an i2c device.

## package - creates .deb packages ##

Outputs: libraspicomm.deb, libraspicommsteprocker.deb, libraspicommdisplay.deb, raspicommd.deb, raspicommdemo.deb

The 'package' folder contains shell scripts in its subfolders to create debian packages for the libraries and and the executables. Executing `sudo ./buildpackage.sh` creates the debian package.

Install it by using `sudo dpkg -i [name].deb`
and remove it again with `sudo dpkg -r [name]` 

Besides deploying the libraries into /lib and the executables into /usr/sbin the packages also deploy additional files. e.g. raspicommd.deb deploys a script `/etc/init.d/raspicommd.sh` and a configuration file `/etc/raspicommd.conf`.
