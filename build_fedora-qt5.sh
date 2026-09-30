#!/bin/bash
set -e

# Update system
sudo dnf -y update

# Install essential build tools individually (instead of group)
sudo dnf -y install gcc gcc-c++ make automake autoconf libtool \
    cmake git

# Install Qt5 development packages
sudo dnf -y install qt5-qtbase-devel qt5-qtdeclarative-devel \
    qt5-qtquickcontrols2-devel qt5-qtwebsockets-devel \
    qt5-qtwebchannel-devel qt5-qttools-devel

SCRIPT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &>/dev/null && pwd)

cd $SCRIPT_DIR

rm -rf build
mkdir build
cd build

cmake -DCMAKE_INSTALL_PREFIX=/usr/local -DCMAKE_BUILD_TYPE=RELEASE ..
make -j"$(nproc)"

chmod +x media-downloader
