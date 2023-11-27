
# QtYaml

**Reader/Writter yaml files.**


## Working architecture

## Prerequisits
>```bash
> mkdir myproject;
> cd myproject;
> git@github.com:flaviomarcio/qtyaml.git
>```
>Check examples in qtyaml/examples


## CMake Build information

>```
>## initial CMake parameters 
>
>-GNinja
>-DCMAKE_BUILD_TYPE:STRING=Debug
>-DCMAKE_PROJECT_INCLUDE_BEFORE:PATH=%{IDE:ResourcePath}/package-manager/auto-setup.cmake
>-DQT_QMAKE_EXECUTABLE:STRING=%{Qt:qmakeExecutable}
>-DCMAKE_PREFIX_PATH:STRING=%{Qt:QT_INSTALL_PREFIX}
>-DCMAKE_C_COMPILER:STRING=%{Compiler:Executable:C}
>-DCMAKE_CXX_COMPILER:STRING=%{Compiler:Executable:Cxx}
>-DCMAKE_INSTALL_PREFIX=~/build/qcrosscache/install/Debug
>```

>```bash
> cd qtyaml
> mkdir build;
> cd build;
> cmake ..
> make;
> make install;
>```

## QMake Build information

>```bash
> cd qtyaml
> qmake qtyaml.pro
> make;
> make install;
> ls -l;
>```

## Configure QMake project

>```c++
>CONFIG += c++17
>CONFIG += console
>CONFIG += silent
>CONFIG -= debug_and_release
>QT += core
>
>TEMPLATE = app
>TARGET = demo
>
>include($$PWD/../../../qtyaml/qtyaml.pri)
>
>HEADERS += \
>    ...
>
>SOURCES += \
>    ...
>
>RESOURCES += \
>    ...
>```

## Implementation method for create connection using connection manager

>Check example in qtyaml/examples
>```c++
> 
>//main implementation
>#include <QCoreApplication>
>#include <QDebug>
>
>int main(int argc, char *argv[])
>{
>}
>```
