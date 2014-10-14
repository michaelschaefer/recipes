#ifndef LIBRARYTHREAD_HH
#define LIBRARYTHREAD_HH


#include <QThread>
#include "library.hh"


class LibraryThread : public QThread {

    Q_OBJECT     


public:

    enum LibraryFunction {
        Rebuild = 1,
        SetPathList = 2,
        Update = 4
    };

    LibraryThread(Library* library, LibraryFunction libraryFunction, QObject* parent = 0);
    void run();
    void setPathList(QStringList& pathList);


private:

    Library* m_library;
    LibraryFunction m_libraryFunction;
    QStringList m_pathList;

};

#endif // LIBRARYTHREAD_HH
