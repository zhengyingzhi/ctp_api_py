######################################################################
# Automatically generated by qmake (3.0) Fri Apr 22 14:06:12 2016
######################################################################
TARGET = kdmd

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
        DESTDIR = ../../build/debug
} else {
        DEFINES += NDEBUG
        DESTDIR = ../../build/release
}

QMAKE_CFLAGS += -std=c99
QMAKE_CXXFLAGS += -std=c++0x -fPIC -Wno-unused-parameter -fvisibility=hidden -fvisibility-inlines-hidden

DEFINES += __STDC_FORMAT_MACROS


INCLUDEPATH += ../api/kdmd
# INCLUDEPATH += /usr/local/include/boost/include
# INCLUDEPATH += /opt/python3.5/include/python3.5m
INCLUDEPATH += ../../thirdparty/boost_1_66_0
INCLUDEPATH += ../../thirdparty/python35/linux_include

LIBS += -lpthread -lrt

BOOST_DIR = ../../thirdparty/boost_1_66_0/linux64

#LIBS += -L$${BOOST_DIR} -lboost_thread -lboost_date_time -lboost_system -lboost_chrono -lboost_atomic -lboost_locale
LIBS += $${BOOST_DIR}/libboost_python.a
LIBS += $${BOOST_DIR}/libboost_thread.a
LIBS += $${BOOST_DIR}/libboost_date_time.a
LIBS += $${BOOST_DIR}/libboost_system.a
LIBS += $${BOOST_DIR}/libboost_chrono.a
LIBS += $${BOOST_DIR}/libboost_atomic.a
LIBS += $${BOOST_DIR}/libboost_locale.a
LIBS += -L../../thirdparty/python35/linux64 -lpython3.5m
LIBS += -L../api/kdmd/linux64 -lKDMdUserApi


# Input
HEADERS += kdmd.h \
    ../api/kdmd/KDCommonDef.h \
    ../api/kdmd/KDFutureMarket.h \
    ../api/kdmd/KDMDApiStruct.h \
    ../api/kdmd/KDMdUserApi.h \
    ../api/kdmd/KDStockMarket.h

SOURCES += kdmd.cpp

