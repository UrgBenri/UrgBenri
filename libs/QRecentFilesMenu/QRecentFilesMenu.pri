!contains( included_modules, $$PWD ) {
    included_modules += $$PWD
    QT       += core gui

    DEPENDPATH += "$$PWD/src"
    INCLUDEPATH += "$$PWD/src"

    SOURCES += $$PWD/src/QRecentFilesMenu.cpp

    HEADERS  += $$PWD/src/QRecentFilesMenu.h

    TRANSLATIONS = $$PWD/i18n/QRecentFilesMenu_fr.ts \
            $$PWD/i18n/QRecentFilesMenu_en.ts \
            $$PWD/i18n/QRecentFilesMenu_ja.ts

    RESOURCES += $$PWD/QRecentFilesMenu.qrc
}
