#ifndef LIBRARYTHREAD_HH
#define LIBRARYTHREAD_HH


#include <QThread>
#include "library.hh"


class LibraryThread : public QThread {

    Q_OBJECT     


public:

    enum LibraryFunction {
        Rebuild = 1,
        Update = 2
    };

    LibraryThread(Library* library, LibraryFunction libraryFunction, QObject* parent = 0);
    void run();


private:

    Library* m_library;
    LibraryFunction m_libraryFunction;

};

#endif // LIBRARYTHREAD_HH
