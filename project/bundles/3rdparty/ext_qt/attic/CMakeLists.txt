# Script to build Qt for digiKam bundle.
#
# Copyright (c) 2015-2019, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

SET(EXTPREFIX_qt "${EXTPREFIX}")
SET(QT_VERSION 5.11.3)

IF (${QT_VERSION} STREQUAL "5.6.3")

    INCLUDE("config-5.6.3-linux.cmake")

    ExternalProject_Add(ext_qt
        DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}
        URL https://download.qt.io/official_releases/qt/5.6/5.6.3/single/qt-everywhere-opensource-src-5.6.3.tar.xz
        URL_MD5 010342d515b62ee1c0e709254f4ef9ab

        PATCH_COMMAND ${PATCH_COMMAND} -p1 -i ${CMAKE_CURRENT_SOURCE_DIR}/qt563-appimage-support.patch

        CONFIGURE_COMMAND <SOURCE_DIR>/configure ${QT_CONFIG}

        BUILD_COMMAND make -j

        UPDATE_COMMAND ""
        BUILD_IN_SOURCE 1
        ALWAYS 0
    )

ELSEIF(${QT_VERSION} STREQUAL "5.9.7")

    INCLUDE("config-5.9.7-linux.cmake")

    ExternalProject_Add(ext_qt
        DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}
        URL https://download.qt.io/official_releases/qt/5.9/5.9.7/single/qt-everywhere-opensource-src-5.9.7.tar.xz
        URL_MD5 70e617aeb1f9bbf84a12b8cf09b01ece

        PATCH_COMMAND ${PATCH_COMMAND} -p1 -i ${CMAKE_CURRENT_SOURCE_DIR}/qt597-appimage-support.patch

        CONFIGURE_COMMAND <SOURCE_DIR>/configure ${QT_CONFIG}

        BUILD_COMMAND make -j

        UPDATE_COMMAND ""
        BUILD_IN_SOURCE 1
        ALWAYS 0
    )

ELSEIF(${QT_VERSION} STREQUAL "5.10.1")

    INCLUDE("config-5.10.1-linux.cmake")

    ExternalProject_Add(ext_qt
        DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}
        URL https://download.qt.io/official_releases/qt/5.10/5.10.1/single/qt-everywhere-src-5.10.1.tar.xz
        URL_MD5 7e167b9617e7bd64012daaacb85477af

        PATCH_COMMAND ${PATCH_COMMAND} -p1 -i ${CMAKE_CURRENT_SOURCE_DIR}/qt5101-appimage-support.patch

        CONFIGURE_COMMAND <SOURCE_DIR>/configure ${QT_CONFIG}

        BUILD_COMMAND make -j

        UPDATE_COMMAND ""
        BUILD_IN_SOURCE 1
        ALWAYS 0
    )

ELSEIF(${QT_VERSION} STREQUAL "5.11.3")

    INCLUDE("config-5.11.3-linux.cmake")

    ExternalProject_Add(ext_qt
        DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}
        URL https://download.qt.io/official_releases/qt/5.11/5.11.3/single/qt-everywhere-src-5.11.3.tar.xz
        URL_MD5 02b353bfe7a40a8dc4274e1d17226d2b

        PATCH_COMMAND ${PATCH_COMMAND} -p1 -i ${CMAKE_CURRENT_SOURCE_DIR}/qt5113-appimage-support.patch

        CONFIGURE_COMMAND <SOURCE_DIR>/configure ${QT_CONFIG}

        BUILD_COMMAND make -j

        UPDATE_COMMAND ""
        BUILD_IN_SOURCE 1
        ALWAYS 0
    )

ELSEIF(${QT_VERSION} STREQUAL "5.12.2")

    INCLUDE("config-5.12.2-linux.cmake")

    ExternalProject_Add(ext_qt
        DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}
        URL https://download.qt.io/official_releases/qt/5.12/5.12.2/single/qt-everywhere-src-5.12.2.tar.xz
        URL_MD5 99c2eb46e533371798b4ca2d1458e065

        PATCH_COMMAND ${PATCH_COMMAND} -p1 -i ${CMAKE_CURRENT_SOURCE_DIR}/qt5122-appimage-support.patch

        CONFIGURE_COMMAND <SOURCE_DIR>/configure ${QT_CONFIG}

        BUILD_COMMAND make -j

        UPDATE_COMMAND ""
        BUILD_IN_SOURCE 1
        ALWAYS 0
    )

ELSE()

    MESSAGE(FATAL_ERROR "Qt version to install not supported !")

ENDIF()
