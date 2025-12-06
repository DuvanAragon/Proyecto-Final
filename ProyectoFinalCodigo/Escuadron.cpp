#include "Escuadron.h"

// Constructor
Escuadron::Escuadron()
    : indiceSeleccionado(-1)
{
}

void Escuadron::agregarMiembro(Personaje* p) {
    if (p == nullptr) return;
    miembros.push_back(p);

    if (indiceSeleccionado == -1) {
        indiceSeleccionado = 0;
    }
}

void Escuadron::eliminarMiembro(Personaje* p) {
    if (p == nullptr) return;

    for (std::size_t i = 0; i < miembros.size(); ++i) {
        if (miembros[i] == p) {
            miembros.erase(miembros.begin() + static_cast<long>(i));

            if (miembros.empty()) {
                indiceSeleccionado = -1;
            } else if (indiceSeleccionado >= static_cast<int>(miembros.size())) {
                indiceSeleccionado = static_cast<int>(miembros.size()) - 1;
            }

            break;
        }
    }
}

void Escuadron::limpiar() {
    miembros.clear();
    indiceSeleccionado = -1;
}

int Escuadron::getCantidad() const {
    return static_cast<int>(miembros.size());
}

bool Escuadron::estaVacio() const {
    return miembros.empty();
}


Personaje* Escuadron::getMiembro(int idx) const {
    if (idx < 0 || idx >= static_cast<int>(miembros.size())) {
        return nullptr;
    }
    return miembros[idx];
}

Personaje* Escuadron::getMiembroActual() const {
    if (indiceSeleccionado < 0 || indiceSeleccionado >= static_cast<int>(miembros.size())) {
        return nullptr;
    }
    return miembros[indiceSeleccionado];
}

int Escuadron::getIndiceSeleccionado() const {
    return indiceSeleccionado;
}

void Escuadron::setIndiceSeleccionado(int idx) {
    if (idx < 0 || idx >= static_cast<int>(miembros.size())) {
        indiceSeleccionado = -1;
    } else {
        indiceSeleccionado = idx;
    }
}

void Escuadron::seleccionarSiguiente() {
    if (miembros.empty()) {
        indiceSeleccionado = -1;
        return;
    }

    if (indiceSeleccionado < 0) {
        indiceSeleccionado = 0;
    } else {
        indiceSeleccionado = (indiceSeleccionado + 1) % static_cast<int>(miembros.size());
    }
}

void Escuadron::seleccionarAnterior() {
    if (miembros.empty()) {
        indiceSeleccionado = -1;
        return;
    }

    if (indiceSeleccionado < 0) {
        indiceSeleccionado = 0;
    } else {
        indiceSeleccionado--;
        if (indiceSeleccionado < 0) {
            indiceSeleccionado = static_cast<int>(miembros.size()) - 1;
        }
    }
}

Personaje* Escuadron::buscarMasHeridoAliado() const {
    Personaje* masHerido = nullptr;
    int saludMin = 0;

    for (Personaje* p : miembros) {
        if (p == nullptr) continue;
        if (!p->getEsAliado()) continue;
        if (p->getSalud() <= 0) continue; // ignorar muertos

        if (masHerido == nullptr || p->getSalud() < saludMin) {
            masHerido = p;
            saludMin = p->getSalud();
        }
    }

    return masHerido;
}

Personaje* Escuadron::buscarMasHeridoEnemigo() const {
    Personaje* masHerido = nullptr;
    int saludMin = 0;

    for (Personaje* p : miembros) {
        if (p == nullptr) continue;
        if (p->getEsAliado()) continue;
        if (p->getSalud() <= 0) continue;

        if (masHerido == nullptr || p->getSalud() < saludMin) {
            masHerido = p;
            saludMin = p->getSalud();
        }
    }

    return masHerido;
}

void Escuadron::actualizarTodos(float dt) {
    if (dt < 0.0f) return;

    for (Personaje* p : miembros) {
        if (p == nullptr) continue;
        if (p->getSalud() <= 0) continue;

        p->mover(dt);
    }
}
