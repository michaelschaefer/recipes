#-------------------------------------------------
#
# Project created by QtCreator 2014-09-27T12:40:34
#
#-------------------------------------------------

QT       += core gui printsupport sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = recipes
TEMPLATE = app
QMAKE_CXXFLAGS += -std=c++11


SOURCES += main.cc \
    mainwindow.cc \
    recipeedit.cc \
    flexiblelayout.cc \
    ingredientlistedit.cc \
    widgetinterface.cc \
    ingredientdialog.cc \
    ingredientedit.cc \
    preparationlistedit.cc \
    sectionedit.cc \
    preparationstepedit.cc \
    preparationstepdialog.cc \
    recipetabwidget.cc \
    recipedata.cc \
    exporter.cc \
    library.cc \
    database.cc \
    librarythread.cc \
    searchwidget.cc \
    searchfiltermodel.cc \
    searchfilterwidget.cc \
    settingsdialog.cc \
    settingstablibrary.cc \
    ftpmanager.cc \
    settingstabformat.cc \
    synchronizationdialog.cc

HEADERS  += mainwindow.hh \
    recipeedit.hh \
    flexiblelayout.hh \
    ingredientlistedit.hh \
    widgetinterface.hh \
    ingredientdialog.hh \
    ingredientedit.hh \
    preparationlistedit.hh \
    sectionedit.hh \
    preparationstepedit.hh \
    preparationstepdialog.hh \
    recipetabwidget.hh \
    recipedata.hh \
    exporter.hh \
    library.hh \
    database.hh \
    librarythread.hh \
    searchwidget.hh \
    searchfiltermodel.hh \
    searchfilterwidget.hh \
    settingsdialog.hh \
    settingstablibrary.hh \
    ftpmanager.hh \
    settingstabformat.hh \
    synchronizationdialog.hh

RESOURCES += recipes.qrc

TRANSLATIONS += ts/recipes_de.ts

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../local/build/qtftp-Desktop-Debug/lib/release/ -lQt5Ftp
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../local/build/qtftp-Desktop-Debug/lib/debug/ -lQt5Ftp
else:unix: LIBS += -L$$PWD/../../local/build/qtftp-Desktop-Debug/lib/ -lQt5Ftp

INCLUDEPATH += $$PWD/../../local/build/qtftp-Desktop-Debug/include
DEPENDPATH += $$PWD/../../local/build/qtftp-Desktop-Debug/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../local/build/qtftp-Desktop-Debug/lib/release/libQt5Ftp.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../local/build/qtftp-Desktop-Debug/lib/debug/libQt5Ftp.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../local/build/qtftp-Desktop-Debug/lib/release/Qt5Ftp.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../local/build/qtftp-Desktop-Debug/lib/debug/Qt5Ftp.lib
else:unix: PRE_TARGETDEPS += $$PWD/../../local/build/qtftp-Desktop-Debug/lib/libQt5Ftp.a
