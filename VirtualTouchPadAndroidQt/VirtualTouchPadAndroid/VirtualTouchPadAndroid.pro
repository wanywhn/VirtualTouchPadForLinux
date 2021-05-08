#-------------------------------------------------
#
# Project created by QtCreator 2019-06-12T08:30:36
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = VirtualTouchPadAndroid
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        startscreen.cpp \
        touchscreen.cpp \
        touchPad.cpp

HEADERS += \
    startscreen.h \
    touchscreen.h \
    touchPad.h \
    PacketBuilder/PacketConfigBuilder.h \
    PacketBuilder/PacketHeadBuilder.h \
    PacketBuilder/PacketMotionBuilder.h \
    PacketBuilder/PacketStatusBuilder.h
android: include(/home/tender/Android/Sdk/android_openssl/openssl.pri)

DISTFILES += \
    android/AndroidManifest.xml \
    android/build.gradle \
    android/gradle.properties \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew.bat \
    android/res/values/libs.xml

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

FORMS += \
    startscreen.ui
