#-------------------------------------------------
#
# Project created by QtCreator 2015-04-08T09:35:05
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = SpectrumAnalyzer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qcustomplot.cpp \
    viewwidget.cpp \
    spectrumplot.cpp \
    redpitayainterface.cpp \
    connectwrapper.cpp \
    settingswidget.cpp \
    fftwidget.cpp \
    consolewindow.cpp \
    helper.cpp \
    fft.cpp \
    plotview.cpp

HEADERS  += mainwindow.h \
    qcustomplot.h \
    viewwidget.h \
    spectrumplot.h \
    redpitayainterface.h \
    connectwrapper.h \
    settingswidget.h \
    fftwidget.h \
    consolewindow.h \
    helper.h \
    fft.h \
    plotview.h

FORMS    += mainwindow.ui \
    viewwidget.ui \
    settingswidget.ui \
    fftwidget.ui \
    consolewindow.ui \
    plotview.ui
