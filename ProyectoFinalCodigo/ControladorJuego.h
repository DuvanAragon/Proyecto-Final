#ifndef CONTROLADORJUEGO_H
#define CONTROLADORJUEGO_H

#include "Nivel.h"

class ControladorJuego
{
public:
    ControladorJuego();

    // getters
    int  getNumeroNivel() const;
    const Comandante* getComandante() const;
    const std::vector<Personaje*>& getAliados() const;
    const std::vector<Personaje*>& getEnemigos() const;
    const std::vector<Proyectil*>& getProyectiles() const;
    const Nivel& getNivel() const;
    Nivel&       getNivel();
    Comandante* getComandante();

    // setters
    void setNumeroNivel(int n);

    // metodos
    void iniciarNivel();
    void actualizarPaso();

    bool victoria() const;
    bool derrota() const;
    bool nivelTerminado() const;



private:
    int  numeroNivelActual;
    Nivel nivel;
};

#endif // CONTROLADORJUEGO_H
