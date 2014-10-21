#include "librarythread.hh"


LibraryThread::LibraryThread(Library* library, LibraryFunction libraryFunction, QObject* parent)
    : QThread(parent) {
    m_library = library;
    m_libraryFunction = libraryFunction;
}


void LibraryThread::run() {
    switch (m_libraryFunction) {
    case Rebuild:
        m_library->rebuild();
        break;
    case Update:
        m_library->update();
    }
}
