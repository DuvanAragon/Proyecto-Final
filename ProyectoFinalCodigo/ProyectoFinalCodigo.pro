QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    canonpesado.cpp \
    comandante.cpp \
    controladorjuego.cpp \
    escuadron.cpp \
    francotirador.cpp \
    fusilero.cpp \
    main.cpp \
    mainwindow.cpp \
    medico.cpp \
    nivel.cpp \
    personaje.cpp \
    proyectil.cpp \
    soldadoenemigo.cpp \
    soldadogranadero.cpp

HEADERS += \
    canonpesado.h \
    comandante.h \
    controladorjuego.h \
    escuadron.h \
    francotirador.h \
    fusilero.h \
    mainwindow.h \
    medico.h \
    nivel.h \
    personaje.h \
    proyectil.h \
    soldadoenemigo.h \
    soldadogranadero.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
