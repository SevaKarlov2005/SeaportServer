QT = core network

CONFIG += c++17 cmdline

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        administrationmodule.cpp \
        authorizationmodule.cpp \
        customsmodule.cpp \
        databasemanager.cpp \
        distributionmodule.cpp \
        loadingmodule.cpp \
        main.cpp \
        planningmodule.cpp \
        tcpserver.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += "D:/PostgreSQL/lib/libpq.lib" \
                                              "D:/LIB/OpenSSL/OpenSSL-Win64/lib/VC/x64/MD/libcrypto.lib" \
                                              "D:/LIB/OpenSSL/OpenSSL-Win64/lib/VC/x64/MD/libssl.lib"
else:win32:CONFIG(debug, debug|release): LIBS += "D:/PostgreSQL/lib/libpq.lib" \
                                                 "D:/LIB/OpenSSL/OpenSSL-Win64/lib/VC/x64/MD/libcrypto.lib" \
                                                 "D:/LIB/OpenSSL/OpenSSL-Win64/lib/VC/x64/MD/libssl.lib"

INCLUDEPATH += "D:/PostgreSQL/include" \
               "D:/LIB/OpenSSL/OpenSSL-Win64/include"


HEADERS += \
    administrationmodule.h \
    authorizationmodule.h \
    customsmodule.h \
    databasemanager.h \
    distributionmodule.h \
    loadingmodule.h \
    planningmodule.h \
    tcpserver.h
