# media-downloader

This project is a Qt/C++ based frontend to youtube-dl and it can be
used to download any media file supported by youtube-dl.

Features offered:-

1. The GUI can be used to download any media supported by youtube-dl.

2. The GUI can show all available media a url can offer.

3. The GUI allows user to choose what media entry offered by a url they want to download.

4. The GUI offers a preset list of options to choose from to select what media entry offered by a url they want to download.

5. The GUI offers an ability to change preset options or add new ones making it possible to use the GUI to automate any use case supported by youtube-dl.

6. The GUI offers an ability to download single media or multiple medias in batches.

7. The GUI has a json based plugin system and can configured to download media using other tools other than youtube-dl.

## Donations
Donations are very much appreciated, Please contact me using mhogomchungu _at__ gmail _dot__ com for further information.

## Extensions

Read the [extensions](https://github.com/mhogomchungu/media-downloader/wiki/Extensions) page for further information.

## FAQ
A Frequently asked Questions' page is [here](https://github.com/mhogomchungu/media-downloader/wiki/Frequently-Asked-Questions).

## Binary packages

#### Prerequisite for Microsoft Windows

1. Youtube-dl for Windows requires [Microsoft Visual C++ 2010 Service Pack 1 Redistributable Package (x86)](https://download.microsoft.com/download/1/6/5/165255E7-1014-4D0A-B094-B6A430A6BFFC/vcredist_x86.exe) to be installed and install it first before continuing.

2. Make sure you have access to the internet before you run media-downloader for the first time because it will attempt to use the internet to download the latest version of youtube-dl.


#### Installer for Microsoft Windows

Installer for Microsoft Windows is [here](https://github.com/mhogomchungu/media-downloader/releases/download/1.3.0/MediaDownloader-1.3.0.setup.exe).

#### Portable version for Microsoft Windows.

A portable version is a self contained version that keeps everything in the application folder.

Portable version for Microsoft Windows is [here](https://github.com/mhogomchungu/media-downloader/releases/download/1.3.0/MediaDownloader-1.3.0.zip).

### Binary packages for Linux distributions

Binary downloads for a few Linux distributions are [here](https://software.opensuse.org//download.html?project=home%3Aobs_mhogomchungu&package=media-downloader).

#### Aur package for Arch Linux
Arch linux users can build the project from source using [this](https://aur.archlinux.org/packages/media-downloader) aur package.

#### Binary package for Ubuntu
To install a binary package in Ubuntu, download a binary package directly and then click it and Ubuntu should offer to install it.
If unsuccessful, follow instructions below for debian.

#### Binary package for Debian
To install a binary package for Debian, download a binary package directly and then install it by doing the following:-
from the folder where you dowload the package.
```
1. Make sure you have Qt libraries installed.
2. Open the terminal.
3. Change directories to where the debian package is.
4. Run the following command to install it: su -c "dpkg -i *.deb"
5. Enter root's password when asked.
```
#### Binary package for Fedora and OpenSuse
To install a binary package for Fedora and OpenSuse, download a binary pacakge directly and then install it by doing the follwing:-
```
1. Make sure you have Qt libraries installed.
2. Open the terminal.
3. Change directories to where the debian package is.
4. Run the following command to install it: sudo rpm –i *.rpm
5. Enter root's password when asked.
```
## Source packages
To build from source, follow the following instructions
```
1. Make sure you have the following packages installed:-
    - Development version of Qt libraries. They will be named
      something like "libqt5-qtbase-devel" or "qt5-devel" or
      "qtbase5-dev" or something else that sounds similar.
    - cmake
    - gcc
2. Dowload the source archive and extract it.
3. Open the terminal and change directories to inside extracted folder.
4. Run command: "mkdir build"
5. Run command: "cd build"
6. Run command: "cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=release .."
7. Run command: "make"
8. Fun command: "sudo make install"

```

# Screenshots

![media-downloader.png](https://raw.githubusercontent.com/mhogomchungu/media-downloader/main/images/media-downloader.png)

![media-downloader.png](https://raw.githubusercontent.com/mhogomchungu/media-downloader/main/images/media-downloader-1.png)

![media-downloader.png](https://raw.githubusercontent.com/mhogomchungu/media-downloader/main/images/media-downloader-2.png)

![media-downloader.png](https://raw.githubusercontent.com/mhogomchungu/media-downloader/main/images/media-downloader-3.png)
