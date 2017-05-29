#-------------------------------------------------
#
# Project created by QtCreator 2017-05-11T18:28:46
#
#-------------------------------------------------

QT += core gui network

win32:RC_FILE = vk.rc

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = VK
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    vkapi.cpp \
    messagemanager.cpp \
    abstractmanager.cpp \
    usermanager.cpp

HEADERS  += mainwindow.h \
    vkapi.h \
    vktypes.h \
    messagemanager.h \
    abstractmanager.h \
    usermanager.h \
    vkdata.hpp

FORMS    += mainwindow.ui

RESOURCES += \
    data.qrc

DISTFILES += \
    vk.rc
