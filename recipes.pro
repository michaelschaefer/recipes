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
    settingstabformat.cc \
    synchronizationdialog.cc \
    synchronizer.cc \
    settingsmanager.cc

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
    settingstabformat.hh \
    synchronizationdialog.hh \
    synchronizer.hh \
    settingsmanager.hh

RESOURCES += recipes.qrc

TRANSLATIONS += ts/recipes_de.ts
