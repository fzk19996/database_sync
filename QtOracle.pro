QT       += core gui
QT +=sql
QT       += network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
QMAKE_CXXFLAGS += -std=c++11
LIBS += -lrt

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
    condition_monitor.cpp \
    destination_dingyue.cpp \
    dialog.cpp \
    error_log.cpp \
    main.cpp \
    mainwindow.cpp \
    source_dingyue.cpp \
    source_peizhi.cpp \
    transfer_log.cpp\
    net_speed.cpp \
    common.cpp \
    tcpserver.cpp \
    netWindow.cpp

HEADERS += \
    condition_monitor.h \
    destination_dingyue.h \
    dialog.h \
    error_log.h \
    mainwindow.h \ \
    source_dingyue.h \
    source_peizhi.h \
    transfer_log.h \
    common.h \
    packet.h \
    net_speed.h \
    tcpserver.h \
    netWindow.h

FORMS += \
    condition_monitor.ui \
    destination_dingyue.ui \
    dialog.ui \
    error_log.ui \
    mainwindow.ui \
    source_dingyue.ui \
    source_peizhi.ui \
    transfer_log.ui \
    netwindow.ui \
    net_window.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

RESOURCES += \
    condition_monitor.qrc
