#!/bin/bash
rm -rf ./*
# replace CMAKE_INSTALL_PREFIX to a directory in your machine , default directory  is  /projects/clparen
cmake -D CMAKE_INSTALL_PREFIX=/projects/clparen ..
make -j4
sudo make install 
sudo ldconfig
# change directory to where executable apps are located 	
cd /projects/clparen/bin/
./oclMGPUVR_Gui -v /projects/volume-datasets/foot/foot
# move back to build directory 	
cd /projects/parallel-rendering-multi-gpu/build	

#to make it executable :  chmod +x clparen.sh
#to run the script : ./clparen.sh or sh clparen.sh 
#don't save the script in build directory! , save it somewhere else
#make sure you are in the build directory when you run the script

