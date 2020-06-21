#-------------------------------------------------
#
# Project created by QtCreator 2020-05-27T20:19:38
#
#-------------------------------------------------

QT       += core gui
QT+=network
QT+=multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = udp
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

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    rsa.cpp \
    heartbeatthread.cpp \
    filedialog.cpp \
    sendfilethread.cpp \
    mediaplayer.cpp

HEADERS += \
        mainwindow.h \
    rsa.h \
    heartbeatthread.h \
    filedialog.h \
    sendfilethread.h \
    mediaplayer.h

FORMS += \
        mainwindow.ui \
    filedialog.ui \
    mediaplayer.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

LIBS += -lcryptopp
CONFIG += console
