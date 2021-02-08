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

#### FAQ: How to download an audio file using options that are not available in the preset list.

1. If you want to download an audio file with best audio then enter ```bestaudio``` in the option field.
2. If you want to download an audio file with best audio and convert it to mp3 for example then enter ```bestaudio -x --audio-format mp3``` in the option fields.

The options fields can be used to enter the full options list to be sent to youtube-dl with only one requirenmnet of the format code being the first entry on the list.

For easy access, you can add your own custom entries like above in the preset list by going to configure tab and then add them to the ```preset list```. Options are comma separated and written in a format of: ```$UINAME($FORMAT_CODE $REST_OF_YOUTUBE-DL-OPTIONS)```

With the above two examples, add the following entries to make them show up in the preset option menu.

    BestAudio(bestaudio)
    BestAudio-mp3(bestaudio -x --audio-format mp3)

This app does not assume its downloading media file from youtube and youtube media files do not get preferential treatment, it works equally to all websites that are supported by youtube-dl.

## Binary packages
Binary download for Microsoft Windows is [here](https://github.com/mhogomchungu/media-downloader/releases/download/1.0.0/MediaDownloader-1.0.0.setup.exe).

Binary downloads for a few distributions are [here](https://software.opensuse.org//download.html?project=home%3Aobs_mhogomchungu&package=media-downloader).

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
