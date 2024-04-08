

$QT_PATH="C:\Qt\5.15.2\mingw81_32"

$QT_PATH_BIN="$QT_PATH\bin"

$GCC_PATH="C:\Qt\Tools\mingw810_32\bin"
$NINJA_PATH="C:\Qt\Tools\Ninja"
$CMAKE_PATH="C:\Qt\Tools\CMake_64\bin"
$GIT_PATH="C:\Program Files (x86)\Git\bin"
$INNO_PATH="C:\Program Files (x86)\Inno Setup 6"
$SEVEN_ZIP_PATH="C:\projects\7-Zip"
$EXTRA_BIN="C:\projects\media-downloader-extra\3rdParty"
$EXTRA_LIB="C:\projects\media-downloader-extra\lib"

$SRC_LOCATION=$PSScriptRoot

$BUILD_PATH="$SRC_LOCATION/BUILD"

$env:Path = "$GCC_PATH;$SEVEN_ZIP_PATH;$QT_PATH_BIN;$NINJA_PATH;$CMAKE_PATH;$INNO_PATH;$GIT_PATH" + $env:Path

Set-Location -Path $SRC_LOCATION

if(Test-Path -Path $BUILD_PATH/MediaDownloader)
{
   Remove-Item -Path $BUILD_PATH/MediaDownloader -Recurse
}

New-Item -ItemType Directory -Path $BUILD_PATH/MediaDownloader

cmake.exe -DCMAKE_VERBOSE_MAKEFILE=FALSE -DCMAKE_BUILD_TYPE:STRING=Release -DCMAKE_PREFIX_PATH="$QT_PATH" -DLIBRARIES_LOCATION="$BUILD_PATH" -DOUTPUT_PATH="$BUILD_PATH" -DSOURCE_PATH="$SRC_LOCATION" -G "CodeBlocks - MinGW Makefiles" -S "$SRC_LOCATION" -B "$BUILD_PATH/MediaDownloader"

if($?)
{
}
else
{
   Read-Host "Press Enter To Exit"
   exit
}

$data = [System.IO.File]::ReadAllText("$BUILD_PATH/MediaDownloader/version.h")

$versionInfoLines = $data.Split([Environment]::NewLine, [System.StringSplitOptions]::RemoveEmptyEntries)

$versionInfoFirstLine = $versionInfoLines[0]

$versionInfo = $versionInfoFirstLine.Split(" ",[System.StringSplitOptions]::RemoveEmptyEntries)[2]

$VERSION = $versionInfo.Replace(".git_tag","")

$VERSION = $VERSION.Replace('"',"")

cmake.exe -DCMAKE_VERBOSE_MAKEFILE=FALSE -DCMAKE_BUILD_TYPE:STRING=Release -DCMAKE_PREFIX_PATH="$QT_PATH" -DLIBRARIES_LOCATION="$BUILD_PATH/MediaDownloader-$VERSION" -DOUTPUT_PATH="$BUILD_PATH" -DSOURCE_PATH="$SRC_LOCATION" -G "CodeBlocks - MinGW Makefiles" -S "$SRC_LOCATION" -B "$BUILD_PATH/MediaDownloader"

if($?)
{
}
else
{
   Read-Host "Press Enter To Exit"
   exit
}

if(Test-Path -Path $BUILD_PATH/MediaDownloader-$VERSION)
{
   Remove-Item -Path $BUILD_PATH/MediaDownloader-$VERSION -Recurse
}

cmake --build $BUILD_PATH/MediaDownloader

if($?)
{
}
else
{
   Read-Host "Press Enter To Exit"
   exit
}

New-Item -ItemType Directory -Path $BUILD_PATH/MediaDownloader-$VERSION

Copy-Item -Path $SRC_LOCATION/translations -Destination $BUILD_PATH/MediaDownloader-$VERSION -Recurse
Copy-Item -Path $BUILD_PATH/MediaDownloader/media-downloader.exe -Destination $BUILD_PATH/MediaDownloader-$VERSION
Copy-Item -Path $EXTRA_BIN -Destination $BUILD_PATH/MediaDownloader-$VERSION -Recurse
Copy-Item -Path $EXTRA_LIB/* -Destination $BUILD_PATH/MediaDownloader-$VERSION

windeployqt.exe $BUILD_PATH/MediaDownloader-$VERSION/media-downloader.exe --libdir $BUILD_PATH/MediaDownloader-$VERSION --plugindir $BUILD_PATH/MediaDownloader-$VERSION

if($?)
{
}
else
{
   Read-Host "Press Enter To Exit"
   exit
}

iscc $BUILD_PATH\MediaDownloader\media-downloader_windows_installer_Qt5.iss

if($?)
{
}
else
{
   Read-Host "Press Enter To Exit"
   exit
}

Write-Output "$VERSION" | Out-File -FilePath $BUILD_PATH/MediaDownloader-$VERSION/version_info.txt -Encoding utf8 -Append

New-Item -ItemType Directory -Path $BUILD_PATH/MediaDownloader-$VERSION/local

7z.exe a $BUILD_PATH/MediaDownloader-$VERSION.zip $BUILD_PATH/MediaDownloader-$VERSION

if($?)
{
   Read-Host "SUCCESS!! Press Enter To Exit"
}
else
{
   Read-Host "Press Enter To Exit"
}
