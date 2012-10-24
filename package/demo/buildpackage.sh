#! /bin/sh

if [ ! -d "debian/usr" ]; then
  mkdir debian/usr
fi

if [ ! -d "debian/usr/sbin" ]; then
  mkdir debian/usr/sbin
fi

# get the new library file
cp ../../demo/raspicommdemo debian/usr/sbin/raspicommdemo

# create the package
dpkg -b debian raspicommdemo.deb
