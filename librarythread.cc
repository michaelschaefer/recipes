#include "librarythread.hh"


LibraryThread::LibraryThread(Library* library, LibraryFunction libraryFunction, QObject* parent)
    : QThread(parent) {
    m_library = library;
    m_libraryFunction = libraryFunction;
}


void LibraryThread::run() {
    switch (m_libraryFunction) {
    case Rebuild: {
        m_library->rebuild();
        break;
    }
    case SetPathList: {
        m_library->setPathList(m_pathList);
        break;
    }
    case Update: {
        m_library->update();
        break;
    }
    }
}


void LibraryThread::setPathList(QStringList& pathList) {
    m_pathList = pathList;
}
