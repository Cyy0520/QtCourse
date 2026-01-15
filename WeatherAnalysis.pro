QT       += core gui network sql charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/database/databasemanager.cpp \
    src/network/networkmanager.cpp \
    src/config/configmanager.cpp \
    src/models/citymodel.cpp

HEADERS += \
    src/mainwindow.h \
    src/database/databasemanager.h \
    src/network/networkmanager.h \
    src/config/configmanager.h \
    src/models/citymodel.h

FORMS += \
    src/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# Output directories
DESTDIR = $$PWD/bin
OBJECTS_DIR = $$PWD/build/obj
MOC_DIR = $$PWD/build/moc
UI_DIR = $$PWD/build/ui
RCC_DIR = $$PWD/build/rcc
