#-------------------------------------------------
#
# Project created by QtCreator 2017-12-28T20:32:33
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = learnopengl
TEMPLATE = app
QMAKE_CXXFLAGS+=-fopenmp
LIBS += -fopenmp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    openglwindow.cpp \
    framebuffer.cpp \
    model.cpp \
    marchingcubesdemo.cpp \
    basedemowidget.cpp \
    test.cpp \
    loopsubdivisionsurfacedemo.cpp \
    core.cpp \
    PathTracingDemo.cpp \
    camera.cpp \
    shape.cpp \
    interaction.cpp \
    light.cpp \
    appearance.cpp \
    bvh.cpp \
    geometry.cpp \
    scene.cpp \
    utils.cpp \
    subdivision.cpp \
    mm.cpp \
    arithimetic.cpp \
    kdtree.cpp

HEADERS += \
        mainwindow.h \
    openglwindow.h \
    framebuffer.h \
    model.h \
    marchingcubesdemo.h \
    basedemowidget.h \
    test.h \
    loopsubdivisionsurfacedemo.h \
    core.h \
    PathTracingDemo.h \
    camera.h \
    shape.h \
    interaction.h \
    light.h \
    appearance.h \
    bvh.h \
    geometry.h \
    scene.h \
    utils.h \
    subdivision.h \
    mm.h \
    arithimetic.h \
    kdtree.h

FORMS += \
        mainwindow.ui

DISTFILES += \
    phongshadingvertexshader.glsl \
    phongshadingfragmentshader.glsl

RESOURCES += \
    resources.qrc
