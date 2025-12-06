#ifndef CANONPESADO_H
#define CANONPESADO_H

#include "Personaje.h"

class Proyectil;

class CanonPesado : public Personaje {
private:
    int   estadoCanon;
    float cadenciaBase;
    float tiempoDesdeUltimoDisparo;

public:
    // Constructores
    CanonPesado();
    CanonPesado(float x,
                float y,
                int   saludMax,
                bool  esAliado,
                int   danoBase,
                int   estadoInicial,
                float cadenciaBase);

    // Getters
    int   getEstadoCanon() const;
    float getCadenciaBase() const;
    float getTiempoDesdeUltimoDisparo() const;
    int   getVida() const;

    // Setters
    void setEstadoCanon(int _estadoCanon);
    void setCadenciaBase(float _cadenciaBase);
    void setTiempoDesdeUltimoDisparo(float _tiempo);
    void setVida(int _vida);

    // Metodos
    void actualizarEstado(float dt);
    Proyectil* disparar(Personaje* objetivo, float velocidadInicial);
};

#endif // CANONPESADO_H
