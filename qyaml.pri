QTREFORCE_QYAML=true
DEFINES+=QTREFORCE_QYAML

include($$PWD/src/qyaml.pri)
QT-=gui

INCLUDEPATH += $$PWD/includes

HEADERS+= \
    $$PWD/includes/QtYaml
