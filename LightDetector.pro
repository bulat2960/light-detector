QT       += core gui charts serialport serialbus printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    chart.cpp \
    chartview.cpp \
    fadinglabel.cpp \
    logger.cpp \
    main.cpp \
    mainwindow.cpp \
    modbusclient.cpp \
    parameterscalculator.cpp \
    pointlabelitem.cpp \
    protocolcreator.cpp \
    settingsdialog.cpp \
    startexperimentdialog.cpp

HEADERS += \
    chart.h \
    chartview.h \
    dataunit.h \
    fadinglabel.h \
    logger.h \
    mainwindow.h \
    modbusclient.h \
    parameterscalculator.h \
    pointlabelitem.h \
    protocolcreator.h \
    settingsdialog.h \
    startexperimentdialog.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
