#include "Medico.h"
#include <cmath>

// Constructores
Medico::Medico()
    : Personaje(),
    cantidadCura(25),
    radioCuracion(50.0f),
    cooldownCuracion(3.0f),
    tiempoDesdeUltimaCuracion(0.0f)
{
    esAliado = true;
    setSaludMax(90);
    setSalud(90);
    danoBase = 5;
}

Medico::Medico(float xParam,
               float yParam,
               int   saludMaxParam,
               bool  esAliadoParam,
               int   danoBaseParam,
               int   cantidadCuraParam,
               float radioCuracionParam,
               float cooldownCuracionParam)
    : Personaje(xParam, yParam, saludMaxParam, esAliadoParam, danoBaseParam),
    cantidadCura(cantidadCuraParam),
    radioCuracion(radioCuracionParam),
    cooldownCuracion(cooldownCuracionParam),
    tiempoDesdeUltimaCuracion(0.0f)
{
}

// Getters
int Medico::getCantidadCura() const {
    return cantidadCura;
}

float Medico::getRadioCuracion() const {
    return radioCuracion;
}

float Medico::getCooldownCuracion() const {
    return cooldownCuracion;
}

float Medico::getTiempoDesdeUltimaCuracion() const {
    return tiempoDesdeUltimaCuracion;
}

// Setters
void Medico::setCantidadCura(int _cantidadCura) {
    if (_cantidadCura < 0) {
        _cantidadCura = 0;
    }
    cantidadCura = _cantidadCura;
}

void Medico::setRadioCuracion(float _radioCuracion) {
    if (_radioCuracion < 0.0f) {
        _radioCuracion = 0.0f;
    }
    radioCuracion = _radioCuracion;
}

void Medico::setCooldownCuracion(float _cooldownCuracion) {
    if (_cooldownCuracion < 0.0f) {
        _cooldownCuracion = 0.0f;
    }
    cooldownCuracion = _cooldownCuracion;
}

void Medico::setTiempoDesdeUltimaCuracion(float _tiempo) {
    if (_tiempo < 0.0f) {
        _tiempo = 0.0f;
    }
    tiempoDesdeUltimaCuracion = _tiempo;
}

// Metodos
void Medico::actualizar(float dt) {
    if (dt < 0.0f) {
        return;
    }
    tiempoDesdeUltimaCuracion += dt;
}

Personaje* Medico::seleccionarAliadoMasHerido(Personaje** aliados, int cantidadAliados) {
    if (aliados == nullptr || cantidadAliados <= 0) {
        return nullptr;
    }

    Personaje* masHerido = nullptr;
    int saludMin = 0;

    for (int i = 0; i < cantidadAliados; ++i) {
        Personaje* p = aliados[i];
        if (p == nullptr) {
            continue;
        }
        if (!p->getEsAliado()) {
            continue;
        }
        if (p->getSalud() <= 0) {
            continue;
        }

        float dx = p->getX() - x;
        float dy = p->getY() - y;
        float dist = std::sqrt(dx * dx + dy * dy);
        if (dist > radioCuracion) {
            continue;
        }

        if (masHerido == nullptr || p->getSalud() < saludMin) {
            masHerido = p;
            saludMin = p->getSalud();
        }
    }

    return masHerido;
}

void Medico::curar(Personaje* objetivo) {
    if (objetivo == nullptr) {
        return;
    }
    if (salud <= 0) {
        return;
    }
    if (!objetivo->getEsAliado()) {
        return;
    }
    if (objetivo->getSalud() <= 0) {
        return;
    }

    float dx = objetivo->getX() - x;
    float dy = objetivo->getY() - y;
    float dist = std::sqrt(dx * dx + dy * dy);
    if (dist > radioCuracion) {
        return;
    }

    if (tiempoDesdeUltimaCuracion < cooldownCuracion) {
        return;
    }

    int saludObj = objetivo->getSalud();
    int saludMaxObj = objetivo->getSaludMax();
    if (saludMaxObj <= 0) {
        return;
    }

    float porcentaje = static_cast<float>(saludObj) / static_cast<float>(saludMaxObj);
    if (porcentaje >= 0.4f) {
        return;
    }

    int nuevaSalud = saludObj + cantidadCura;
    if (nuevaSalud > saludMaxObj) {
        nuevaSalud = saludMaxObj;
    }
    objetivo->setSalud(nuevaSalud);
    tiempoDesdeUltimaCuracion = 0.0f;
}
