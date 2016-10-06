!contains( included_modules, $$PWD ) {
    included_modules += $$PWD
    QT += core gui

    DEPENDPATH += $$PWD
    INCLUDEPATH += $$PWD

    SOURCES += \
            $$PWD/QuadSplitter.cpp

    HEADERS  += \
            $$PWD/QuadSplitter.h
}
