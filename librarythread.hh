#ifndef LIBRARYTHREAD_HH
#define LIBRARYTHREAD_HH


#include <QThread>
#include "library.hh"


class LibraryThread : public QThread {

    Q_OBJECT     


public:

    enum LibraryFunction {
        Export,
        Rebuild,
        Update
    };

    LibraryThread(Library* library, LibraryFunction libraryFunction, QObject* parent = 0);

    void run();
    void setExportPath(QString exportPath);


private:

    Library* m_library;
    LibraryFunction m_libraryFunction;
    QString m_exportPath;

};

#endif // LIBRARYTHREAD_HH
