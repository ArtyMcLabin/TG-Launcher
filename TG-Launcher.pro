#-------------------------------------------------
#
# Project created by QtCreator 2015-05-19T18:34:55
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets network

TARGET = TG-Launcher
TEMPLATE = app

#quazip
INCLUDEPATH += C:/Qt/zlib
LIBS += -LC:/Qt/zlib -lz
INCLUDEPATH += C:/Qt/quazip/quazip
LIBS += -LC:/Qt/quazip/quazip/release -lquazip
###############################################^

SOURCES += main.cpp\
        widget.cpp \
    tg_server.cpp \
    ../QStringPaint/qstringpaint.cpp

HEADERS  += widget.h \
    tg_server.h \
    ../QStringPaint/qstringpaint.h

FORMS    += widget.ui

OTHER_FILES +=

RESOURCES += \
    res.qrc

# remove before flight
LIBS += -lpsapi
