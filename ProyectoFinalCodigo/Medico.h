#ifndef MEDICO_H
#define MEDICO_H

#include "Personaje.h"

class Medico : public Personaje {
private:
    int   cantidadCura;
    float radioCuracion;
    float cooldownCuracion;
    float tiempoDesdeUltimaCuracion;

public:
    // Constructores
    Medico();
    Medico(float x,
           float y,
           int   saludMax,
           bool  esAliado,
           int   danoBase,
           int   cantidadCura,
           float radioCuracion,
           float cooldownCuracion);

    // Getters
    int   getCantidadCura() const;

    float getRadioCuracion() const;
    float getCooldownCuracion() const;
    float getTiempoDesdeUltimaCuracion() const;

    // Setters
    void setCantidadCura(int _cantidadCura);
    void setRadioCuracion(float _radioCuracion);
    void setCooldownCuracion(float _cooldownCuracion);
    void setTiempoDesdeUltimaCuracion(float _tiempo);

    // Metodos
    void actualizar(float dt);
    Personaje* seleccionarAliadoMasHerido(Personaje** aliados, int cantidadAliados);
    void curar(Personaje* objetivo);
};

#endif // MEDICO_H
