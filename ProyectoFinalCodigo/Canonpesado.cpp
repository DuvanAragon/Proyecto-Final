#include "CanonPesado.h"
#include "Proyectil.h"
#include <cmath>

// Constructores
CanonPesado::CanonPesado()
    : Personaje(),
    estadoCanon(1),
    cadenciaBase(3.0f),
    tiempoDesdeUltimoDisparo(0.0f)
{
    esAliado = false;
    setSaludMax(200);
    setSalud(200);
    danoBase = 50;
}

CanonPesado::CanonPesado(float xParam,
                         float yParam,
                         int   saludMaxParam,
                         bool  esAliadoParam,
                         int   danoBaseParam,
                         int   estadoInicial,
                         float cadenciaBaseParam)
    : Personaje(xParam, yParam, saludMaxParam, esAliadoParam, danoBaseParam),
    estadoCanon(estadoInicial),
    cadenciaBase(cadenciaBaseParam),
    tiempoDesdeUltimoDisparo(0.0f)
{
    vx = 0.0f;
    vy = 0.0f;
}

// Getters
int CanonPesado::getEstadoCanon() const {
    return estadoCanon;
}

float CanonPesado::getCadenciaBase() const {
    return cadenciaBase;
}

float CanonPesado::getTiempoDesdeUltimoDisparo() const {
    return tiempoDesdeUltimoDisparo;
}

int CanonPesado::getVida() const {
    return getSalud();
}

// Setters
void CanonPesado::setEstadoCanon(int _estadoCanon) {
    estadoCanon = _estadoCanon;
}

void CanonPesado::setCadenciaBase(float _cadenciaBase) {
    if (_cadenciaBase < 0.0f) {
        _cadenciaBase = 0.0f;
    }
    cadenciaBase = _cadenciaBase;
}

void CanonPesado::setTiempoDesdeUltimoDisparo(float _tiempo) {
    if (_tiempo < 0.0f) {
        _tiempo = 0.0f;
    }
    tiempoDesdeUltimoDisparo = _tiempo;
}

void CanonPesado::setVida(int _vida) {
    setSalud(_vida);
}

// Metodos
void CanonPesado::actualizarEstado(float dt) {
    if (dt < 0.0f) {
        return;
    }
    if (salud <= 0) {
        estadoCanon = 0;
        return;
    }
    tiempoDesdeUltimoDisparo += dt;
}

Proyectil* CanonPesado::disparar(Personaje* objetivo, float velocidadInicial) {
    if (objetivo == nullptr) return nullptr;
    if (salud <= 0) return nullptr;
    if (estadoCanon == 0) return nullptr;
    if (objetivo->getEsAliado() == esAliado) return nullptr;
    if (tiempoDesdeUltimoDisparo < cadenciaBase) return nullptr;
    if (velocidadInicial <= 0.0f) velocidadInicial = 1.0f;

    float dx = objetivo->getX() - x;
    float dy = objetivo->getY() - y;
    float dist = std::sqrt(dx * dx + dy * dy);
    if (dist == 0.0f) dist = 1.0f;

    float dirX = dx / dist;
    float dirY = dy / dist;

    float vxProj = dirX * velocidadInicial;
    float vyProj = dirY * velocidadInicial;

    Proyectil* p = new Proyectil(x, y, vxProj, vyProj, danoBase,
                                 Proyectil::TIPO_CANON, esAliado);
    tiempoDesdeUltimoDisparo = 0.0f;
    return p;
}
