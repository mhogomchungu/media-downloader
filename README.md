# Media Downloader

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

7. The GUI is offered in multiple languages and as of this writing, the supported languages are English, Chinese, Spanish, Polish, Turkish, Russian, Japanese, French, Italian, Portuguese, Arabic, Korean, Swedish, German, Greek and Ukrainian.

## Extensions

Media Downloader is a GUI front end to [yt-dlp](https://github.com/yt-dlp/yt-dlp), [gallery-dl](https://github.com/mikf/gallery-dl), [lux](https://github.com/iawia002/lux),
[you-get](https://github.com/soimort/you-get), [svtplay-dl](https://github.com/spaam/svtplay-dl), [aria2c](https://aria2.github.io/),[wget](https://www.gnu.org/software/wget) and [get-sauce](https://github.com/gan-of-culture/get-sauce).

To Install these extensins, go to "Configure" tab and then go to "Extensions" sub tab and then click the "Add An Extension" button and then select from the list an extension you want to install.

## FAQ
A Frequently asked Questions page is [here](https://github.com/mhogomchungu/media-downloader/wiki/Frequently-Asked-Questions).

#### Prerequisite before running for the first time

Make sure you have access to the internet before you run "Media Downloader" for the first time because it will attempt to use the internet to download the latest version of yt-dlp. Installing most extensions will also cause "Media Downloader" to access the internet to download the extension's executable.

## Binary packages

#### Bundle for MacOS

Bundle for MacOS is  [here](https://github.com/mhogomchungu/media-downloader/releases/download/5.4.6/MediaDownloaderQt6-5.4.6.dmg). This bundle is not notarized and your system may report it as "corrupted". Search the internet on how to install bundles that are not notarized if you want to use this app on MacOS. This bundle works on macOS 14.0 or later.

#### Installer for Microsoft Windows

Installer for Microsoft Windows that is 32 bit, build with Qt5 and has a minimum requirenment of windows 7 is [here](https://github.com/mhogomchungu/media-downloader/releases/download/5.4.6/MediaDownloaderQt5-5.4.6.setup.exe).

Installer for Microsoft Windows that is 64 bit, build with Qt6 and has a minimum requirenment of windows 10 is [here](https://github.com/mhogomchungu/media-downloader/releases/download/5.4.6/MediaDownloaderQt6-5.4.6.setup.exe).

#### Portable version for Microsoft Windows

A portable version is a self-contained version that keeps everything in the application folder and does not need to be installed first.

Portable version for Microsoft Windows that is 32 bit, build with Qt5 and has a minimum requirenment of windows 7 is [here](https://github.com/mhogomchungu/media-downloader/releases/download/5.4.6/MediaDownloaderQt5-5.4.6.zip).

Portable version for Microsoft Windows that is 64 bit, build with Qt6 and has a minimum requirenment of windows 10 is [here](https://github.com/mhogomchungu/media-downloader/releases/download/5.4.6/MediaDownloaderQt6-5.4.6.zip).

You can also install the portable version for Windows using scoop with the following commands:

Add the extras bucket:
```powershell
scoop bucket add extras
```
Install Media Downloader:
```powershell
scoop install media-downloader
```

Git versions for windows and macos can be downloaded from [here](https://github.com/mhogomchungu/media-downloader-git/releases).

#### Problems with Windows's antivirus programs

Once in a while, Windows Defender and other antivirus tools will report this application as a virus/unsafe
or Potentially unwanted. These are false positive reports and they are tracked [here](https://github.com/mhogomchungu/media-downloader/issues/481).


#### Flatpak

Media Downloader is on [flathub](https://flathub.org/apps/io.github.mhogomchungu.media-downloader) for those who prefer to use flatpaks.

#### Aur package for Arch Linux
Arch Linux users can build the project from source using [this](https://aur.archlinux.org/packages/media-downloader) aur package.

#### Package for Fedora
Media Downloader is in official Fedora repositories and can be installed by running ```sudo dnf -y install media-downloader```

### Binary packages for other Linux distributions

Binary packages i maintain for a few Linux distributions are [here](https://software.opensuse.org//download.html?project=home%3Aobs_mhogomchungu&package=media-downloader).

### Packaging Status

A short list of distributions that have Media Downloader in their repositories and the version they have is maintained [here](https://repology.org/project/media-downloader/badges).


# How to compile for Linux

1. clone the repo and cd into it
```console
git clone https://github.com/mhogomchungu/media-downloader && cd media-downloader
```

2. chmod it to make it a executeable
```console
chmod +x build_linux.sh
```

3. now run the shell script
```console
./build_linux.sh
```


### Fedora

Fedora users can use the following script to build from source:

```console
./build_fedora.sh
```
### Arch linux

Arch linux users can use the following script to build from source:

```console
./build_arch.sh
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
