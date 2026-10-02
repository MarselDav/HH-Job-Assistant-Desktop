QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 console

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    apiclient.cpp \
    flowlayout.cpp \
    main.cpp \
    mainwindow.cpp \
    resumehistorymanager.cpp \
    searchfield.cpp \
    filterbutton.cpp \
    resumeanalyzerwidget.cpp \
    resumedropzone.cpp \
    jsonformatter.cpp \
    resumecard.cpp \
    vacanciessearchwidget.cpp \
    vacancycard.cpp

HEADERS += \
    apiclient.h \
    flowlayout.h \
    mainwindow.h \
    resumehistorymanager.h \
    stylesheetloader.h \
    searchfield.h \
    filterbutton.h \
    resumeanalyzerwidget.h \
    resumedropzone.h \
    jsonformatter.h \
    resumecard.h \
    vacanciessearchwidget.h \
    vacancycard.h

FORMS += \
    mainwindow.ui \
    vacancycard.ui

TRANSLATIONS += \
    HH-Job-Assistant-Desktop_ru_RU.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
