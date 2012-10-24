#! /bin/sh

# create the directory if it does not exist yet
if [ ! -d "debian/lib" ]; then
  mkdir debian/lib
fi

# get the new library file
cp ../../steprocker/libraspicommsteprocker.so debian/lib/libraspicommsteprocker.so

# create the package
dpkg -b debian libraspicommsteprocker.deb
