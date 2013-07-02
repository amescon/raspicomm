

_____
### - OBSOLETE! - For reference purposes only! -  ###

This is the outdated API for the raspicomm.  
For up-to-date information please refer to our [getting started](http://www.amescon.com/products/getting-started.aspx) page. 

_____ 


  

  
  

## RasPiComm API Overview ##

The RasPiComm sourcecode contains the following folders:

- raspicomm - the raspicomm api
- daemon - raspicomm ethernet daemon
- steprocker - library that implements the tmcl (=trinamic motion control language) as a sample implementation of a rs485 device.
- display - library for using an lcd display as a sample i2c device.
- demo - a sample/demo application that uses the forementioned libraries.
- package - folder that holds scripts to create .deb packages.

*Attention: Executing the raspicomm api requires superuser privileges!*

### RS-232 Support ###

There is no api support for the RS-232 port, as you can use standard linux libraries for accessing the serial port. The device name is '/dev/ttyAMA0'.

For example:  
``int fd = open("/dev/ttyAMA0", O_RDWR | O_NOCTTY | O_NDELAY);``  
``...``  

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
