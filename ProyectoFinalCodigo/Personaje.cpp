#include "Personaje.h"

// Constructores
Personaje::Personaje()
    : x(0.0f),
    y(0.0f),
    vx(0.0f),
    vy(0.0f),
    salud(100),
    saludMax(100),
    esAliado(true),
    danoBase(10)
{
}

Personaje::Personaje(float xParam,
                     float yParam,
                     int   saludMaxParam,
                     bool  esAliadoParam,
                     int   danoBaseParam)
    : x(xParam),
    y(yParam),
    vx(0.0f),
    vy(0.0f),
    salud(saludMaxParam),
    saludMax(saludMaxParam),
    esAliado(esAliadoParam),
    danoBase(danoBaseParam)
{
}

// Getters
float Personaje::getX() const {
    return x;
}

float Personaje::getY() const {
    return y;
}

float Personaje::getVx() const {
    return vx;
}

float Personaje::getVy() const {
    return vy;
}

int Personaje::getSalud() const {
    return salud;
}

int Personaje::getSaludMax() const {
    return saludMax;
}

bool Personaje::getEsAliado() const {
    return esAliado;
}

int Personaje::getDanoBase() const {
    return danoBase;
}

// Setters
void Personaje::setPosicion(float _x, float _y) {
    x = _x;
    y = _y;
}

void Personaje::setVelocidad(float _vx, float _vy) {
    vx = _vx;
    vy = _vy;
}

void Personaje::setSalud(int _salud) {
    salud = _salud;
    if (salud > saludMax) {
        salud = saludMax;
    }
    if (salud < 0) {
        salud = 0;
    }
}

void Personaje::setSaludMax(int _saludMax) {
    if (_saludMax < 1) {
        _saludMax = 1;
    }
    saludMax = _saludMax;
    if (salud > saludMax) {
        salud = saludMax;
    }
}

void Personaje::setEsAliado(bool _esAliado) {
    esAliado = _esAliado;
}

void Personaje::setDanoBase(int _danoBase) {
    danoBase = _danoBase;
}

// Metodos
void Personaje::mover(float dt) {
    if (dt < 0.0f) {
        return;
    }
    x += vx * dt;
    y += vy * dt;
}

void Personaje::recibirDano(int cantidad) {
    if (cantidad <= 0) {
        return;
    }
    if (salud <= 0) {
        return;
    }
    setSalud(salud - cantidad);
}

void Personaje::atacar(Personaje* objetivo) {
    if (objetivo == nullptr) {
        return;
    }
    if (salud <= 0) {
        return;
    }
    if (objetivo->getEsAliado() == esAliado) {
        return;
    }
    objetivo->recibirDano(danoBase);
}
