#ifndef FUSILERO_H
#define FUSILERO_H

#include "Personaje.h"

class Proyectil;

class Fusilero : public Personaje {
private:
    float alcanceDisparo;
    float cadenciaDisparo;
    float tiempoDesdeUltimoDisparo;

public:
    // Constructores
    Fusilero();
    Fusilero(float x,
             float y,
             int   saludMax,
             bool  esAliado,
             int   danoBase,
             float alcanceDisparo,
             float cadenciaDisparo);

    // Getters
    float getAlcanceDisparo() const;
    float getCadenciaDisparo() const;
    float getTiempoDesdeUltimoDisparo() const;


    // Setters
    void setAlcanceDisparo(float _alcanceDisparo);
    void setCadenciaDisparo(float _cadenciaDisparo);
    void setTiempoDesdeUltimoDisparo(float _tiempo);

    // Metodos
    void actualizar(float dt)override;
    void atacar(Personaje* objetivo) override;
    Proyectil* disparar(Personaje* objetivo, float velocidadInicial);
};

#endif // FUSILERO_H
