#include "SoldadoEnemigo.h"
#include "Proyectil.h"
#include <cmath>
#include <cstdlib>


// Constructores
SoldadoEnemigo::SoldadoEnemigo()
    : Personaje(),
    rangoVision(80.0f),
    rangoAtaque(70.0f),
    cadenciaDisparo(2.0f),
    tiempoDesdeUltimoDisparo(0.0f),
    velocidadMovimiento(30.0f)
{
    esAliado = false;
    setSaludMax(90);
    setSalud(90);
    danoBase = 10;
}

SoldadoEnemigo::SoldadoEnemigo(float xParam,
                               float yParam,
                               int   saludMaxParam,
                               bool  esAliadoParam,
                               int   danoBaseParam,
                               float rangoVisionParam,
                               float rangoAtaqueParam,
                               float cadenciaDisparoParam,
                               float velocidadMovimientoParam)
    : Personaje(xParam, yParam, saludMaxParam, esAliadoParam, danoBaseParam),
    rangoVision(rangoVisionParam),
    rangoAtaque(rangoAtaqueParam),
    cadenciaDisparo(cadenciaDisparoParam),
    tiempoDesdeUltimoDisparo(0.0f),
    velocidadMovimiento(velocidadMovimientoParam)
{
}

namespace {
    float randEnRango(float minVal, float maxVal) {
        float t = std::rand() / static_cast<float>(RAND_MAX);
        return minVal + (maxVal - minVal) * t;
    }
}

// Getters
float SoldadoEnemigo::getRangoVision() const {
    return rangoVision;
}

float SoldadoEnemigo::getRangoAtaque() const {
    return rangoAtaque;
}

float SoldadoEnemigo::getCadenciaDisparo() const {
    return cadenciaDisparo;
}

float SoldadoEnemigo::getTiempoDesdeUltimoDisparo() const {
    return tiempoDesdeUltimoDisparo;
}

float SoldadoEnemigo::getVelocidadMovimiento() const {
    return velocidadMovimiento;
}

// Setters
void SoldadoEnemigo::setRangoVision(float _rangoVision) {
    if (_rangoVision < 0.0f) {
        _rangoVision = 0.0f;
    }
    rangoVision = _rangoVision;
}

void SoldadoEnemigo::setRangoAtaque(float _rangoAtaque) {
    if (_rangoAtaque < 0.0f) {
        _rangoAtaque = 0.0f;
    }
    rangoAtaque = _rangoAtaque;
}

void SoldadoEnemigo::setCadenciaDisparo(float _cadenciaDisparo) {
    if (_cadenciaDisparo < 0.0f) {
        _cadenciaDisparo = 0.0f;
    }
    cadenciaDisparo = _cadenciaDisparo;
}

void SoldadoEnemigo::setTiempoDesdeUltimoDisparo(float _tiempo) {
    if (_tiempo < 0.0f) {
        _tiempo = 0.0f;
    }
    tiempoDesdeUltimoDisparo = _tiempo;
}

void SoldadoEnemigo::setVelocidadMovimiento(float _velocidadMovimiento) {
    if (_velocidadMovimiento < 0.0f) {
        _velocidadMovimiento = 0.0f;
    }
    velocidadMovimiento = _velocidadMovimiento;
}

// Metodos
void SoldadoEnemigo::actualizarIA(Personaje* objetivo, float dt) {
    if (dt < 0.0f || objetivo == nullptr) return;
    if (salud <= 0) return;

    tiempoDesdeUltimoDisparo += dt;

    float dx = objetivo->getX() - x;
    float dy = objetivo->getY() - y;
    float dist = std::sqrt(dx * dx + dy * dy);

    if (dist > rangoVision) {
        vx = 0.0f;
        vy = 0.0f;
        return;
    }

    if (dist > rangoAtaque) {
        if (dist > 0.0f) {
            float dirX = dx / dist;
            float dirY = dy / dist;
            vx = dirX * velocidadMovimiento;
            vy = dirY * velocidadMovimiento;
            mover(dt);
        }
    } else {
        vx = 0.0f;
        vy = 0.0f;
    }
}


void SoldadoEnemigo::atacar(Personaje* objetivo) {
    if (objetivo == nullptr) {
        return;
    }
    if (salud <= 0) {
        return;
    }
    if (objetivo->getEsAliado() == esAliado) {
        return;
    }

    float dx = objetivo->getX() - x;
    float dy = objetivo->getY() - y;
    float dist = std::sqrt(dx * dx + dy * dy);
    if (dist > rangoAtaque) {
        return;
    }

    objetivo->recibirDano(danoBase);
}

Proyectil* SoldadoEnemigo::disparar(Personaje* objetivo, float velocidadInicial) {
    if (objetivo == nullptr) return nullptr;
    if (salud <= 0) return nullptr;
    if (objetivo->getEsAliado() == esAliado) return nullptr;

    float dx = objetivo->getX() - x;
    float dy = objetivo->getY() - y;
    float dist = std::sqrt(dx * dx + dy * dy);
    if (dist > rangoAtaque) return nullptr;

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

Proyectil* SoldadoEnemigo::dispararConError(Personaje* objetivo,
                                            float velocidadInicial,
                                            float errorMaxGrados)
{
    if (!objetivo) return nullptr;
    if (salud <= 0) return nullptr;
    if (objetivo->getEsAliado() == esAliado) return nullptr;

    float dx   = objetivo->getX() - x;
    float dy   = objetivo->getY() - y;
    float dist = std::sqrt(dx * dx + dy * dy);
    if (dist <= 0.0f)  return nullptr;
    if (dist > rangoAtaque) return nullptr;

    if (tiempoDesdeUltimoDisparo < cadenciaDisparo) return nullptr;
    if (velocidadInicial <= 0.0f) velocidadInicial = 1.0f;
    if (errorMaxGrados < 0.0f)   errorMaxGrados   = 0.0f;

    // Ángulo base hacia el comandante
    float angBase = std::atan2(dy, dx);

    // Error aleatorio
    float maxRad  = errorMaxGrados * 3.14159265f / 180.0f;
    float delta   = randEnRango(-maxRad, maxRad);
    float angulo  = angBase + delta;

    float vxProj = std::cos(angulo) * velocidadInicial;
    float vyProj = std::sin(angulo) * velocidadInicial;

    // 🔧 ALTURA DEL ARMA (unos píxeles por encima de los pies)
    const float ALTURA_ARMA = 35.0f;
    float yDisparo = y - ALTURA_ARMA;

    Proyectil* p = new Proyectil(x, yDisparo,
                                 vxProj, vyProj,
                                 danoBase,
                                 Proyectil::TIPO_BALA,
                                 esAliado);  // esAliado = false → proyectil enemigo

    tiempoDesdeUltimoDisparo = 0.0f;
    return p;
}
