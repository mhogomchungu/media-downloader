#!/bin/bash
set -e

sudo apt update
sudo apt install -y build-essential cmake git
sudo apt install -y qtbase5-dev qtdeclarative5-dev \
    qml-module-qtquick-controls2 qml-module-qtwebsockets \
    qml-module-qtwebchannel qttools5-dev qttools5-dev-tools

cd ~/media-downloader
rm -rf build
mkdir build
cd build

cmake ..
make -j"$(nproc)"

chmod +x media-downloader
./media-downloader