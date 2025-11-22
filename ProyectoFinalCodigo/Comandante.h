#ifndef COMANDANTE_H
#define COMANDANTE_H

#include "Personaje.h"

class Proyectil;

class Comandante : public Personaje {
private:
    float radioOrden;
    float bonoDanioAliados;

public:
    // Constructores
    Comandante();
    Comandante(float x,
               float y,
               int   saludMax,
               bool  esAliado,
               int   danoBase,
               float radioOrden,
               float bonoDanioAliados);

    // Getters
    float getRadioOrden() const;
    float getBonoDanioAliados() const;

    // Setters
    void setRadioOrden(float _radioOrden);
    void setBonoDanioAliados(float _bonoDanioAliados);

    // Metodos
    void darOrdenAtacar(Personaje* aliado, Personaje* objetivo);
    Proyectil* disparar(Personaje* objetivo, float velocidadInicial);
};

#endif // COMANDANTE_H
