QT       += core gui network widgets opengl xml

TARGET = UrgBenriPlus
TEMPLATE = app

CONFIG += c++11

BUILD_NUMBER = $$system(hg parent --template \"{rev}\")

APP_REVISION = $${BUILD_NUMBER}
APP_VERSION_DATE = $$system(hg parent --template \"{date|shortdate}\")

APP_VERSION = 2.2.0

macx:CONFIG += x68 ppc x86_64 ppc64


VERSION = $$APP_VERSION
DEFINES += \
    APP_VERSION=\\\"$$APP_VERSION\\\" \
    APP_REVISION=\\\"$$APP_REVISION\\\" \
    APP_VERSION_DATE=\\\"$$APP_VERSION_DATE\\\"

QMAKE_TARGET_COMPANY = "Hokuyo Automatic Co.,LTD."
QMAKE_TARGET_PRODUCT = $${TARGET}
QMAKE_TARGET_DESCRIPTION = "Area Sensors Series setting tool"
QMAKE_TARGET_COPYRIGHT = "Copyright (C) 2013 Hokuyo Automatic Co.,LTD."
PACKAGE_DOMAIN = "http://hokuyo-aut.jp"
PACKAGE_VERSION = $$APP_VERSION

CONFIG(release):DESTDIR = $$PWD/../bin
CONFIG(debug, debug|release):DESTDIR = $$PWD/../bin

include(UrgBenri.pri)

CONFIG += exceptions rtti

DEPENDPATH += .

win32:DEFINES += _CRT_SECURE_NO_WARNINGS _SCL_SECURE_NO_WARNINGS

SOURCES += $$PWD/main.cpp

win32:RC_FILE += $$PWD/UrgBenri.rc
macx:ICON = $$PWD/icons/UrgBenri.icns

TRANSLATIONS = $$PWD/i18n/UrgBenri_fr.ts \
	$$PWD/i18n/UrgBenri_en.ts \
	$$PWD/i18n/UrgBenri_ja.ts

win32 {
    CONFIG += embed_manifest_exe
    QMAKE_LFLAGS_WINDOWS += /MANIFESTUAC:level=\'asInvoker\'
}

