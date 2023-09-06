TEMPLATE = app
TARGET = client
CONFIG += c++17
QT += widgets

SOURCES += UI/main.cpp \
    client/client.cpp \
    UI/mainwindow.cpp

HEADERS += client/client.h \
    UI/mainwindow.h

INCLUDEPATH += /opt/homebrew/Cellar/boost/1.81.0_1/include/

FORMS += \
    mainwindow.ui

RESOURCES += \
    res.qrc
