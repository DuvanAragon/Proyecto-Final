#ifndef FRANCOTIRADOR_H
#define FRANCOTIRADOR_H

#include "Personaje.h"

class Proyectil;

class Francotirador : public Personaje {
private:
    bool apuntando;
    float tiempoApuntando;
    float tiempoNecesario;
    Personaje* objetivoActual;

public:
    // Constructor
    Francotirador();

    // Getters
    bool getApuntando() const;

    // Metodos
    void comenzarApuntar(Personaje* objetivo);
    bool listoParaDisparar() const;
    void actualizar(float dt);
    Proyectil* disparar(Personaje* objetivo, float velocidadInicial);
};

#endif
