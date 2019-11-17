#!/usr/bin/env sh


set -x
. ./sbin/config.sh

echo 'Build package'
rm -rf ./turtle-dist
mkdir ./turtle-dist
cp ./src/cpp/turtle-s/main.cpp ./turtle-dist/main.cpp
cp ./src/cpp/vendor/rs232.c ./turtle-dist/rs232.c
cp ./src/cpp/vendor/rs232.h ./turtle-dist/rs232.h

mkdir ./turtle-dist/utils
cp ./src/py/send-motion.py ./turtle-dist/utils/send-motion.py

echo 'Make tar ball'
tar -czf turtle-package.tgz turtle-dist


echo 'Start deployment'
scp turtle-package.tgz $REMOTE_USER@$REMOTE_HOST:$REMOTE_APP_DIR && \
ssh $REMOTE_USER@$REMOTE_HOST 'bash -s' < ./sbin/turtle-s/pi-build.sh
echo 'Enjoy!';
