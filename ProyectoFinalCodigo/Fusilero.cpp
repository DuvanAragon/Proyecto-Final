#include "Fusilero.h"
#include "Proyectil.h"
#include <cmath>

// Constructores
Fusilero::Fusilero()
    : Personaje(),
    alcanceDisparo(200.0f),
    cadenciaDisparo(1.0f),
    tiempoDesdeUltimoDisparo(0.0f)
{
    esAliado = true;
    setSaludMax(100);
    setSalud(100);
    danoBase = 15;
}

Fusilero::Fusilero(float xParam,
                   float yParam,
                   int   saludMaxParam,
                   bool  esAliadoParam,
                   int   danoBaseParam,
                   float alcanceDisparoParam,
                   float cadenciaDisparoParam)
    : Personaje(xParam, yParam, saludMaxParam, esAliadoParam, danoBaseParam),
    alcanceDisparo(alcanceDisparoParam),
    cadenciaDisparo(cadenciaDisparoParam),
    tiempoDesdeUltimoDisparo(0.0f)
{
}

// Getters
float Fusilero::getAlcanceDisparo() const {
    return alcanceDisparo;
}

float Fusilero::getCadenciaDisparo() const {
    return cadenciaDisparo;
}

float Fusilero::getTiempoDesdeUltimoDisparo() const {
    return tiempoDesdeUltimoDisparo;
}

// Setters
void Fusilero::setAlcanceDisparo(float _alcanceDisparo) {
    if (_alcanceDisparo < 0.0f) {
        _alcanceDisparo = 0.0f;
    }
    alcanceDisparo = _alcanceDisparo;
}

void Fusilero::setCadenciaDisparo(float _cadenciaDisparo) {
    if (_cadenciaDisparo < 0.0f) {
        _cadenciaDisparo = 0.0f;
    }
    cadenciaDisparo = _cadenciaDisparo;
}

void Fusilero::setTiempoDesdeUltimoDisparo(float _tiempo) {
    if (_tiempo < 0.0f) {
        _tiempo = 0.0f;
    }
    tiempoDesdeUltimoDisparo = _tiempo;
}

// Metodos
void Fusilero::actualizar(float dt) {
    if (dt < 0.0f) {
        return;
    }
    tiempoDesdeUltimoDisparo += dt;
}

void Fusilero::atacar(Personaje* objetivo) {
    if (objetivo == nullptr) {
        return;
    }
    if (salud <= 0) {
        return;
    }
    if (objetivo->getEsAliado() == esAliado) {
        return;
    }
    if (objetivo->getSalud() <= 0) {
        return;
    }

    float dx = objetivo->getX() - x;
    float dy = objetivo->getY() - y;
    float dist = std::sqrt(dx * dx + dy * dy);
    if (dist > alcanceDisparo) {
        return;
    }

    if (tiempoDesdeUltimoDisparo < cadenciaDisparo) {
        return;
    }

    objetivo->recibirDano(danoBase);
    tiempoDesdeUltimoDisparo = 0.0f;
}

Proyectil* Fusilero::disparar(Personaje* objetivo, float velocidadInicial) {
    if (objetivo == nullptr) return nullptr;
    if (salud <= 0) return nullptr;
    if (objetivo->getEsAliado() == esAliado) return nullptr;

    float dx = objetivo->getX() - x;
    float dy = objetivo->getY() - y;
    float dist = std::sqrt(dx * dx + dy * dy);
    if (dist > alcanceDisparo) return nullptr;

    if (tiempoDesdeUltimoDisparo < cadenciaDisparo) return nullptr;
    if (velocidadInicial <= 0.0f) velocidadInicial = 1.0f;

    float dirX = dx / dist;
    float dirY = dy / dist;

    float vxProj = dirX * velocidadInicial;
    float vyProj = dirY * velocidadInicial;

    Proyectil* p = new Proyectil(x, y, vxProj, vyProj, danoBase,
                                 Proyectil::TIPO_BALA, esAliado);
    tiempoDesdeUltimoDisparo = 0.0f;
    return p;
}
