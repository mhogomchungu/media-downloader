#!/bin/bash
set -e

sudo apt update
sudo apt install -y build-essential cmake git
sudo apt install -y qtbase5-dev qtdeclarative5-dev \
    qml-module-qtquick-controls2 qml-module-qtwebsockets \
    qml-module-qtwebchannel qttools5-dev qttools5-dev-tools

SCRIPT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &>/dev/null && pwd)

cd $SCRIPT_DIR
rm -rf build
mkdir build
cd build

cmake -DCMAKE_INSTALL_PREFIX=/usr/local -DCMAKE_BUILD_TYPE=RELEASE ..
make -j"$(nproc)"

chmod +x media-downloader
./media-downloader
