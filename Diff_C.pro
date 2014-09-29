TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
QMAKE_CXXFLAGS += -std=c++11

SOURCES += main.cpp \
    util/util.cpp \
    memcached/encaplibmemcache.cpp \
    extractUrl/extractUrl.cpp

HEADERS += \
    util/util.h \
    memcached/encaplibmemcache.h \
    extractUrl/getDifferenceList.h \
    extractUrl/extractUrl.h



INCLUDEPATH += /usr/local/libmemcached/include \
               /usr/local/libmemcached/include/libmemcached




LIBS += /usr/local/libmemcached/lib/libmemcached.so
#LIBS += /usr/local/libmemcached/lib/libmemcached.so \
        #/usr/local/libmemcached/lib/libhashkit.so \
        #/usr/local/libmemcached/lib/libmemcachedutil.so
