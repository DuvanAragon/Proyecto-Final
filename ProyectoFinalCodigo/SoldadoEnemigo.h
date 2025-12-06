#ifndef SOLDADO_ENEMIGO_H
#define SOLDADO_ENEMIGO_H

#include "Personaje.h"

class Proyectil;

class SoldadoEnemigo : public Personaje {
private:
    float rangoVision;
    float rangoAtaque;
    float cadenciaDisparo;
    float tiempoDesdeUltimoDisparo;
    float velocidadMovimiento;

public:
    // Constructores
    SoldadoEnemigo();
    SoldadoEnemigo(float x,
                   float y,
                   int   saludMax,
                   bool  esAliado,
                   int   danoBase,
                   float rangoVision,
                   float rangoAtaque,
                   float cadenciaDisparo,
                   float velocidadMovimiento);

    // Getters
    float getRangoVision() const;
    float getRangoAtaque() const;
    float getCadenciaDisparo() const;
    float getTiempoDesdeUltimoDisparo() const;
    float getVelocidadMovimiento() const;


    // Setters
    void setRangoVision(float _rangoVision);
    void setRangoAtaque(float _rangoAtaque);
    void setCadenciaDisparo(float _cadenciaDisparo);
    void setTiempoDesdeUltimoDisparo(float _tiempo);
    void setVelocidadMovimiento(float _velocidadMovimiento);

    // Metodos
    void actualizarIA(Personaje* objetivo, float dt);
    void atacar(Personaje* objetivo) override;
    Proyectil* disparar(Personaje* objetivo, float velocidadInicial);
};

#endif // SOLDADO_ENEMIGO_H
