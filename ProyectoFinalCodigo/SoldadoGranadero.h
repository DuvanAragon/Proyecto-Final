#ifndef SOLDADO_GRANADERO_H
#define SOLDADO_GRANADERO_H

#include "Personaje.h"

class Proyectil;

class SoldadoGranadero : public Personaje {
private:
    float rangoVision;
    float rangoLanzamiento;
    float cooldownGranada;
    float tiempoDesdeUltGranada;
    float velocidadMovimiento;
    float radioExplosion;
    bool lanzoEnEsteFrame;

public:
    // Constructores
    SoldadoGranadero();
    SoldadoGranadero(float x,
                     float y,
                     int   saludMax,
                     bool  esAliado,
                     int   danoBase,
                     float rangoVision,
                     float rangoLanzamiento,
                     float cooldownGranada,
                     float velocidadMovimiento,
                     float radioExplosion);

    // Getters
    float getRangoVision() const;
    float getRangoLanzamiento() const;
    float getCooldownGranada() const;
    float getTiempoDesdeUltGranada() const;
    float getVelocidadMovimiento() const;
    float getRadioExplosion() const;
    bool getLanzoEnEsteFrame() const { return lanzoEnEsteFrame; }


    // Setters
    void setRangoVision(float _rangoVision);
    void setRangoLanzamiento(float _rangoLanzamiento);
    void setCooldownGranada(float _cooldownGranada);
    void setTiempoDesdeUltGranada(float _tiempo);
    void setVelocidadMovimiento(float _velocidadMovimiento);
    void setRadioExplosion(float _radioExplosion);

    // Metodos
    void actualizarIA(Personaje* objetivo, float dt);
    void resetLanzoEnEsteFrame() { lanzoEnEsteFrame = false; }
    Proyectil* lanzarGranada(Personaje* objetivo, float velocidadInicial);
    Proyectil* lanzarGranadaConError(Personaje* objetivo,
                                     float velocidadInicial,
                                     float errorMaxDistancia);

};

#endif // SOLDADO_GRANADERO_H
