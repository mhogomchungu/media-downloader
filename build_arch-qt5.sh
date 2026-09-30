#!/bin/bash
set -e

sudo pacman -Syu --needed --noconfirm --noprogressbar base-devel cmake git
sudo pacman -Syu --needed --noconfirm --noprogressbar qt5-base qt5-declarative \
    qt5-quickcontrols2 qt5-websockets qt5-webchannel qt5-tools

SCRIPT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &>/dev/null && pwd)

cd $SCRIPT_DIR

rm -rf build
mkdir build
cd build

cmake -DCMAKE_INSTALL_PREFIX=/usr/local -DCMAKE_BUILD_TYPE=RELEASE ..
make -j"$(nproc)"

chmod +x media-downloader
