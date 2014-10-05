#-------------------------------------------------
#
# Project created by QtCreator 2014-09-27T12:40:34
#
#-------------------------------------------------

QT       += core gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = recipes
TEMPLATE = app


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
    exportwidget.cc \
    recipedata.cc

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
    exportwidget.hh \
    recipedata.hh

RESOURCES += \
    recipes.qrc
