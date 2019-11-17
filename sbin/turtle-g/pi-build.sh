#!/usr/bin/env sh

echo 'Untar package'
set -x
cd /home/pi/Desktop
tar zxvf turtle-package.tgz -C .
chmod +x ./turtle-dist
rm turtle-package.tgz
chmod +x ./turtle-dist

echo 'Compile the turtle'
cd ./turtle-dist
g++ $(pkg-config opencv4 --cflags --libs) -o g++ $(pkg-config raspicam --cflags --libs) -o g++ $(pkg-config wiringpi --cflags --libs) -o main main.cpp motion.cpp
