QT += core
QT -= gui

TARGET = virtual-rendering-multi-gpu
CONFIG += console
CONFIG -= app_bundle
CONFIG += c++11

TEMPLATE = app

SOURCES += main.cpp \
    VirtualExperiment.cpp \
    VirtualNode.cpp \
    VirtualGPU.cpp \
    VirtualVolume.cpp \
    VirtualFrameWork.cpp \
    VirtualImage.cpp \
    TaskRender.cpp \
    TaskCollect.cpp

HEADERS += \
    VirtualExperiment.h \
    VirtualNode.h \
    VirtualGPU.h \
    VirtualVolume.h \
    VirtualFrameWork.h \
    VirtualImage.h \
    TaskRender.h \
    TaskCollect.h \
    Transformation.h

