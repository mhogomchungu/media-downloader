# media-downloader

This project is a Qt/C++ based frontend to youtube-dl and it can be
used to download any media file supported by youtube-dl

Features offered:-

1. The GUI can be used to download any media supported by youtube-dl.

2. The GUI can show all available media a url can offer.

3. The GUI allows user to choose what media entry offered by a url they want to download.

4. The GUI offers a preset list of options to choose from to select what media entry offered by a url they want to download.

5. The GUI offers an ability to change preset options or add new ones making it possible to use the GUI to automate any use case supported by youtube-dl.

6. The GUI offers an ability to download single media or multiple medias in batches.

## Binary packages
Binary download for Microsoft Windows is [here.](https://github.com/mhogomchungu/media-downloader/releases/download/1.0.0/MediaDownloader-1.0.0.setup.exe)

Binary downloads for a few distributions are [here.](https://software.opensuse.org//download.html?project=home%3Aobs_mhogomchungu&package=media-downloader)

To install a binary package in Ubuntu, download a binary package directly and then click it and Ubuntu should offer to install it.

To install a binary package for Debian, download a binary package directly and then install it by doing the following:-
from the folder where you dowload the package.
```
0. Make sure you have Qt libraries installed.
1. Open the terminal.
2. Change directories to where the debian packages are.
3. Run the following command to install them: su -c "dpkg -i *.deb"
4. Enter root's password when asked.
```

To install a binary package for Fedora and OpenSuse, download a binary pacakge directly and then install it by doing the follwing:-
```
0. Make sure you have Qt libraries installed.
1. Open the terminal.
2. Change directories to where the debian packages are.
3. Run the following command to install them: sudo rpm –i *..rpm
4. Enter root's password when asked.
```
## Source packages
To build from source, follow the following instructions
```
0. Make sure you have the following packages installed:-
    - Development version of Qt libraries installed. They will be named something like like
      "libqt5-qtbase-devel" or "qt5-devel" or "qtbase5-dev"
    - cmake
    - gcc
1. Dowload the source archive and extract it.
2. Open the terminal and change directories to inside extracted folder.
3. Run command: "mkdir build"
4. Run command: "cd build"
5. Run command: "cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=release .."
6. Run command: "make"
7. Fun commandl: "sudo make install"

```

# Screenshots

![media-downloader.png](https://raw.githubusercontent.com/mhogomchungu/media-downloader/main/images/media-downloader.png)

![media-downloader.png](https://raw.githubusercontent.com/mhogomchungu/media-downloader/main/images/media-downloader-1.png)

![media-downloader.png](https://raw.githubusercontent.com/mhogomchungu/media-downloader/main/images/media-downloader-2.png)

![media-downloader.png](https://raw.githubusercontent.com/mhogomchungu/media-downloader/main/images/media-downloader-3.png)
