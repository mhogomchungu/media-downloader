#!/bin/bash
set -e

sudo pacman -Syu --needed --noconfirm --noprogressbar base-devel cmake git
sudo pacman -Syu --needed --noconfirm --noprogressbar qt5-base qt5-declarative \
    qt5-quickcontrols2 qt5-websockets qt5-webchannel qt5-tools

cd ~/media-downloader
rm -rf build
mkdir build
cd build

cmake ..
make -j"$(nproc)"

chmod +x media-downloader
