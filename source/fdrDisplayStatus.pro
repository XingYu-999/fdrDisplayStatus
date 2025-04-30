QT       += core gui network xml sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    getFdrInfoClient.cpp \
    main.cpp \
    mainDialog.cpp \
    mainwinpreference.cpp \
    messagetips.cpp \
    widgetlogin.cpp

HEADERS += \
    backGroundData.h \
    getFdrInfoClient.h \
    mainDialog.h \
    mainwinpreference.h \
    messagetips.h \
    widgetlogin.h

FORMS += \
    mainDialog.ui \
    mainwinpreference.ui \
    widgetlogin.ui

#使用MSVC编译器，指定/mp编译选项，编译器将使用并行编译，同时起多个编译进程并行编译不同的cpp
QMAKE_CXXFLAGS += /MP

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    ../../../build-sorters-Desktop_Qt_6_5_1_MSVC2019_64bit-Release/fdrDisplayStatus/source/system.xml \
    logo.rc

RESOURCES += \
    image.qrc

RC_FILE += \
    logo.rc
