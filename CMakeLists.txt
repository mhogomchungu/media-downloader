if(${CMAKE_VERSION} VERSION_GREATER "3.26")
    cmake_minimum_required(VERSION 3.26.0)
else()
    cmake_minimum_required(VERSION 3.0.2)
endif()

project(media-downloader LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set( PGR_VERSION "5.4.6" )
set( COPYRIGHT_YEARS "2021-2025" )

INCLUDE(GNUInstallDirs)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_definitions(-Qunused-arguments)
endif()

if(${CMAKE_VERSION} VERSION_LESS "3.1.0")
    message("Warning your cmake version is too old(<3.1.0) and code my not compile")
    add_definitions(-std=c++14)
endif()

option(BUILD_WITH_QT6 "Build with Qt 6" OFF)

if(BUILD_WITH_QT6)
    find_package(Qt6 COMPONENTS Core Widgets Network REQUIRED )
    message("-- Building With Qt6")
else()
    find_package(Qt5 COMPONENTS Core QUIET)
    if(Qt5_FOUND)
        message("-- Building With Qt5")
        set(BUILD_WITH_QT6 OFF)
        find_package(Qt5 COMPONENTS Core Widgets Network REQUIRED)
    else()
        find_package(Qt6 COMPONENTS Core QUIET)
        if(Qt6_FOUND)
            message("-- Building With Qt6")
            set(BUILD_WITH_QT6 ON)
            find_package(Qt6 COMPONENTS Core Widgets Network REQUIRED)
    else()
        message( FATAL_ERROR "-- Failed to find Qt5 or Qt6")
        endif()
    endif()
endif()

if(BUILD_FLATPAK)
    file( WRITE  ${PROJECT_BINARY_DIR}/flatpak.h "\n#define FLATPAK 1\n" )
else()
    file( WRITE  ${PROJECT_BINARY_DIR}/flatpak.h "\n#define FLATPAK 0\n" )
endif()

include_directories(${PROJECT_BINARY_DIR})

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

set(UI_FILES
    src/mainwindow.ui
    src/logwindow.ui)

set(MOC_FILES
    src/mainwindow.h
    src/logwindow.h
    src/basicdownloader.h
    src/batchdownloader.h
    src/configure.h
    src/library.h
    src/playlistdownloader.h
    src/utils/single_instance.hpp)

set(SRC
    src/mainwindow.cpp
    src/logwindow.cpp
    src/main.cpp
    src/settings.cpp
    src/utility.cpp
    src/proxy.cpp
    src/basicdownloader.cpp
    src/about.cpp
    src/reportFinished.cpp
    src/themes.cpp
    src/batchdownloader.cpp
    src/tabmanager.cpp
    src/directoryEntries.cpp
    src/configure.cpp
    src/versionInfo.cpp
    src/translator.cpp
    src/playlistdownloader.cpp
    src/networkAccess.cpp
    src/logger.cpp
    src/engines.cpp
    src/tableWidget.cpp
    src/library.cpp
    src/engines/tests.cpp
    src/engines/yt-dlp.cpp
    src/engines/safaribooks.cpp
    src/engines/generic.cpp
    src/engines/aria2c.cpp
    src/engines/lux.cpp
    src/engines/wget.cpp
    src/engines/svtplay-dl.cpp
    src/engines/you-get.cpp
    src/engines/getsauce.cpp
    src/engines/gallery-dl.cpp
    src/engines/quickjs.cpp
    src/engines/deno.cpp)

set(RESOURCES src/icon.qrc )

if(WIN32)
    set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/src/appicon.rc")
    add_executable(media-downloader WIN32 ${SRC} ${MOC_FILES} ${UI_FILES} ${RESOURCES} ${APP_ICON_RESOURCE_WINDOWS})
    set_target_properties(media-downloader PROPERTIES COMPILE_FLAGS "-Wextra -Wall -s -pedantic")
else()
    if(APPLE)
        set(icon "${CMAKE_CURRENT_SOURCE_DIR}/src/media-downloader.icns")
        add_executable(MediaDownloader MACOSX_BUNDLE ${SRC} ${MOC_FILES} ${UI_FILES} ${RESOURCES} ${icon})
        set_target_properties(MediaDownloader PROPERTIES MACOSX_BUNDLE ON)
        set_target_properties(MediaDownloader PROPERTIES COMPILE_FLAGS "-Wextra -Wall -s -fPIC -pedantic")
    else()
        add_executable(media-downloader ${SRC} ${MOC_FILES} ${UI_FILES} ${RESOURCES})
        set_target_properties(media-downloader PROPERTIES COMPILE_FLAGS "-Wextra -Wall -s -fPIC -pedantic")
    endif()
endif()

if(BUILD_WITH_QT6)
    if(WIN32)
        target_link_libraries(media-downloader Advapi32.lib Dwmapi.lib Iphlpapi.lib Qt6::Widgets Qt6::Core Qt6::Network)
    else()
        if(APPLE)
            target_link_libraries(MediaDownloader Qt6::Widgets Qt6::Core Qt6::Network)
        else()
            target_link_libraries(media-downloader Qt6::Widgets Qt6::Core Qt6::Network)
        endif()
    endif()
else()
    if(WIN32)
        target_link_libraries(media-downloader Advapi32.lib Dwmapi.lib Iphlpapi.lib Qt5::Widgets Qt5::Core Qt5::Network)
    else()
        if(APPLE)
             target_link_libraries(MediaDownloader Qt5::Widgets Qt5::Core Qt5::Network)
        else()
            target_link_libraries(media-downloader Qt5::Widgets Qt5::Core Qt5::Network)
        endif()
    endif()
endif()

if(APPLE)
else()
    if(BUILD_FLATPAK)
        set_target_properties( media-downloader PROPERTIES OUTPUT_NAME io.github.mhogomchungu.media-downloader)
        install(TARGETS media-downloader RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
        install(FILES src/flatpak/io.github.mhogomchungu.media-downloader.metainfo.xml DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/metainfo)
        install(FILES src/flatpak/io.github.mhogomchungu.media-downloader.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/512x512/apps)
        install(FILES src/flatpak/io.github.mhogomchungu.media-downloader.desktop DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications)
    else()
        install(TARGETS media-downloader RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
        install(FILES src/media-downloader.desktop DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications)
        install(FILES src/media-downloader.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pixmaps)
    endif()
endif()

install(DIRECTORY translations DESTINATION ${CMAKE_INSTALL_DATADIR}/media-downloader FILES_MATCHING PATTERN "*.qm")

file(WRITE ${PROJECT_BINARY_DIR}/locale_path.h "\n#define TRANSLATION_PATH \"${CMAKE_INSTALL_FULL_DATADIR}/media-downloader/translations/\"\n")

set( OutputBaseFilename "MediaDownloader-${PGR_VERSION}.setup" )

if(OUTPUT_BASE_NAME)
    set(OutputBaseFilename ${OUTPUT_BASE_NAME})
endif()

if(BUILD_WITH_QT6)

file(WRITE ${PROJECT_BINARY_DIR}/version.h
"
#define VERSION \"${PGR_VERSION}.git_tag\"
#define QTVERSION \"${Qt6Core_VERSION}\"
#define COPYRIGHT \"${COPYRIGHT_YEARS}\"
")

else()
file(WRITE ${PROJECT_BINARY_DIR}/version.h
"
#define VERSION \"${PGR_VERSION}.git_tag\"
#define QTVERSION \"${Qt5Core_VERSION}\"
#define COPYRIGHT \"${COPYRIGHT_YEARS}\"
")
endif()

file(WRITE ${PROJECT_BINARY_DIR}/VERSION_INFO "${PGR_VERSION}")

if(BUILD_WITH_QT6)

file(WRITE ${PROJECT_BINARY_DIR}/media-downloader_windows_installer_Qt6.iss
"
; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName            \"Media Downloader\"
#define MyAppVersion         \"${PGR_VERSION}\"
#define MyAppPublisher       \"Francis Banyikwa\"
#define MyAppURL             \"https://github.com/mhogomchungu/media-downloader\"
#define MyAppExeName         \"media-downloader.exe\"
#define MyAppCopyright       \"${COPYRIGHT_YEARS}, Francis Banyikwa\"
#define MyEXELocation        \"${PROJECT_BINARY_DIR}\\media-downloader.exe\"
#define MyBuildPath          \"${PROJECT_BINARY_DIR}\"
#define MyLibrariesLocation  \"${LIBRARIES_LOCATION}\"
#define MyOutPutPath         \"${OUTPUT_PATH}\"
#define MySourcesPath        \"${SOURCE_PATH}\"

[Setup]
AppId={{6DD595EF-ECA2-481B-B008-CB7302603A0D}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppVerName={#MyAppName} {#MyAppVersion}
AppCopyright={#MyAppCopyright}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
VersionInfoDescription={#MyAppName} installer
VersionInfoProductName={#MyAppName}
VersionInfoVersion={#MyAppVersion}.0
UninstallDisplayName={#MyAppName}
UninstallDisplayIcon={app}\\{#MyAppExeName}

DefaultDirName={pf}\\{#MyAppName}
LicenseFile={#MySourcesPath}\\LICENSE.txt
OutputDir={#MyOutPutPath}
SetupIconFile={#MySourcesPath}\\src\\media-downloader.ico
Compression=lzma
SolidCompression=yes
DisableProgramGroupPage=yes
OutputBaseFilename=${OutputBaseFilename}

[Tasks]
Name: \"desktopicon\"; Description: \"{cm:CreateDesktopIcon}\"; GroupDescription: \"{cm:AdditionalIcons}\"; Flags: unchecked
Name: \"quicklaunchicon\"; Description: \"{cm:CreateQuickLaunchIcon}\"; GroupDescription: \"{cm:AdditionalIcons}\"; Flags: unchecked; OnlyBelowVersion: 0,6.1

[Files]
Source: {#MyEXELocation}; DestDir: {app}; Flags: ignoreversion
Source: {#MyLibrariesLocation}\\*.dll; DestDir: {app} ; Flags: ignoreversion
Source: {#MySourcesPath}\\LICENSE.txt; DestDir: {app}; Flags: ignoreversion
Source: {#MyLibrariesLocation}\\iconengines\\* ; DestDir: {app}\\iconengines; Flags: ignoreversion
Source: {#MyLibrariesLocation}\\imageformats\\*; DestDir: {app}\\imageformats; Flags: ignoreversion
Source: {#MyLibrariesLocation}\\styles\\*; DestDir: {app}\\styles; Flags: ignoreversion
Source: {#MyLibrariesLocation}\\platforms\\*; DestDir: {app}\\platforms; Flags: ignoreversion
Source: {#MyLibrariesLocation}\\generic\\*; DestDir: {app}\\generic; Flags: ignoreversion
Source: {#MyLibrariesLocation}\\tls\\*; DestDir: {app}\\tls; Flags: ignoreversion
Source: {#MyLibrariesLocation}\\networkinformation\\*; DestDir: {app}\\networkinformation; Flags: ignoreversion
Source: {#MyLibrariesLocation}\\3rdParty\\*; DestDir: {app}\\3rdParty; Flags: ignoreversion recursesubdirs
Source: {#MySourcesPath}\\translations\\*; DestDir: {app}\\translations; Flags: ignoreversion
Source: {#MySourcesPath}\\src\\media-downloader.ico; DestDir: {app}\\icons; Flags: ignoreversion

; NOTE: Don't use \"Flags: ignoreversion\" on any shared system files

[Icons]
Name: \"{commonprograms}\\{#MyAppName}\"; Filename: \"{app}\\{#MyAppExeName}\"; IconFilename: \"{app}\\icons\\media-downloader.ico\"; WorkingDir: \"{app}\"
;Name: \"{commonprograms}\\Uninstall {#MyAppName}\"; Filename: \"{uninstallexe}\"; IconFilename: \"{app}\\icons\\media-downloader.ico\"; WorkingDir: \"{app}\"
Name: \"{commondesktop}\\{#MyAppName}\"; Filename: \"{app}\\{#MyAppExeName}\"; IconFilename: \"{app}\\icons\\media-downloader.ico\"; WorkingDir: \"{app}\"; Tasks: desktopicon
Name: \"{userappdata}\\Microsoft\\Internet Explorer\\Quick Launch\\{#MyAppName}\"; Filename: \"{app}\\{#MyAppExeName}\"; IconFilename: \"{app}\\icons\\media-downloade.ico\"; WorkingDir: \"{app}\"; Tasks: quicklaunchicon

[Run]
Filename: \"{app}\\{#MyAppExeName}\"; Description: \"{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}\"; Flags: nowait postinstall skipifsilent

[Languages]
Name: \"english\"             ; MessagesFile: \"compiler:Default.isl\"
Name: \"brazilianportuguese\" ; MessagesFile: \"compiler:Languages\\BrazilianPortuguese.isl\"
Name: \"catalan\"             ; MessagesFile: \"compiler:Languages\\Catalan.isl\"
Name: \"corsican\"            ; MessagesFile: \"compiler:Languages\\Corsican.isl\"
Name: \"czech\"               ; MessagesFile: \"compiler:Languages\\Czech.isl\"
Name: \"danish\"              ; MessagesFile: \"compiler:Languages\\Danish.isl\"
Name: \"dutch\"               ; MessagesFile: \"compiler:Languages\\Dutch.isl\"
Name: \"finnish\"             ; MessagesFile: \"compiler:Languages\\Finnish.isl\"
Name: \"french\"              ; MessagesFile: \"compiler:Languages\\French.isl\"
Name: \"german\"              ; MessagesFile: \"compiler:Languages\\German.isl\"
Name: \"hebrew\"              ; MessagesFile: \"compiler:Languages\\Hebrew.isl\"
Name: \"italian\"             ; MessagesFile: \"compiler:Languages\\Italian.isl\"
Name: \"japanese\"            ; MessagesFile: \"compiler:Languages\\Japanese.isl\"
Name: \"norwegian\"           ; MessagesFile: \"compiler:Languages\\Norwegian.isl\"
Name: \"polish\"              ; MessagesFile: \"compiler:Languages\\Polish.isl\"
Name: \"portuguese\"          ; MessagesFile: \"compiler:Languages\\Portuguese.isl\"
Name: \"russian\"             ; MessagesFile: \"compiler:Languages\\Russian.isl\"
Name: \"slovenian\"           ; MessagesFile: \"compiler:Languages\\Slovenian.isl\"
Name: \"spanish\"             ; MessagesFile: \"compiler:Languages\\Spanish.isl\"
Name: \"turkish\"             ; MessagesFile: \"compiler:Languages\\Turkish.isl\"
Name: \"ukrainian\"           ; MessagesFile: \"compiler:Languages\\Ukrainian.isl\"
")

else()

file(WRITE ${PROJECT_BINARY_DIR}/media-downloader_windows_installer_Qt5.iss
"
; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName            \"Media Downloader\"
#define MyAppVersion         \"${PGR_VERSION}\"
#define MyAppPublisher       \"Francis Banyikwa\"
#define MyAppURL             \"https://github.com/mhogomchungu/media-downloader\"
#define MyAppExeName         \"media-downloader.exe\"
#define MyAppCopyright       \"${COPYRIGHT_YEARS}, Francis Banyikwa\"
#define MyEXELocation        \"${PROJECT_BINARY_DIR}\\media-downloader.exe\"
#define MyBuildPath          \"${PROJECT_BINARY_DIR}\"
#define MyLibrariesLocation  \"${LIBRARIES_LOCATION}\"
#define MyOutPutPath         \"${OUTPUT_PATH}\"
#define MySourcesPath        \"${SOURCE_PATH}\"

[Setup]
AppId={{6DD595EF-ECA2-481B-B008-CB7302603A0D}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppVerName={#MyAppName} {#MyAppVersion}
AppCopyright={#MyAppCopyright}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
VersionInfoDescription={#MyAppName} installer
VersionInfoProductName={#MyAppName}
VersionInfoVersion={#MyAppVersion}.0
UninstallDisplayName={#MyAppName}
UninstallDisplayIcon={app}\\{#MyAppExeName}

DefaultDirName={pf}\\{#MyAppName}
LicenseFile={#MySourcesPath}\\LICENSE.txt
OutputDir={#MyOutPutPath}
SetupIconFile={#MySourcesPath}\\src\\media-downloader.ico
Compression=lzma
SolidCompression=yes
DisableProgramGroupPage=yes
OutputBaseFilename=${OutputBaseFilename}

[Tasks]
Name: \"desktopicon\"; Description: \"{cm:CreateDesktopIcon}\"; GroupDescription: \"{cm:AdditionalIcons}\"; Flags: unchecked
Name: \"quicklaunchicon\"; Description: \"{cm:CreateQuickLaunchIcon}\"; GroupDescription: \"{cm:AdditionalIcons}\"; Flags: unchecked; OnlyBelowVersion: 0,6.1

[Files]
Source: {#MyEXELocation}; DestDir: {app}; Flags: ignoreversion
Source: {#MyLibrariesLocation}\\*.dll; DestDir: {app} ; Flags: ignoreversion
Source: {#MySourcesPath}\\LICENSE.txt; DestDir: {app}; Flags: ignoreversion
Source: {#MyLibrariesLocation}\\bearer\\* ; DestDir: {app}\\bearer; Flags: ignoreversion
Source: {#MyLibrariesLocation}\\iconengines\\* ; DestDir: {app}\\iconengines; Flags: ignoreversion
Source: {#MyLibrariesLocation}\\imageformats\\*; DestDir: {app}\\imageformats; Flags: ignoreversion
Source: {#MyLibrariesLocation}\\styles\\*; DestDir: {app}\\styles; Flags: ignoreversion
Source: {#MyLibrariesLocation}\\platforms\\*; DestDir: {app}\\platforms; Flags: ignoreversion
Source: {#MyLibrariesLocation}\\3rdParty\\*; DestDir: {app}\\3rdParty; Flags: ignoreversion recursesubdirs
Source: {#MySourcesPath}\\translations\\*; DestDir: {app}\\translations; Flags: ignoreversion
Source: {#MySourcesPath}\\src\\media-downloader.ico; DestDir: {app}\\icons; Flags: ignoreversion

; NOTE: Don't use \"Flags: ignoreversion\" on any shared system files

[Icons]
Name: \"{commonprograms}\\{#MyAppName}\"; Filename: \"{app}\\{#MyAppExeName}\"; IconFilename: \"{app}\\icons\\media-downloader.ico\"; WorkingDir: \"{app}\"
;Name: \"{commonprograms}\\Uninstall {#MyAppName}\"; Filename: \"{uninstallexe}\"; IconFilename: \"{app}\\icons\\media-downloader.ico\"; WorkingDir: \"{app}\"
Name: \"{commondesktop}\\{#MyAppName}\"; Filename: \"{app}\\{#MyAppExeName}\"; IconFilename: \"{app}\\icons\\media-downloader.ico\"; WorkingDir: \"{app}\"; Tasks: desktopicon
Name: \"{userappdata}\\Microsoft\\Internet Explorer\\Quick Launch\\{#MyAppName}\"; Filename: \"{app}\\{#MyAppExeName}\"; IconFilename: \"{app}\\icons\\media-downloade.ico\"; WorkingDir: \"{app}\"; Tasks: quicklaunchicon

[Run]
Filename: \"{app}\\{#MyAppExeName}\"; Description: \"{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}\"; Flags: nowait postinstall skipifsilent

[Languages]
Name: \"english\"             ; MessagesFile: \"compiler:Default.isl\"
Name: \"brazilianportuguese\" ; MessagesFile: \"compiler:Languages\\BrazilianPortuguese.isl\"
Name: \"catalan\"             ; MessagesFile: \"compiler:Languages\\Catalan.isl\"
Name: \"corsican\"            ; MessagesFile: \"compiler:Languages\\Corsican.isl\"
Name: \"czech\"               ; MessagesFile: \"compiler:Languages\\Czech.isl\"
Name: \"danish\"              ; MessagesFile: \"compiler:Languages\\Danish.isl\"
Name: \"dutch\"               ; MessagesFile: \"compiler:Languages\\Dutch.isl\"
Name: \"finnish\"             ; MessagesFile: \"compiler:Languages\\Finnish.isl\"
Name: \"french\"              ; MessagesFile: \"compiler:Languages\\French.isl\"
Name: \"german\"              ; MessagesFile: \"compiler:Languages\\German.isl\"
Name: \"hebrew\"              ; MessagesFile: \"compiler:Languages\\Hebrew.isl\"
Name: \"italian\"             ; MessagesFile: \"compiler:Languages\\Italian.isl\"
Name: \"japanese\"            ; MessagesFile: \"compiler:Languages\\Japanese.isl\"
Name: \"norwegian\"           ; MessagesFile: \"compiler:Languages\\Norwegian.isl\"
Name: \"polish\"              ; MessagesFile: \"compiler:Languages\\Polish.isl\"
Name: \"portuguese\"          ; MessagesFile: \"compiler:Languages\\Portuguese.isl\"
Name: \"russian\"             ; MessagesFile: \"compiler:Languages\\Russian.isl\"
Name: \"slovenian\"           ; MessagesFile: \"compiler:Languages\\Slovenian.isl\"
Name: \"spanish\"             ; MessagesFile: \"compiler:Languages\\Spanish.isl\"
Name: \"turkish\"             ; MessagesFile: \"compiler:Languages\\Turkish.isl\"
Name: \"ukrainian\"           ; MessagesFile: \"compiler:Languages\\Ukrainian.isl\"
")

endif()

if(APPLE)

    SET( qtconf_dest_dir MediaDownloader.app/Contents/Resources )
    SET(MACOSX_BUNDLE_ICON_FILE media-downloader.icns)
    SET( APPS "\${CMAKE_INSTALL_PREFIX}/MediaDownloader.app" )

    set_target_properties( MediaDownloader
	                   PROPERTIES
			   MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/src/MacOSXBundleInfo.plist.in"
			   MACOSX_BUNDLE_ICON_FILE "media-downloader.icns"
			   MACOSX_BUNDLE_NAME "MediaDownloader"
			   MACOSX_BUNDLE_VERSION "${PGR_VERSION}"
			   MACOSX_BUNDLE_LONG_VERSION_STRING "${PGR_VERSION}"
			   MACOSX_BUNDLE_SHORT_VERSION_STRING "${PGR_VERSION}"
			   MACOSX_BUNDLE_GUI_IDENTIFIER "org.MediaDownloader.gui"
			   MACOSX_BUNDLE_COPYRIGHT "${COPYRIGHT_YEARS}, Francis Banyikwa" )

    install( TARGETS MediaDownloader BUNDLE DESTINATION . COMPONENT Runtime )

    file( COPY src/media-downloader.icns DESTINATION  "MediaDownloader.app/Contents/Resources/" )
    file( COPY translations/ar.qm DESTINATION        "MediaDownloader.app/Contents/Resources/translations/" )
    file( COPY translations/de_DE.qm DESTINATION     "MediaDownloader.app/Contents/Resources/translations/" )
    file( COPY translations/en_US.qm DESTINATION     "MediaDownloader.app/Contents/Resources/translations/" )
    file( COPY translations/es.qm DESTINATION        "MediaDownloader.app/Contents/Resources/translations/" )
    file( COPY translations/fr_FR.qm DESTINATION     "MediaDownloader.app/Contents/Resources/translations/" )
    file( COPY translations/it_IT.qm DESTINATION     "MediaDownloader.app/Contents/Resources/translations/" )
    file( COPY translations/ja_JP.qm DESTINATION     "MediaDownloader.app/Contents/Resources/translations/" )
    file( COPY translations/nl_NL.qm DESTINATION     "MediaDownloader.app/Contents/Resources/translations/" )
    file( COPY translations/pl_PL.qm DESTINATION     "MediaDownloader.app/Contents/Resources/translations/" )
    file( COPY translations/pt_BR.qm DESTINATION     "MediaDownloader.app/Contents/Resources/translations/" )
    file( COPY translations/ru_RU.qm DESTINATION     "MediaDownloader.app/Contents/Resources/translations/" )
    file( COPY translations/sv_SE.qm DESTINATION     "MediaDownloader.app/Contents/Resources/translations/" )
    file( COPY translations/tr_TR.qm DESTINATION     "MediaDownloader.app/Contents/Resources/translations/" )
    file( COPY translations/zh_CN.qm DESTINATION     "MediaDownloader.app/Contents/Resources/translations/" )

    file( COPY changelog DESTINATION                 "MediaDownloader.app/Contents/Resources/" )
endif()
