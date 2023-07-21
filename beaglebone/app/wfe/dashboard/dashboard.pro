# Run this to make the Makefile:
# qmake -makefile -o Makefile dashboard.pro

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = dashboard
CONFIG   += console
CONFIG += -g
CONFIG   -= app_bundle

TEMPLATE += app

SOURCES +=  displays/textdisplay.cpp \
			displays/errordisplay.cpp \
			displays/dial.cpp \
			canBusReader.cpp \
			canLogsWriter.cpp \
			DashData.cpp \
			dashboardUI.cpp \
			main.cpp
HEADERS +=  canBusReader.hpp \
			canLogsWriter.hpp \
			DashData.hpp \
			dashboardUI.hpp

DESTDIR  = ../../../../Bin/dashboard/
OBJECTS_DIR  = ../../../../Bin/dashboard/Src

mkdir.commands = $(CHK_DIR_EXISTS) $${OBJECTS_DIR} $(MKDIR) $${OBJECTS_DIR}
QMAKE_EXTRA_TARGETS += mkdir
