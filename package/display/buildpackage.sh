#! /bin/sh

if [ ! -d "debian/lib" ]; then

  # create the directory
  mkdir debian/lib

fi

# get the new library file
cp ../../display/libraspicommdisplay.so debian/lib/libraspicommdisplay.so

# create the package
dpkg -b debian libraspicommdisplay.deb
