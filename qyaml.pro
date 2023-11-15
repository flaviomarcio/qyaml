CONFIG -= debug_and_release
CONFIG += c++17
CONFIG += silent

TARGET = QYaml
TEMPLATE = lib

include($$PWD/src/qyaml.pri)

INCLUDEPATH += $$PWD/includes
