#ifndef NIVEL_H
#define NIVEL_H

#include <vector>

#include "Personaje.h"
#include "Comandante.h"
#include "Medico.h"
#include "Fusilero.h"
#include "Francotirador.h"
#include "SoldadoEnemigo.h"
#include "SoldadoGranadero.h"
#include "CanonPesado.h"
#include "Proyectil.h"

class Nivel
{
public:
    // Constructor
    Nivel();
    ~Nivel();

    // Setters
    void setNumeroNivel(int _numeroNivel);

    // Getters
    int  getNumeroNivel() const { return numeroNivel; }

    Fusilero* getFusilero();
    Medico*   getMedico();

    Comandante*       getComandante();
    const Comandante* getComandante() const;

    Proyectil* dispararComandante(float velocidadInicial);

    const std::vector<Personaje*>& getAliados() const;
    const std::vector<Personaje*>& getEnemigos() const;
    const std::vector<Proyectil*>& getProyectiles() const;

    // Metodos
    void iniciar();
    void actualizarPaso();

    bool hayVictoria() const;
    bool hayDerrota() const;
    bool hayTiempoAgotado() const;
    bool estaTerminado() const;
    Personaje* obtenerEnemigoMasCercanoAl(
        const Personaje* referencia,
        const std::vector<Personaje*>& enemigos
        ) const;


    bool verificarVictoria(const Comandante* comandante,
                           const std::vector<Personaje*>& enemigos,
                           const std::vector<Personaje*>& aliados,
                           float zonaXmin, float zonaXmax,
                           float zonaYmin, float zonaYmax);

    void obtenerParametrosIAEnemigo(
        const Personaje* enemigo,
        float &rangoVision,
        float &distPref,
        float &velMovimiento,
        float &rangoDispMin,
        float &rangoDispMax,
        float &intervaloDisp,
        float &velProyectil
        ) const;


    void aplicarImpactoProyectil(Personaje* objetivo, int dano);

    void aplicarExplosionGranada(Personaje* objetivo,
                                 float xExpl,
                                 float yExpl,
                                 float radio,
                                 int dano);

private:
    // Configuracion inicial de cada nivel
    void configurarNivel1();
    void configurarNivel2();
    void configurarNivel3();

    // Apoyo logico
    Personaje* buscarMasHeridoAliado();
    Personaje* primerEnemigoVivo();
    void       actualizarProyectiles();

private:
    int   numeroNivel;
    float tiempoActual;
    float tiempoMaximo;
    float deltaTiempo;

    float radioImpacto;
    float limiteMapaX;

    float cdComandante;
    float tiempoDesdeDisparoCom;

    // Entidades propias del nivel
    Comandante       comandante;
    Medico           medico;
    Fusilero         fusilero;
    Francotirador    francotirador;
    SoldadoEnemigo   enemigo;
    SoldadoGranadero granadero;
    CanonPesado      canon;

    // Listas de punteros a personajes
    std::vector<Personaje*> aliados;
    std::vector<Personaje*> enemigos;

    // Proyectiles dinamicos
    std::vector<Proyectil*> proyectiles;
};

#endif // NIVEL_H
