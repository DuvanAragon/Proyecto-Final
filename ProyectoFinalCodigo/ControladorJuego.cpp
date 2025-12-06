#include "ControladorJuego.h"

// Constructor
ControladorJuego::ControladorJuego()
    : numeroNivelActual(1)
{
    nivel.setNumeroNivel(numeroNivelActual);
}

// Configurar nivel
void ControladorJuego::setNumeroNivel(int n)
{
    if (n < 1) {
        n = 1;
    }
    numeroNivelActual = n;
    nivel.setNumeroNivel(numeroNivelActual);
}

int ControladorJuego::getNumeroNivel() const
{
    return numeroNivelActual;
}

void ControladorJuego::iniciarNivel()
{
    nivel.iniciar();
}



void ControladorJuego::actualizarPaso()
{
    nivel.actualizarPaso();
}

Comandante* ControladorJuego::getComandante()
{
    return nivel.getComandante();
}

const Comandante* ControladorJuego::getComandante() const
{
    return nivel.getComandante();
}

const std::vector<Personaje*>& ControladorJuego::getAliados() const
{
    return nivel.getAliados();
}

const std::vector<Personaje*>& ControladorJuego::getEnemigos() const
{
    return nivel.getEnemigos();
}

const std::vector<Proyectil*>& ControladorJuego::getProyectiles() const
{
    return nivel.getProyectiles();
}


bool ControladorJuego::victoria() const
{
    return nivel.hayVictoria();
}

bool ControladorJuego::derrota() const
{
    return nivel.hayDerrota();
}

bool ControladorJuego::nivelTerminado() const
{
    return nivel.estaTerminado();
}

Nivel& ControladorJuego::getNivel()
{
    return nivel;
}

const Nivel& ControladorJuego::getNivel() const
{
    return nivel;
}
