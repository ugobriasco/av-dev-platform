#!/usr/bin/env sh


set -x
. ./sbin/config.sh

echo 'Build package'
rm -rf ./turtle-dist
mkdir ./turtle-dist
cp ./src/cpp/lib/motion.cpp ./turtle-dist/motion.cpp
cp ./src/cpp/lib/motion.h ./turtle-dist/motion.h
cp ./src/cpp/turtle-g/main.cpp ./turtle-dist/main.cpp

echo 'Make tar ball'
tar -czf turtle-package.tgz turtle-dist


echo 'Start deployment'
scp turtle-package.tgz $REMOTE_USER@$REMOTE_HOST:$REMOTE_APP_DIR && \
ssh $REMOTE_USER@$REMOTE_HOST 'bash -s' < ./sbin/turtle-g/pi-build.sh
echo 'Enjoy!';
