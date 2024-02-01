# media-downloader

This project is a Qt/C++ based GUI frontend to CLI multiple CLI-based tools that deal with downloading online media.

[yt-dlp](https://github.com/yt-dlp/yt-dlp) CLI tool is the default supported tool and other tools can be added by
downloading their extension and a list of supported extensions is managed [here](https://github.com/mhogomchungu/media-downloader/wiki/Extensions).


Features offered:-

1. The GUI can be used to download any media from any website supported by installed extensions.

2. The GUI offers a configurable list of preset options that can be used to download media if they are provided in multiple formats.

3. The GUI offers the ability to do an unlimited number of concurrent downloads. Be careful with this ability because doing too many concurrent
downloads may cause the host to ban you.

4. The GUI offers the ability to do batch downloads by entering individual links in the UI or telling the app to read them from a local file.

5. The GUI offers an ability to download a playlist from websites that supports them like YouTube.

6. The GUI offers the ability to manage links to playlists to easily monitor their activities(subscriptions).

7. The GUI is offered in multiple languages and as of this writing, the supported languages are English, Chinese, Spanish, Polish,
Turkish, Russian, Japanese, French, and Italian.

## Donations
Donations are very much appreciated, Please contact me using mhogomchungu _at__ gmail _dot__ com for further information.

## Extensions

Media Downloader is a GUI front end to [yt-dlp](https://github.com/yt-dlp/yt-dlp), [youtube-dl](https://github.com/ytdl-org/youtube-dl/), [gallery-dl](https://github.com/mikf/gallery-dl), [lux](https://github.com/iawia002/lux),
[you-get](https://github.com/soimort/you-get), [svtplay-dl](https://github.com/spaam/svtplay-dl), [aria2c](https://aria2.github.io/), [wget](https://www.gnu.org/software/wget) and [safari books](https://github.com/mhogomchungu/safaribooks).

Read the [extensions](https://github.com/mhogomchungu/media-downloader/wiki/Extensions) page for further information on these programs and how to install them.

## FAQ
A Frequently asked Questions page is [here](https://github.com/mhogomchungu/media-downloader/wiki/Frequently-Asked-Questions).

#### Prerequisite before running for the first time

Make sure you have access to the internet before you run the media-downloader for the first time because it will attempt to use the internet to download the latest version of yt-dlp. Installing most extensions will also cause the media-downloader to access the internet to download the extension's executable.

## Binary packages

#### Installer for Microsoft Windows

Installer for Microsoft Windows is [here](https://github.com/mhogomchungu/media-downloader/releases/download/4.3.0/MediaDownloader-4.3.0.setup.exe).

#### Portable version for Microsoft Windows

A portable version is a self-contained version that keeps everything in the application folder.

Portable version for Microsoft Windows is [here](https://github.com/mhogomchungu/media-downloader/releases/download/4.3.0/MediaDownloader-4.3.0.zip).

Git version of portable version for Microsoft Windows is [here](https://github.com/mhogomchungu/media-downloader-git/releases).

#### Aur package for Arch Linux
Arch Linux users can build the project from source using [this](https://aur.archlinux.org/packages/media-downloader) aur package.

#### Package for Fedora
Media Downloader is in official Fedora repositories and can be installed by running ```sudo dnf -y install media-downloader```

### Binary packages for other Linux distributions

Binary downloads for a few Linux distributions are [here](https://software.opensuse.org//download.html?project=home%3Aobs_mhogomchungu&package=media-downloader).

#### Binary package for Ubuntu
To install a binary package in Ubuntu, download a binary package directly and then click it and Ubuntu should offer to install it.
If unsuccessful, follow the instructions below for Debian.

#### Binary package for Debian
To install a binary package for Debian, download a binary package directly and then install it by doing the following:-
from the folder where you download the package.
```
1. Make sure you have Qt libraries installed.
2. Open the terminal.
3. Change directories to where the debian package is.
4. Run the following command to install it: su -c "dpkg -i *.deb"
5. Enter root's password when asked.
```
#### Binary package for Fedora and OpenSuse
To install a binary package for Fedora and OpenSuse, download a binary package directly and then install it by doing the following:-
```
1. Make sure you have Qt libraries installed.
2. Open the terminal.
3. Change directories to where the debian package is.
4. Run the following command to install it: sudo rpm –i *.rpm
5. Enter root's password when asked.
```
## Source packages
To build from the source, follow the following instructions
```
1. Make sure you have the following packages installed:-
    - Development version of Qt libraries. They will be named
      something like "libqt5-qtbase-devel" or "qt5-devel" or
      "qtbase5-dev" or something else that sounds similar.
    - cmake
    - gcc
2. Download the source archive and extract it.
3. Open the terminal and change directories to inside the extracted folder.
4. Run command: "mkdir build"
5. Run command: "cd build"
6. Run command: "cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=release .."
7. Run command: "make"
8. Fun command: "sudo make install"

```

# Screenshots


![media-downloader.png](https://raw.githubusercontent.com/mhogomchungu/media-downloader/main/images/media-downloader-1.png)

![media-downloader.png](https://raw.githubusercontent.com/mhogomchungu/media-downloader/main/images/media-downloader-2.png)

![media-downloader.png](https://raw.githubusercontent.com/mhogomchungu/media-downloader/main/images/media-downloader-3.png)

![media-downloader.png](https://raw.githubusercontent.com/mhogomchungu/media-downloader/main/images/media-downloader-4.png)

![media-downloader.png](https://raw.githubusercontent.com/mhogomchungu/media-downloader/main/images/media-downloader-5.png)

![media-downloader.png](https://raw.githubusercontent.com/mhogomchungu/media-downloader/main/images/media-downloader-6.png)

![media-downloader.png](https://raw.githubusercontent.com/mhogomchungu/media-downloader/main/images/media-downloader-7.png)

# Disclaimer

This program is intended to be used  in a way that does not violate any laws that are applicable to its users.

# License

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
