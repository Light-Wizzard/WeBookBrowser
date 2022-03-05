TEMPLATE = app
TARGET = "WeBookBrowser"
QT_VERSION = 6.2.2
CONFIG += qt
#!versionAtLeast(QT_VERSION, 6.2.1):error("Use at least Qt version 6.2.1")
QT       += core network gui widgets webenginewidgets webenginecore webchannel
#DEFINES  += MY_SENSORS
#CONFIG *= strict_c++
CONFIG *= "c++17"
#CONFIG *= "c++2a"
#CONFIG *= "c++latest"

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    src/BookmarkMenu.h \
    src/BookmarkTreeWidgetView.h \
    src/DownloadList.h \
    src/DownloadManagerWidget.h \
    src/HelpView.h \
    src/MainWindow.h \
    src/WebPage.h \
    src/WebView.h \
    src/XbelMenuReader.h \
    src/XbelTreeReader.h \
    src/XbelTreeWriter.h \
    src/MyLanguageData.h \
    src/MyLanguageModel.h \
    src/MyOrgSettings.h \
    src/MySystemInformation.h \
    src/qaesencryption.h

SOURCES += \
    src/main.cpp \
    src/MainWindow.cpp \
    src/BookmarkMenu.cpp \
    src/BookmarkTreeWidgetView.cpp \
    src/DownloadList.cpp \
    src/DownloadManagerWidget.cpp \
    src/HelpView.cpp \
    src/WebPage.cpp \
    src/WebView.cpp \
    src/XbelMenuReader.cpp \
    src/XbelTreeReader.cpp \
    src/XbelTreeWriter.cpp \
    src/MyLanguageData.cpp \
    src/MyLanguageModel.cpp \
    src/MyOrgSettings.cpp \
    src/MySystemInformation.cpp \
    src/qaesencryption.cpp


FORMS += src/MainWindow.ui \
    src/certificateerrordialog.ui \
    src/passworddialog.ui

#
# Translations
#
CONFIG += localize_deployment
SYMBIAN_SUPPORTED_LANGUAGES += zh_CN
SYMBIAN_LANG.zh_CN = 31
CONFIG += lrelease
CONFIG += embed_translations
LRELEASE_DIR=./translations
QM_FILES_RESOURCE_PREFIX=./translations
# ar,de,en,es,fr,it,ja,no,ru,sv,zh_CN
TRANSLATIONS += translations/WeBookBrowser_ar.ts \
                translations/WeBookBrowser_de.ts \
                translations/WeBookBrowser_en.ts \
                translations/WeBookBrowser_es.ts \
                translations/WeBookBrowser_fr.ts \
                translations/WeBookBrowser_it.ts \
                translations/WeBookBrowser_ja.ts \
                translations/WeBookBrowser_no.ts \
                translations/WeBookBrowser_ru.ts \
                translations/WeBookBrowser_sv.ts \
                translations/WeBookBrowser_zh_CN.ts

RESOURCES += WeBookBrowser.qrc
DISTFILES   += README.md
###############################################################################
#
# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
#DEFINES += QT_DEPRECATED_WARNINGS
#
# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
#
#
macos:QMAKE_INFO_PLIST = macos/Info.plist
ios:QMAKE_INFO_PLIST   = ios/Info.plist
#-------------------------------------------------------------------------------------
#contains(QMAKE_HOST.arch, x86_64) { #x64
#    BUILD_ARCH = x64
#} else { #x32
#    BUILD_ARCH += x32
#}
#CONFIG(release, debug|release) {
#    BUILD_TYPE = release
#} else {
#    BUILD_TYPE = debug
#}
#GENF_ROOT   = $${BUILD_ARCH}/_output
#BIN_OUTPUT  = $${GENF_ROOT}/_bin
#DESTDIR     = $${BIN_OUTPUT}/$${BUILD_TYPE}
#OBJECTS_DIR = $${GENF_ROOT}/$${TARGET}/$${BUILD_TYPE}/_build
#MOC_DIR     = $${GENF_ROOT}/$${TARGET}/$${BUILD_TYPE}/_moc
#UI_DIR      = $${GENF_ROOT}/$${TARGET}/$${BUILD_TYPE}/_ui
#RCC_DIR     = $${GENF_ROOT}/$${TARGET}/$${BUILD_TYPE}/_rc
#-------------------------------------------------------------------------------------
#
#Release:DESTDIR     = release
#Release:OBJECTS_DIR = release/.obj
#Release:MOC_DIR     = release/.moc
#Release:RCC_DIR     = release/.rcc
#Release:UI_DIR      = release/.ui

#Debug:DESTDIR     = debug
#Debug:OBJECTS_DIR = debug/.obj
#Debug:MOC_DIR     = debug/.moc
#Debug:RCC_DIR     = debug/.rcc
#Debug:UI_DIR      = debug/.ui


win32-g++{
    contains(QMAKE_HOST.arch, x86_64) { #x64
        DEFINES += MINGW_X64
    } else { #x32
        DEFINES += MINGW_X32
    }

    CONFIG(release, debug|release) {
        #release
        QMAKE_CXXFLAGS += -std=c++0x -O2 -Os -msse2 -ffp-contract=fast -fpic
    }
    else {
        #debug
        DEFINES += _DEBUG
        QMAKE_CXXFLAGS += -std=c++0x -O0 -g3 -msse2 -fpic
    }
}

#
unix {
    isEmpty(PREFIX) {
        PREFIX = /usr
    }
    target.path         = "$${PREFIX}/bin"
    shortcutfiles.files = "usr/share/applications/$${TARGET}.desktop"
    shortcutfiles.path  = usr/share/applications
    data.files         += "usr/share/icons/hicolor/48x48/apps/$${TARGET}.png"
    data.path           = usr/share/pixmaps
    INSTALLS           += shortcutfiles
    INSTALLS           += data
    INSTALLS           += target
    macx {
        RC_FILE = macos/Icon.icns
    }
} else {
    isEmpty(PREFIX) {
        PREFIX = /usr
    }
    target.path         = "$${PREFIX}/bin"
    shortcutfiles.files = "usr/share/applications/$${TARGET}.desktop"
    shortcutfiles.path  = usr/share/applications
    data.files         += "usr/share/icons/hicolor/48x48/apps/$${TARGET}.png"
    data.path           = usr/share/pixmaps
    INSTALLS           += shortcutfiles
    INSTALLS           += data
    INSTALLS           += target
}
################################ End of File ##################################
