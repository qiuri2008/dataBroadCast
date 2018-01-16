#-------------------------------------------------
#
# Project created by QtCreator 2014-04-08T19:54:40
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = abs
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    sction.cpp \
    demux.cpp \
    crc.cpp \
    dvb.cpp \
    batParse.cpp \
    com_is_buf.cpp \
    comParse.cpp \
    uial_xmlparser.cpp \
    uial_info_service.cpp \
    ui_service_index.cpp \
    bmp8_to_16.cpp \
    bmp32_to_16.cpp \
    bmp24_to_16.cpp \
    bmp16_to_16.cpp \
    ui_service_second.cpp \
    ui_service_third.cpp

HEADERS  += mainwindow.h \
    type_.h \
    sction.h \
    pat.h \
    demux.h \
    dvb.h \
    crc.h \
    batParse.h \
    com_is_buf.h \
    com_is_buf_type.h \
    comParse.h \
    bmp32_to_16.h \
    bmp8_to_16.h \
    bmp16_to_16.h \
    bmp24_to_16.h \
    uial_xmlparser.h \
    uial_info_service.h

FORMS    += mainwindow.ui
