#include "SoldadoGranadero.h"
#include "Proyectil.h"
#include <cmath>
#include <cstdlib>

// Constructores
SoldadoGranadero::SoldadoGranadero()
    : Personaje(),
    rangoVision(150.0f),
    rangoLanzamiento(150.0f),
    cooldownGranada(8.0f),
    tiempoDesdeUltGranada(0.0f),
    velocidadMovimiento(30.0f),
    radioExplosion(40.0f),
    lanzoEnEsteFrame(false)
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
    radioExplosion(radioExplosionParam),
    lanzoEnEsteFrame(false)
{
}

namespace {
    float randEnRango(float minVal, float maxVal) {
        float t = std::rand() / static_cast<float>(RAND_MAX);
        return minVal + (maxVal - minVal) * t;
    }
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

Proyectil* SoldadoGranadero::lanzarGranada(Personaje* objetivo,
                                           float velocidadInicial)
{
    if (objetivo == nullptr) return nullptr;
    if (salud <= 0)          return nullptr;
    if (tiempoDesdeUltGranada < cooldownGranada) return nullptr;

    float dx   = objetivo->getX() - x;
    float dy   = objetivo->getY() - y;
    float dist = std::sqrt(dx * dx + dy * dy);
    if (dist > rangoLanzamiento) return nullptr;
    if (velocidadInicial <= 0.0f) velocidadInicial = 1.0f;

    float dirX = dx / dist;
    float dirY = dy / dist;

    float vxProj = dirX * velocidadInicial;
    float vyProj = dirY * velocidadInicial + 15.0f;   // pequeño arco

    // 🔧 Altura desde la mano para que no salga del suelo
    const float ALTURA_MANO = 35.0f;
    float yLanzamiento = y - ALTURA_MANO;

    Proyectil* granada = new Proyectil(x, yLanzamiento,
                                       vxProj, vyProj,
                                       danoBase, Proyectil::TIPO_GRANADA, esAliado);

    granada->configurarExplosivo(
        radioExplosion,
        0.0f,
        objetivo->getX(),
        objetivo->getY()
        );

    // 🔧 TIEMPO DE VIDA CORTO PARA LA PARÁBOLA VISUAL
    granada->setTiempoVidaMax(1.5f);

    tiempoDesdeUltGranada = 0.0f;
    lanzoEnEsteFrame = true;
    return granada;

}
Proyectil* SoldadoGranadero::lanzarGranadaConError(Personaje* objetivo,
                                                   float /*velocidadInicial*/,
                                                   float errorMaxDistancia)
{
    if (!objetivo) return nullptr;
    if (salud <= 0) return nullptr;
    if (tiempoDesdeUltGranada < cooldownGranada) return nullptr;

    // --- Snapshot de la posición del comandante ---
    float xCmd = objetivo->getX();
    float yCmd = objetivo->getY();  // en tu modelo es ~0 para nivel 2

    // Error en X para oleadas menos precisas
    if (errorMaxDistancia < 0.0f) errorMaxDistancia = 0.0f;
    float errX = randEnRango(-errorMaxDistancia, errorMaxDistancia);

    float xObjetivo = xCmd + errX;
    float yObjetivo = 0.0f;     // queremos que caiga al suelo

    // --- Punto de lanzamiento lógico ---
    const float ALTURA_LANZAMIENTO = 40.0f; // 40 unidades sobre el suelo lógico
    float x0 = x;
    float y0 = ALTURA_LANZAMIENTO;

    // Distancia horizontal
    float dx = xObjetivo - x0;
    float distancia = std::fabs(dx);
    if (distancia < 1.0f) distancia = 1.0f;

    // --- Cálculo balístico aproximado (ignorando rozamiento) ---
    const float g = 9.8f;              // módulo de la gravedad

    // Ángulo de tiro (en radianes) -> 55º aprox
    float angulo = 55.0f * 3.14159265f / 180.0f;
    float sin2a  = std::sin(2.0f * angulo);
    if (std::fabs(sin2a) < 0.01f) sin2a = 0.01f;

    // Velocidad inicial para alcanzar distancia 'distancia'
    float v0 = std::sqrt(distancia * g / sin2a);

    float dirX   = (dx >= 0.0f) ? 1.0f : -1.0f;
    float vxProj = dirX * v0 * std::cos(angulo);
    float vyProj = v0 * std::sin(angulo);     // positivo hacia arriba

    // --- Crear la granada con el constructor correcto (7 parámetros) ---
    Proyectil* granada = new Proyectil(
        x0,
        y0,
        vxProj,
        vyProj,
        danoBase,
        Proyectil::TIPO_GRANADA,
        esAliado    // false para enemigo
        );

    // Tiempo de vuelo aproximado (sube y baja)
    float tiempoVuelo = 2.0f * v0 * std::sin(angulo) / g;
    granada->setTiempoVidaMax(tiempoVuelo + 0.5f);  // un poquito más

    // --- “Inteligencia” según errorMaxDistancia ---
    float tiempoDetonacion = 0.0f;

    if (errorMaxDistancia >= 30.0f) {
        // Oleadas fáciles: explota casi al tocar suelo
        tiempoDetonacion = 0.0f;
    } else if (errorMaxDistancia >= 15.0f) {
        // Intermedio: rebote corto
        tiempoDetonacion = 0.6f;
    } else {
        // Muy preciso (oleadas avanzadas): más tiempo rodando
        tiempoDetonacion = 1.2f;
    }

    granada->configurarExplosivo(
        radioExplosion,
        tiempoDetonacion,
        xObjetivo,
        yObjetivo
        );

    tiempoDesdeUltGranada = 0.0f;
    lanzoEnEsteFrame      = true;
    return granada;
}
