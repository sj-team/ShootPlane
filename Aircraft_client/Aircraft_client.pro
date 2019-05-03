#-------------------------------------------------
#
# Project created by QtCreator 2019-04-21T14:03:51
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Aircraft_client
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
        changepwd.cpp \
        creategame.cpp \
        errbox.cpp \
        friendlist.cpp \
        gamegui.cpp \
        login.cpp \
        main.cpp \
        mainwindow.cpp \
        mytablewidget.cpp \
        regsiter.cpp \
        setting.cpp \
        tools.cpp

HEADERS += \
        changepwd.h \
        creategame.h \
        errbox.h \
        friendlist.h \
        gamegui.h \
        login.h \
        mainwindow.h \
        mytablewidget.h \
        regsiter.h \
        setting.h \
        tools.h

FORMS += \
        changepwd.ui \
        creategame.ui \
        errbox.ui \
        friendlist.ui \
        gamegui.ui \
        login.ui \
        mainwindow.ui \
        regsiter.ui \
        setting.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES +=
