#!/usr/bin/env sh

echo 'Untar package'
set -x
cd /home/pi/Desktop
tar zxvf brain-package.tgz -C .
chmod +x ./brain-dist
rm brain-package.tgz
chmod +x ./brain-dist

echo 'Compile the brain'
cd ./brain-dist
g++ $(pkg-config opencv4 --cflags --libs) -o g++ $(pkg-config raspicam --cflags --libs) -o main main.cpp rs232.c
