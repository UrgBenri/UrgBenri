!contains( included_modules, $$PWD ) {
    included_modules += $$PWD
    QT += core widgets

    DEFINES  += LOG_CONSOLE
    DEPENDPATH += $$PWD
    INCLUDEPATH += $$PWD

    FORMS += \
        $$PWD/LogConsoleWidget.ui

    HEADERS += \
        $$PWD/LogConsoleWidget.h

    SOURCES += \
        $$PWD/LogConsoleWidget.cpp

    RESOURCES += \
        $$PWD/LogConsoleWidget.qrc
}

