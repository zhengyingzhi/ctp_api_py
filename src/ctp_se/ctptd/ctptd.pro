######################################################################
# Automatically generated by qmake (3.0) Fri Apr 22 14:06:12 2016
######################################################################
TARGET = ctptd_se

build_pass:CONFIG(debug, debug|release) {
        BuildConf = debug
        TARGET = $$join(TARGET,,,_debug)
        BuildSuff = _debug
} else {
        BuildConf = release
        BuildSuff = ""
}


TCONFIG -= qt
QT      -= core gui

CONFIG += debug_and_release plugin
CONFIG += object_parallel_to_source

TEMPLATE = lib



QMAKE_CXXFLAGS += -static-libstdc++
QMAKE_LFLAGS += -static-libstdc++

build_pass:CONFIG(debug, debug|release) {
        DEFINES += DEBUG _DEBUG
        DESTDIR = ../../../build/debug
} else {
        DEFINES += NDEBUG
        DESTDIR = ../../../build/release
}

QMAKE_CFLAGS += -std=c99
QMAKE_CXXFLAGS += -std=c++0x -fPIC -Wno-unused-parameter -fvisibility=hidden -fvisibility-inlines-hidden

DEFINES += __STDC_FORMAT_MACROS


INCLUDEPATH += ../../api/ctp_se
INCLUDEPATH += ../../../thirdparty
INCLUDEPATH += ../../../thirdparty/python35/linux_include

LIBS += -lpthread -lrt

LIBS += -L../../../thirdparty/python35/linux64 -lpython3.5m
LIBS += -L../../api/ctp_se/linux64 -lthosttraderapi_se


# Input
HEADERS += ctptd.h \
    ../myctp.h \
    ../../api/ctp_se/ThostFtdcMdApi.h \
    ../../api/ctp_se/ThostFtdcTraderApi.h \
    ../../api/ctp_se/ThostFtdcUserApiDataType.h \
    ../../api/ctp_se/ThostFtdcUserApiStruct.h \
    ../pyscript/ctp_td_header_function.h \
    ../pyscript/ctp_td_header_on.h \
    ../pyscript/ctp_td_header_process.h \
    ../pyscript/ctp_td_header_function.h \
    ../pyscript/ctp_td_header_process.h

SOURCES += ctptd.cpp

