# Installing opencv4 on a RPI

## Preparation

Cleanup the Raspian image

```
sudo apt-get purge wolfram-engine
sudo apt-get purge libreoffice*
sudo apt-get clean
sudo apt-get autoremove
```

Check space available

```
sudo raspi-config
sudo reboot
df -h
```

Update OS
```
sudo apt-get upgrade
sudo apt-get update
```

Increase swap rate
```
sudo nano /etc/dphys-swapfile
## set this value CONF_SWAPSIZE=2048
sudo /etc/init.d/dphys-swapfile stop
sudo /etc/init.d/dphys-swapfile start
```

## Installation

```
sudo apt-get install build-essential cmake pkg-config
sudo apt-get install libjpeg-dev libtiff5-dev libjasper-dev libpng12-dev
sudo apt-get install libavcodec-dev libavformat-dev libswscale-dev libv4l-dev
sudo apt-get install libxvidcore-dev libx264-dev
sudo apt-get install cmake git libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev
sudo apt-get install python-dev python-numpy libtbb2 libtbb-dev libjpeg-dev libpng-dev libtiff-dev libjasper-dev libdc1394-22-dev
```


GUI of open CV, gtk package, and matrix function
```
sudo apt-get install libgtk2.0-dev libgtk-3-dev
sudo apt-get install libatlas-base-dev gfortran
```                           

## Preparing working directory

```
mkdir opencv
cd ~/opencv
git clone https://github.com/opencv/opencv.git
git clone https://github.com/opencv/opencv_contrib.git
```

## Preparing build directory

```
mkdir build
cd build
cmake -D CMAKE_BUILD_TYPE=Release -DCMAKE_EXTRA_MODULES_PATH=../opencv_contrib/modules/ -D CMAKE_INSTALL_PREFIX=/usr/local ../opencv -DBUILD_opencv_java=OFF -DBUILD_opencv_python2=OFF -DBUILD_opencv_python3=OFF
make -j4
sudo make install
sudo ldconfig
```



<---- YOU ARE HERE



sudo nano /etc/dphys-swapfile
//////////set this value CONF_SWAPSIZE=100

 sudo /etc/init.d/dphys-swapfile stop
 sudo /etc/init.d/dphys-swapfile start



sudo nano /etc/ld.so.conf.d/opencv.conf

/usr/local/lib                        //  add  this path

sudo ldconfig


sudo nano /etc/bash.bashrc

PKG_CONFIG PATH=$PKG_CONFIG_PATH:/usr/local/lib/pkgconfig               // add this path

export PKG_CONFIG_PATH





geany open cv path for build
g++ $(pkg-config opencv4 --cflags --libs) -o %e %f





geany open cv path for build
g++ $(pkg-config opencv4 --cflags --libs) -o %e %f





installing raspicam

git clone https://github.com/cedricve/raspicam.git
cd raspicam
mkdir build
cd build
cmake ..
make
sudo make install
sudo ldconfig


g++ $(pkg-config raspicam --cflags --libs) -o %e %f






# Package Information for pkg-config

prefix=/usr/local
exec_prefix=${prefix}
libdir=${exec_prefix}/lib
includedir_old=${prefix}/include/raspicam
includedir_new=${prefix}/include/raspicam

Name: raspicam
Description: Open Source Computer Vision Library
Version: 2.0.0
Libs: -L${exec_prefix}/lib -lraspicam -lraspicam_cv
Libs.private: -ldl -lm -lpthread -lrt
Cflags: -I${includedir_old} -I${includedir_new}



Updated Geany Path

g++ $(pkg-config opencv4 --cflags --libs) -o g++ $(pkg-config raspicam --cflags --libs) -o %e %f
