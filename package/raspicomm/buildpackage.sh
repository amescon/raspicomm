#! /bin/sh

if [ ! -d "debian/lib" ]; then

  # create the directory
  mkdir debian/lib

fi

# get the new library file
cp ../../raspicomm/libraspicomm.so debian/lib/libraspicomm.so

# create the package
dpkg -b debian libraspicomm.deb
