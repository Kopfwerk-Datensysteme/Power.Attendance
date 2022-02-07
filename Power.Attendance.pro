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
		passwordchangedialog.cpp \
		timestampdialog.cpp \
		userdialog.cpp \
		usermodifydialog.cpp

HEADERS += \
                common.h \
                database.h \
		fingerprintreader.h \
		mainwindow.h \
    passwordchangedialog.h \
                timestampdialog.h \
                userdialog.h \
                usermodifydialog.h \

FORMS += \
		mainwindow.ui \
    passwordchangedialog.ui \
		timestampdialog.ui \
		userdialog.ui \
                usermodifydialog.ui \

LIBS += \

RESOURCES += \
		Power.Attendance.qrc
