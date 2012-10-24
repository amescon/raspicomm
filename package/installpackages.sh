#! /bin/sh

# install all packages in the correct order
dpkg -i libraspicomm.deb
dpkg -i libraspicommsteprocker.deb
dpkg -i libraspicommdisplay.deb
dpkg -i raspicommd.deb
dpkg -i raspicommdemo.deb