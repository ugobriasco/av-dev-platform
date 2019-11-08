#!/usr/bin/env sh


set -x
. ./sbin/config.sh

echo '🧠 Brain-build START'
rm -rf ./brain-dist
mkdir ./brain-dist
cp ./src/opencv/main.cpp ./brain-dist/main.cpp
cp ./src/opencv/rs232.c ./brain-dist/rs232.c
cp ./src/opencv/rs232.h ./brain-dist/rs232.h

mkdir ./brain-dist/utils
cp ./src/py/send-motion.py ./brain-dist/utils/send-motion.py

tar -czf brain-package.tgz brain-dist
echo '🧠 Brain-build DONE'

echo '🧠 Brain-deploy START'
scp brain-package.tgz $REMOTE_USER@$REMOTE_HOST:$REMOTE_APP_DIR && \
ssh $REMOTE_USER@$REMOTE_HOST 'bash -s' < ./sbin/brain/brain-build.sh
echo '🧠 Brain-deploy DONE';
