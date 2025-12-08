#include "Proyectil.h"
#include <cmath>

namespace {
const float GRAVEDAD     = -9.8f;
const float ALTURA_SUELO = 0.0f;
}

// Constructores
Proyectil::Proyectil()
    : x(0.0f),
    y(0.0f),
    vx(0.0f),
    vy(0.0f),
    dano(10),
    activo(true),
    tipo(TIPO_BALA),
    deAliado(true),
    tiempoVida(0.0f),
    tiempoVidaMax(10.0f),
    explosivo(false),
    radioExplosion(0.0f),
    xImpactoPrevisto(0.0f),
    yImpactoPrevisto(0.0f),
    tiempoDetonacion(0.0f),
    tiempoRestanteDetonacion(0.0f),
    exploto(false)
{
}

Proyectil::Proyectil(float xParam,
                     float yParam,
                     float vxParam,
                     float vyParam,
                     int   danoParam,
                     int   tipoParam,
                     bool  deAliadoParam)
    : x(xParam),
    y(yParam),
    vx(vxParam),
    vy(vyParam),
    dano(danoParam),
    activo(true),
    tipo(tipoParam),
    deAliado(deAliadoParam),
    tiempoVida(0.0f),
    tiempoVidaMax(10.0f),
    explosivo(false),
    radioExplosion(0.0f),
    xImpactoPrevisto(0.0f),
    yImpactoPrevisto(0.0f),
    tiempoDetonacion(0.0f),
    tiempoRestanteDetonacion(0.0f),
    exploto(false)
{
}

// Getters
float Proyectil::getX() const { return x; }
float Proyectil::getY() const { return y; }
float Proyectil::getVx() const { return vx; }
float Proyectil::getVy() const { return vy; }
int   Proyectil::getDano() const { return dano; }
bool  Proyectil::estaActivo() const { return activo; }
int   Proyectil::getTipo() const { return tipo; }
bool  Proyectil::getDeAliado() const { return deAliado; }
float Proyectil::getTiempoVida() const { return tiempoVida; }
float Proyectil::getTiempoVidaMax() const { return tiempoVidaMax; }
bool  Proyectil::esExplosivo() const { return explosivo; }
float Proyectil::getRadioExplosion() const { return radioExplosion; }
float Proyectil::getXImpactoPrevisto() const { return xImpactoPrevisto; }
float Proyectil::getYImpactoPrevisto() const { return yImpactoPrevisto; }
bool  Proyectil::yaExploto() const { return exploto; }
float Proyectil::getTiempoRestanteDetonacion() const { return tiempoRestanteDetonacion; }

// Setters
void Proyectil::setPosicion(float _x, float _y) {
    x = _x;
    y = _y;
}

void Proyectil::setVelocidad(float _vx, float _vy) {
    vx = _vx;
    vy = _vy;
}

void Proyectil::setDano(int _dano) {
    dano = _dano;
}

void Proyectil::setActivo(bool _activo) {
    activo = _activo;
}

void Proyectil::setTipo(int _tipo) {
    tipo = _tipo;
}

void Proyectil::setDeAliado(bool _deAliado) {
    deAliado = _deAliado;
}

void Proyectil::setTiempoVidaMax(float _tiempoVidaMax) {
    if (_tiempoVidaMax < 0.0f) _tiempoVidaMax = 0.0f;
    tiempoVidaMax = _tiempoVidaMax;
}

void Proyectil::configurarExplosivo(float _radioExplosion,
                                    float _tiempoDetonacion,
                                    float _xImpactoPrevisto,
                                    float _yImpactoPrevisto)
{
    if (_radioExplosion < 0.0f) _radioExplosion = 0.0f;
    if (_tiempoDetonacion < 0.0f) _tiempoDetonacion = 0.0f;

    explosivo                = true;
    radioExplosion           = _radioExplosion;
    xImpactoPrevisto         = _xImpactoPrevisto;
    yImpactoPrevisto         = _yImpactoPrevisto;
    tiempoDetonacion         = _tiempoDetonacion;
    tiempoRestanteDetonacion = _tiempoDetonacion;
}

// Metodos
void Proyectil::actualizar(float dt) {
    if (!activo || dt < 0.0f) return;

    tiempoVida += dt;
    if (tiempoVidaMax > 0.0f && tiempoVida >= tiempoVidaMax) {
        activo = false;
        return;
    }

    x += vx * dt;
    y += vy * dt;

    aplicarFisica(dt);

    bool enSuelo = (y <= ALTURA_SUELO);
    if (enSuelo) {
        y = ALTURA_SUELO;

        if (tipo == TIPO_GRANADA) {
            const float umbralRebote = 2.0f;
            const float coefRest     = 0.4f;
            const float friccion     = 25.0f;

            if (std::fabs(vy) > umbralRebote) {
                vy = -vy * coefRest;
                vx *= 0.8f;
            } else {
                vy = 0.0f;

                if (std::fabs(vx) > 0.0f) {
                    float signo = (vx > 0.0f) ? 1.0f : -1.0f;
                    float dv    = friccion * dt;
                    float mag   = std::fabs(vx) - dv;
                    if (mag < 0.0f) mag = 0.0f;
                    vx = signo * mag;
                }
            }
        } else if (tipo != TIPO_BALA) {
            activo = false;
        }
    }

    if (explosivo && !exploto) {
        if (tiempoDetonacion > 0.0f) {
            tiempoRestanteDetonacion -= dt;
            if (tiempoRestanteDetonacion <= 0.0f) {
                exploto = true;
                activo  = false;
            }
        } else {
            if (enSuelo) {
                exploto = true;
                activo  = false;
            }
        }
    } else if (!explosivo && tipo != TIPO_BALA && enSuelo) {
    }

}

void Proyectil::aplicarFisica(float dt) {
    if (tipo == TIPO_BALA) {
        return;
    }

    const float kAire = 0.15f;

    vx -= kAire * vx * dt;
    vy += GRAVEDAD * dt - kAire * vy * dt;
}

bool Proyectil::fueDestruido() const {
    return !activo;
}
