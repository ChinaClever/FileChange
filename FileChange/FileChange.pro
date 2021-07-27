QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CleverPackagingTool_V1.8

CONFIG += c++11

# Cryptopp使用方法链接：https://blog.zhengjunxin.com/archives/996/
# https://github.com/weidai11/cryptopp
# 使用了英特尔官方指令集用于优化算法速度，以下是缺少的：
QMAKE_CXXFLAGS += "-mssse3"
QMAKE_CXXFLAGS += "-msse4.1"
QMAKE_CXXFLAGS += "-mavx2"
QMAKE_CXXFLAGS += "-mpclmul"
QMAKE_CXXFLAGS += "-maes"
QMAKE_CXXFLAGS += "-msha"
# 支持OpenMP特性，发挥算法的最大潜能
LIBS += -lgomp -lpthread
QMAKE_CXXFLAGS += "-fopenmp"

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
