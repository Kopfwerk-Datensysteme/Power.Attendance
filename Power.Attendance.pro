QT += core gui sql network printsupport widgets svg

TARGET = Power.Attendance

TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

QMAKE_CXXFLAGS += /std:c++20

SOURCES += \
		common.cpp \
		database.cpp \
		fingerprintreader.cpp \
		main.cpp \
		mainwindow.cpp \
		timestampdialog.cpp \
		userdialog.cpp \
		usermodifydialog.cpp

HEADERS += \
                common.h \
                database.h \
		fingerprintreader.h \
		mainwindow.h \
    timestampdialog.h \
    userdialog.h \
    usermodifydialog.h

FORMS += \
		mainwindow.ui \
		timestampdialog.ui \
		userdialog.ui \
		usermodifydialog.ui

LIBS += \
                -lwinbio

RESOURCES += \
		Power.Attendance.qrc
