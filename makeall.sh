
# building raspicomm library & build the package & install the package
cd raspicomm
make
cd ..
cd package/raspicomm
./buildpackage.sh
dpkg -i libraspicomm.deb
cd ../..

# building display library
cd display
make
cd ..
cd package/display
./buildpackage.sh
dpkg -i libraspicommdisplay.deb
cd ../..

cd steprocker
make
cd ..
cd package/steprocker
./buildpackage.sh
dpkg -i libraspicommsteprocker.deb
cd ../..

cd daemon
make
cd ..
cd package/daemon
./buildpackage.sh
dpkg -i raspicommd.deb
cd ../..

cd demo
make
cd ..
cd package/demo
./buildpackage.sh
dpkg -i raspicommdemo.deb
cd ../..
