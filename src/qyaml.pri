QT+=core
QT-=gui

INCLUDEPATH+=$$PWD/includes


HEADERS+= \
    $$PWD/qyaml.h \
    $$PWD/qyaml_document.h \
    $$PWD/qyaml_global.h

SOURCES+= \
    $$PWD/qyaml_document.cpp
