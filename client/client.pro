TEMPLATE = app
TARGET = client
CONFIG += c++17
QT += widgets

ICON = /Users/mac/Documents/Projects/cpp_projects/CL-system/main_icon/1.icns

INCLUDEPATH += ../shared
INCLUDEPATH += /opt/homebrew/Cellar/boost/1.85.0/include/

SOURCES += UI/main.cpp \
    UI/mainwindow.cpp \
    UI/connection.cpp \
    client/client.cpp \
    UI/temperature.cpp \
    UI/light.cpp \
    ../shared/hashfunc.cpp


HEADERS += \
    UI/mainwindow.h \
    UI/connection.h \
    client/client.h \
    UI/temperature.h \
    UI/light.h \
    ../shared/hashfunc.h







FORMS += \
    UI/connection.ui \
    UI/mainwindow.ui \
    UI/temperature.ui \
    UI/light.ui

RESOURCES += \
    res.qrc

contains(ANDROID_TARGET_ARCH,arm64-v8a) {
    ANDROID_PACKAGE_SOURCE_DIR = \
        $$PWD/android
}




