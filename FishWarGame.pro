QT += core widgets

CONFIG += c++17

TARGET = FishingWarGame
TEMPLATE = app

SOURCES += \
    main.cpp \
    gamewindow.cpp \
    gamewidget.cpp \
    gamemath.cpp \
    cannon.cpp \
    bullet.cpp \
    fish.cpp \
    fishmanager.cpp \
    specialeventmanager.cpp

HEADERS += \
    gamewindow.h \
    gamewidget.h \
    gamemath.h \
    cannon.h \
    bullet.h \
    fish.h \
    fishmanager.h \
    specialeventmanager.h

RESOURCES += \
    resources.qrc

# 编译选项
QMAKE_CXXFLAGS += -Wall -Wextra
