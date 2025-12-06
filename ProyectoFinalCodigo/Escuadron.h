#ifndef ESCUADRON_H
#define ESCUADRON_H

#include <vector>
#include "Personaje.h"

class Escuadron {
private:
    std::vector<Personaje*> miembros;
    int indiceSeleccionado;

public:
    // Constructor
    Escuadron();

    // getters
    Personaje* getMiembro(int idx) const;
    Personaje* getMiembroActual() const;

    int  getIndiceSeleccionado() const;
    int getCantidad() const;

    // setters
    void setIndiceSeleccionado(int idx);


    // Metodos
    void agregarMiembro(Personaje* p);
    void eliminarMiembro(Personaje* p);
    void limpiar();

    void seleccionarSiguiente();
    void seleccionarAnterior();

    bool estaVacio() const;

    void actualizarTodos(float dt);

    Personaje* buscarMasHeridoAliado() const;
    Personaje* buscarMasHeridoEnemigo() const;

};

#endif // ESCUADRON_H
