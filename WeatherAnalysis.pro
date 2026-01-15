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
    src/models/citymodel.cpp \
    src/models/cityfiltermodel.cpp \
    src/services/cityservice.cpp \
    src/services/weatherservice.cpp \
    src/workers/weatherworker.cpp \
    src/views/citywidget.cpp \
    src/views/currentweatherwidget.cpp \
    src/views/forecastwidget.cpp

HEADERS += \
    src/mainwindow.h \
    src/database/databasemanager.h \
    src/network/networkmanager.h \
    src/config/configmanager.h \
    src/models/citymodel.h \
    src/models/cityfiltermodel.h \
    src/models/weatherdata.h \
    src/services/cityservice.h \
    src/services/weatherservice.h \
    src/workers/weatherworker.h \
    src/views/citywidget.h \
    src/views/currentweatherwidget.h \
    src/views/forecastwidget.h

FORMS += \
    src/mainwindow.ui \
    src/views/citywidget.ui \
    src/views/currentweatherwidget.ui \
    src/views/forecastwidget.ui

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
