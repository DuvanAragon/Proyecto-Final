#include "Fusilero.h"
#include "Proyectil.h"
#include <cmath>
#include <cstdlib>

// Constructores
Fusilero::Fusilero()
    : Personaje(),
    alcanceDisparo(350.0f),
    cadenciaDisparo(0.75f),
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


    objetivo->recibirDano(danoBase);
}

Proyectil* Fusilero::disparar(Personaje* objetivo, float velocidadInicial)
{
    if (!objetivo) return nullptr;

    if (tiempoDesdeUltimoDisparo < cadenciaDisparo) {
        return nullptr;
    }
    tiempoDesdeUltimoDisparo = 0.0f;

    float x = getX();
    float y = getY();

    float dx = objetivo->getX() - x;
    float dy = objetivo->getY() - y;
    float dist = std::sqrt(dx*dx + dy*dy);
    if (dist < 1.0f) dist = 1.0f;

    float dirBaseX = dx / dist;
    float dirBaseY = dy / dist;

    const float PI = 3.14159265f;

    float angBase = std::atan2(dirBaseY, dirBaseX);

    float dispersionGrados = 5.0f;


    if (dist > 250.0f) {
        float factor = (dist - 250.0f) / 200.0f;
        if (factor > 1.5f) factor = 1.5f;
        dispersionGrados += 3.0f * factor;
    }

    float maxRad = dispersionGrados * PI / 180.0f;

    float r = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
    float offset = -maxRad + 2.0f * maxRad * r;

    float angFinal = angBase + offset;

    float dirX = std::cos(angFinal);
    float dirY = std::sin(angFinal);

    float vxProj = dirX * velocidadInicial;
    float vyProj = dirY * velocidadInicial;

    Proyectil* p = new Proyectil(
        x, y,
        vxProj, vyProj,
        danoBase,
        Proyectil::TIPO_BALA,
        true
        );
    p->setTiempoVidaMax(3.0f);
    return p;
}
