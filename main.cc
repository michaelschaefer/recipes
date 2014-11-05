#include <QApplication>
#include <QLibraryInfo>
#include <QTranslator>
#include "mainwindow.hh"


int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName(MainWindow::ApplicationOrganization);
    QCoreApplication::setApplicationName(MainWindow::ApplicationName);

    QApplication application(argc, argv);

    QString locale;
    if (argc == 1) {
        locale = QLocale::system().name().mid(0, 2);
    } else {
        locale = QString(argv[1]);
    }

    // translate Qt framework
    QString path = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
    QTranslator qtTranslator;
    qtTranslator.load("qt_" + locale, path);
    application.installTranslator(&qtTranslator);

    // translate application
    QTranslator recipesTranslator;
    recipesTranslator.load(":/ts/recipes_" + locale);
    application.installTranslator(&recipesTranslator);

    MainWindow mainWindow;
    mainWindow.show();

    return application.exec();
}
