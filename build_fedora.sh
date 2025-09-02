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

# Build media-downloader
cd ~/media-downloader
rm -rf build
mkdir build
cd build

cmake ..
make -j"$(nproc)"

chmod +x media-downloader

