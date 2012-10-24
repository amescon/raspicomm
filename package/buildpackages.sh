#! /bin/sh

# build all packages
cd raspicomm
. ./buildpackage.sh
cd ..

cd daemon
. ./buildpackage.sh
cd ..

cd steprocker
. ./buildpackage.sh
cd ..

cd display
. ./buildpackage.sh
cd ..

cd demo
. ./buildpackage.sh
cd ..

# copy them to the base directory
cp raspicomm/libraspicomm.deb ./libraspicomm.deb
cp steprocker/libraspicommsteprocker.deb ./libraspicommsteprocker.deb
cp display/libraspicommdisplay.deb ./libraspicommdisplay.deb
cp daemon/raspicommd.deb ./raspicommd.deb
cp demo/raspicommdemo.deb ./raspicommdemo.deb
