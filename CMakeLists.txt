cmake_minimum_required(VERSION 3.0)

project(media-downloader)

add_definitions(-Wextra -Wall -pedantic)

project(media-downloader)

set( PGR_VERSION "1.0.0" )
set( COPYRIGHT_YEARS "2021" )

INCLUDE(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt5 COMPONENTS Core Widgets  REQUIRED)

add_definitions(${Qt5Widgets_DEFINITIONS})
add_definitions(${Qt5Core_DEFINITIONS})
include_directories(${PROJECT_BINARY_DIR})

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

set(UI_FILES
    src/mainwindow.ui)

set(MOC_FILES
    src/mainwindow.h
    src/basicdownloader.h
    src/batchdownloader.h
    src/batchfiledownloader.h
    src/configure.h
    src/playlistdownloader.h)

set(SRC
    src/mainwindow.cpp
    src/main.cpp
    src/settings.cpp
    src/utility.cpp
    src/basicdownloader.cpp
    src/about.cpp
    src/batchdownloader.cpp
    src/tabmanager.cpp
    src/batchfiledownloader.cpp
    src/configure.cpp
    src/playlistdownloader.cpp)

set(RESOUCES
    src/icon.qrc)

QT5_WRAP_UI(UI ${UI_FILES})
QT5_WRAP_CPP(MOC ${MOC_FILES})
QT5_ADD_RESOURCES(TRAY_RC_SRCS ${RESOUCES})

add_executable(media-downloader ${MOC} ${UI} ${SRC} ${TRAY_RC_SRCS})
target_link_libraries(media-downloader ${Qt5Widgets_LIBRARIES} ${Qt5Core_LIBRARIES})

install(TARGETS media-downloader RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install(FILES src/media-downloader.desktop
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications)

install(FILES src/media-downloader.png
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons)

file( WRITE ${PROJECT_BINARY_DIR}/version.h
"
#define VERSION \"${PGR_VERSION}\"
#define COPYRIGHT \"${COPYRIGHT_YEARS}\"
")
