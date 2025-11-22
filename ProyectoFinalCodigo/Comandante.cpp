#include "Comandante.h"
#include "Proyectil.h"
#include <cmath>

// Constructores
Comandante::Comandante()
    : Personaje(),
    radioOrden(100.0f),
    bonoDanioAliados(0.5f)
{
    esAliado = true;
    setSaludMax(120);
    setSalud(120);
    danoBase = 20;
}

Comandante::Comandante(float x,
                       float y,
                       int   saludMax,
                       bool  esAliadoParam,
                       int   danoBaseParam,
                       float radioOrdenParam,
                       float bonoDanioAliadosParam)
    : Personaje(x, y, saludMax, esAliadoParam, danoBaseParam),
    radioOrden(radioOrdenParam),
    bonoDanioAliados(bonoDanioAliadosParam)
{
}

// Getters
float Comandante::getRadioOrden() const {
    return radioOrden;
}

float Comandante::getBonoDanioAliados() const {
    return bonoDanioAliados;
}

// Setters
void Comandante::setRadioOrden(float _radioOrden) {
    if (_radioOrden < 0.0f) {
        _radioOrden = 0.0f;
    }
    radioOrden = _radioOrden;
}

void Comandante::setBonoDanioAliados(float _bonoDanioAliados) {
    if (_bonoDanioAliados < 0.0f) {
        _bonoDanioAliados = 0.0f;
    }
    bonoDanioAliados = _bonoDanioAliados;
}

// Metodos
void Comandante::darOrdenAtacar(Personaje* aliado, Personaje* objetivo) {
    if (aliado == nullptr || objetivo == nullptr) {
        return;
    }
    if (!aliado->getEsAliado()) {
        return;
    }
    if (objetivo->getEsAliado() == aliado->getEsAliado()) {
        return;
    }
    if (aliado->getSalud() <= 0 || objetivo->getSalud() <= 0) {
        return;
    }

    float dx = aliado->getX() - x;
    float dy = aliado->getY() - y;
    float dist = std::sqrt(dx * dx + dy * dy);
    if (dist > radioOrden) {
        return;
    }

    int danioOriginal = aliado->getDanoBase();
    int danioPotenciado = danioOriginal + static_cast<int>(danioOriginal * bonoDanioAliados);
    if (danioPotenciado <= 0) {
        danioPotenciado = 1;
    }

    aliado->setDanoBase(danioPotenciado);
    aliado->atacar(objetivo);
    aliado->setDanoBase(danioOriginal);
}

Proyectil* Comandante::disparar(Personaje* objetivo, float velocidadInicial) {
    if (objetivo == nullptr) return nullptr;
    if (salud <= 0) return nullptr;
    if (objetivo->getEsAliado() == esAliado) return nullptr;
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
                                 Proyectil::TIPO_BALA, esAliado);
    return p;
}
