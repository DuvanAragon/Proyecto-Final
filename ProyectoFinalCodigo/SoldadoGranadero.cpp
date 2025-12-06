#include "SoldadoGranadero.h"
#include "Proyectil.h"
#include <cmath>

// Constructores
SoldadoGranadero::SoldadoGranadero()
    : Personaje(),
    rangoVision(150.0f),
    rangoLanzamiento(150.0f),
    cooldownGranada(8.0f),
    tiempoDesdeUltGranada(0.0f),
    velocidadMovimiento(30.0f),
    radioExplosion(40.0f)
{
    esAliado = false;
    setSaludMax(90);
    setSalud(90);
    danoBase = 25;
}

SoldadoGranadero::SoldadoGranadero(float xParam,
                                   float yParam,
                                   int   saludMaxParam,
                                   bool  esAliadoParam,
                                   int   danoBaseParam,
                                   float rangoVisionParam,
                                   float rangoLanzamientoParam,
                                   float cooldownGranadaParam,
                                   float velocidadMovimientoParam,
                                   float radioExplosionParam)
    : Personaje(xParam, yParam, saludMaxParam, esAliadoParam, danoBaseParam),
    rangoVision(rangoVisionParam),
    rangoLanzamiento(rangoLanzamientoParam),
    cooldownGranada(cooldownGranadaParam),
    tiempoDesdeUltGranada(0.0f),
    velocidadMovimiento(velocidadMovimientoParam),
    radioExplosion(radioExplosionParam)
{
}

// Getters
float SoldadoGranadero::getRangoVision() const {
    return rangoVision;
}

float SoldadoGranadero::getRangoLanzamiento() const {
    return rangoLanzamiento;
}

float SoldadoGranadero::getCooldownGranada() const {
    return cooldownGranada;
}

float SoldadoGranadero::getTiempoDesdeUltGranada() const {
    return tiempoDesdeUltGranada;
}

float SoldadoGranadero::getVelocidadMovimiento() const {
    return velocidadMovimiento;
}

float SoldadoGranadero::getRadioExplosion() const {
    return radioExplosion;
}

// Setters
void SoldadoGranadero::setRangoVision(float _rangoVision) {
    if (_rangoVision < 0.0f) _rangoVision = 0.0f;
    rangoVision = _rangoVision;
}

void SoldadoGranadero::setRangoLanzamiento(float _rangoLanzamiento) {
    if (_rangoLanzamiento < 0.0f) _rangoLanzamiento = 0.0f;
    rangoLanzamiento = _rangoLanzamiento;
}

void SoldadoGranadero::setCooldownGranada(float _cooldownGranada) {
    if (_cooldownGranada < 0.0f) _cooldownGranada = 0.0f;
    cooldownGranada = _cooldownGranada;
}

void SoldadoGranadero::setTiempoDesdeUltGranada(float _tiempo) {
    if (_tiempo < 0.0f) _tiempo = 0.0f;
    tiempoDesdeUltGranada = _tiempo;
}

void SoldadoGranadero::setVelocidadMovimiento(float _velocidadMovimiento) {
    if (_velocidadMovimiento < 0.0f) _velocidadMovimiento = 0.0f;
    velocidadMovimiento = _velocidadMovimiento;
}

void SoldadoGranadero::setRadioExplosion(float _radioExplosion) {
    if (_radioExplosion < 0.0f) _radioExplosion = 0.0f;
    radioExplosion = _radioExplosion;
}

// Metodos
void SoldadoGranadero::actualizarIA(Personaje* objetivo, float dt) {
    if (dt < 0.0f || objetivo == nullptr) return;
    if (salud <= 0) return;

    tiempoDesdeUltGranada += dt;

    float dx = objetivo->getX() - x;
    float dy = objetivo->getY() - y;
    float dist = std::sqrt(dx * dx + dy * dy);

    if (dist > rangoVision) {
        vx = 0.0f;
        vy = 0.0f;
        return;
    }

    if (dist <= rangoLanzamiento) {
        vx = 0.0f;
        vy = 0.0f;
        return;
    }

    if (dist > 0.0f) {
        float dirX = dx / dist;
        float dirY = dy / dist;
        vx = dirX * velocidadMovimiento;
        vy = dirY * velocidadMovimiento;
        mover(dt);
    }
}

Proyectil* SoldadoGranadero::lanzarGranada(Personaje* objetivo, float velocidadInicial) {
    if (objetivo == nullptr) return nullptr;
    if (salud <= 0) return nullptr;
    if (tiempoDesdeUltGranada < cooldownGranada) return nullptr;

    float dx = objetivo->getX() - x;
    float dy = objetivo->getY() - y;
    float dist = std::sqrt(dx * dx + dy * dy);

    if (dist > rangoLanzamiento) return nullptr;
    if (velocidadInicial <= 0.0f) velocidadInicial = 1.0f;

    float dirX = dx / dist;
    float dirY = dy / dist;

    float vxProj = dirX * velocidadInicial;
    float vyProj = dirY * velocidadInicial + 15.0f;

    Proyectil* granada = new Proyectil(x, y, vxProj, vyProj,
                                       danoBase, Proyectil::TIPO_GRANADA, esAliado);

    granada->configurarExplosivo(
        radioExplosion,
        0.0f,
        objetivo->getX(),
        objetivo->getY()
        );

    tiempoDesdeUltGranada = 0.0f;
    return granada;
}
