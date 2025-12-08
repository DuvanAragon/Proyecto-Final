QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Canonpesado.cpp \
    Comandante.cpp \
    ControladorJuego.cpp \
    Escuadron.cpp \
    Francotirador.cpp \
    Fusilero.cpp \
    Medico.cpp \
    Nivel.cpp \
    Personaje.cpp \
    PersonajeGrafico.cpp \
    Proyectil.cpp \
    ProyectilGrafico.cpp \
    SoldadoEnemigo.cpp \
    SoldadoGranadero.cpp \
    main.cpp \
    mainwindow.cpp \
    menuwindow.cpp \
    nivel2window.cpp \
    personajelateralgrafico.cpp

HEADERS += \
    Canonpesado.h \
    Comandante.h \
    ControladorJuego.h \
    Escuadron.h \
    Francotirador.h \
    Fusilero.h \
    Medico.h \
    Nivel.h \
    Personaje.h \
    PersonajeGrafico.h \
    Proyectil.h \
    ProyectilGrafico.h \
    SoldadoEnemigo.h \
    SoldadoGranadero.h \
    mainwindow.h \
    menuwindow.h \
    nivel2window.h \
    personajelateralgrafico.h

FORMS += \
    mainwindow.ui \
    menuwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

QT += widgets multimedia

RESOURCES += \
    Sonidos.qrc \
    Sprires.qrc
