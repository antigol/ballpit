#-------------------------------------------------
#
# Project created by QtCreator 2012-11-25T22:52:49
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = particles
TEMPLATE = app


SOURCES += main.cpp\
    skybox.cpp \
    glcube.cpp \
    glsphere.cpp \
    widget.cpp \
    particle.cpp

HEADERS  += \
    skybox.h \
    glcube.h \
    glsphere.h \
    widget.h \
    shaders.h \
    particle.h

OTHER_FILES +=

RESOURCES += \
    data.qrc
