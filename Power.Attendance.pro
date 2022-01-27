QT += core gui sql network printsupport widgets

TARGET = Power.Attendance

TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

QMAKE_CXXFLAGS += /std:c++20

SOURCES += \
		common.cpp \
		database.cpp \
		fingerprintreader.cpp \
		main.cpp \
		mainwindow.cpp

HEADERS += \
                common.h \
                database.h \
		fingerprintreader.h \
		mainwindow.h

FORMS += \
		mainwindow.ui

LIBS += \
                -lwinbio

RESOURCES += \
		Power.Attendance.qrc
