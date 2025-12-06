#include "Francotirador.h"
#include "Proyectil.h"
#include <cmath>

Francotirador::Francotirador() : Personaje() {
    // Configurar como aliado y stats base
    esAliado = true;
    saludMax = 80;
    salud    = saludMax;
    danoBase = 40;

    apuntando = false;
    tiempoApuntando = 0.0f;
    tiempoNecesario = 1.0f;
    objetivoActual = nullptr;
}

bool Francotirador::getApuntando() const {
    return apuntando;
}

void Francotirador::comenzarApuntar(Personaje* objetivo) {
    objetivoActual = objetivo;
    apuntando = true;
    tiempoApuntando = 0.0f;
}

bool Francotirador::listoParaDisparar() const {
    return apuntando && tiempoApuntando >= tiempoNecesario;
}

void Francotirador::actualizar(float dt) {
    if (dt < 0.0f) return;
    if (!apuntando) return;

    tiempoApuntando += dt;
    if (tiempoApuntando > tiempoNecesario) {
        tiempoApuntando = tiempoNecesario;
    }
}

Proyectil* Francotirador::disparar(Personaje* objetivo, float velocidadInicial) {
    if (!listoParaDisparar()) return nullptr;
    if (objetivo == nullptr) return nullptr;
    if (salud <= 0) return nullptr;
    if (velocidadInicial <= 0.0f) velocidadInicial = 1.0f;

    float dx = objetivo->getX() - x;
    float dy = objetivo->getY() - y;
    float dist = std::sqrt(dx * dx + dy * dy);
    if (dist == 0.0f) dist = 1.0f;

    float dirX = dx / dist;
    float dirY = dy / dist;

    float vxProj = dirX * velocidadInicial;
    float vyProj = dirY * velocidadInicial;

    Proyectil* p = new Proyectil(x,
                                 y,
                                 vxProj,
                                 vyProj,
                                 danoBase,
                                 Proyectil::TIPO_BALA,
                                 esAliado);

    apuntando = false;
    tiempoApuntando = 0.0f;
    objetivoActual = nullptr;

    return p;
}
