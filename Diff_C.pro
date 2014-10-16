TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
QMAKE_CXXFLAGS += -std=c++11

QT       += core gui
SOURCES += main.cpp \
    util/util.cpp \
    memcached/encaplibmemcache.cpp \
    extractUrl/extractUrl.cpp \
    getweb/getweb.cpp \
    diff/diff.cpp


HEADERS += \
    util/util.h \
    memcached/encaplibmemcache.h \
    extractUrl/extractUrl.h \
    getweb/getweb.h \
    getweb/my_atomic.h \
    diff/diff.h



INCLUDEPATH += /usr/local/libmemcached/include \
               /usr/local/libmemcached/include/libmemcached




LIBS += /usr/local/libmemcached/lib/libmemcached.so \
        -lpthread
#LIBS += /usr/local/libmemcached/lib/libmemcached.so \
        #/usr/local/libmemcached/lib/libhashkit.so \
        #/usr/local/libmemcached/lib/libmemcachedutil.so
