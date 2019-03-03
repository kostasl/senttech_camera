#-------------------------------------------------
#
# Project created by QtCreator 2017-02-14T11:39:45
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = viewer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    capturewindow.cpp \
    cameraid.cpp \
    settingfile.cpp

HEADERS  += mainwindow.h \
    capturewindow.h \
    cameraid.h \
    settingfile.h

FORMS    += mainwindow.ui \
    capturewindow.ui \
    cameraid.ui \
    settingfile.ui

unix:!macx: LIBS += -L/usr/lib/x86_64-linux-gnu/ -lopencv_core -lopencv_imgproc \
    -L/usr/local/lib/sentech/ -lstcam

INCLUDEPATH += /usr/lib/x86_64-linux-gnu \
    /usr/local/lib/sentech

DEPENDPATH += /usr/lib/x86_64-linux-gnu \
    /usr/local/lib/sentech
