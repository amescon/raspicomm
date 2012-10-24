#! /bin/sh

if [ ! -d "debian/usr" ]; then
  mkdir debian/usr
fi

if [ ! -d "debian/usr/sbin" ]; then
  mkdir debian/usr/sbin
fi

if [ ! -d "debian/etc" ]; then
  mkdir debian/etc
fi

if [ ! -d "debian/etc/init.d" ]; then
  mkdir debian/etc/init.d
fi

# get the new library file
cp ../../daemon/raspicommd debian/usr/sbin/raspicommd

# get the init.d script
cp ../../daemon/raspicommd.sh debian/etc/init.d/raspicommd.sh

# get the daemons configuration file
cp ../../daemon/raspicommd.conf debian/etc/raspicommd.conf

# create the package
dpkg -b debian raspicommd.deb
