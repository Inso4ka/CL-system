TEMPLATE = app
TARGET = client
CONFIG += c++17
QT += widgets

SOURCES += UI/main.cpp \
    UI/mainwindow.cpp \
    UI/connection.cpp \
    client/client.cpp \
    UI/temperature.cpp \
    UI/light.cpp

HEADERS += \
    UI/mainwindow.h \
    UI/connection.h \
    client/client.h \
    UI/temperature.h \
    UI/light.h




INCLUDEPATH += /opt/homebrew/Cellar/boost/1.81.0_1/include/

FORMS += \
    UI/connection.ui \
    UI/mainwindow.ui \
    UI/temperature.ui \
    UI/light.ui

RESOURCES += \
    res.qrc




