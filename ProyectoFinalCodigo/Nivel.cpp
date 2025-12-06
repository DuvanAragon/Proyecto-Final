#include "Nivel.h"
#include "Personaje.h"
#include "Francotirador.h"
#include "SoldadoGranadero.h"
#include <cmath>

using namespace std;

// Constructores
Nivel::Nivel() {
    numeroNivel = 1;
    tiempoActual = 0.0f;
    tiempoMaximo = 30.0f;
    deltaTiempo = 0.016f;

    radioImpacto = 10.0f;
    limiteMapaX = 500.0f;

    cdComandante = 2.0f;
    tiempoDesdeDisparoCom = 0.0f;

    setNumeroNivel(1);
}

// Seters
void Nivel::setNumeroNivel(int _numeroNivel) {
    numeroNivel = _numeroNivel;
    aliados.clear();
    enemigos.clear();

    proyectiles.clear();
    tiempoActual = 0.0f;
    tiempoDesdeDisparoCom = 0.0f;

    if (numeroNivel == 1) {
        configurarNivel1();
    } else if (numeroNivel == 2) {
        configurarNivel2();
    } else {
        configurarNivel3();
    }
}

// Getters
const std::vector<Personaje*>& Nivel::getAliados() const {
    return aliados;
}

const std::vector<Personaje*>& Nivel::getEnemigos() const {
    return enemigos;
}

const std::vector<Proyectil*>& Nivel::getProyectiles() const {
    return proyectiles;
}

Comandante* Nivel::getComandante() {
    return &comandante;
}

const Comandante* Nivel::getComandante() const {
    return &comandante;
}

// Metodos privados de configuracion
void Nivel::configurarNivel1() {
    tiempoMaximo = 20.0f;

    comandante.setPosicion(0.0f, 0.0f);
    medico.setPosicion(-10.0f, 0.0f);
    fusilero.setPosicion(5.0f, 0.0f);
    francotirador.setPosicion(10.0f, 0.0f);

    enemigo.setPosicion(60.0f, 0.0f);

    aliados.push_back(&comandante);
    aliados.push_back(&medico);
    aliados.push_back(&fusilero);

    enemigos.push_back(&enemigo);
}

void Nivel::configurarNivel2() {
    tiempoMaximo = 25.0f;

    comandante.setPosicion(0.0f, 0.0f);
    medico.setPosicion(-12.0f, 0.0f);
    fusilero.setPosicion(4.0f, 0.0f);
    francotirador.setPosicion(8.0f, 2.0f);

    enemigo.setPosicion(70.0f, 0.0f);
    granadero.setPosicion(90.0f, 0.0f);

    aliados.push_back(&comandante);
    aliados.push_back(&medico);
    aliados.push_back(&fusilero);
    aliados.push_back(&francotirador);

    enemigos.push_back(&enemigo);
    enemigos.push_back(&granadero);
}

void Nivel::configurarNivel3() {
    tiempoMaximo = 30.0f;

    comandante.setPosicion(0.0f, 0.0f);
    medico.setPosicion(-15.0f, 0.0f);
    fusilero.setPosicion(3.0f, -1.0f);
    francotirador.setPosicion(7.0f, 2.0f);

    enemigo.setPosicion(80.0f, 0.0f);
    granadero.setPosicion(100.0f, 0.0f);
    canon.setPosicion(130.0f, 0.0f);

    aliados.push_back(&comandante);
    aliados.push_back(&medico);
    aliados.push_back(&fusilero);
    aliados.push_back(&francotirador);

    enemigos.push_back(&enemigo);
    enemigos.push_back(&granadero);
    enemigos.push_back(&canon);
}

Personaje* Nivel::buscarMasHeridoAliado() {
    Personaje* masHerido = nullptr;
    float porcentajeMin = 1.1f;

    for (Personaje* p : aliados) {
        if (p->getSalud() <= 0) continue;
        float porc = (float)p->getSalud() / (float)p->getSaludMax();
        if (porc < porcentajeMin) {
            porcentajeMin = porc;
            masHerido = p;
        }
    }
    return masHerido;
}

Personaje* Nivel::primerEnemigoVivo() {
    for (Personaje* p : enemigos) {
        if (p->getSalud() > 0) return p;
    }
    return nullptr;
}

void Nivel::actualizarProyectiles() {
    for (Proyectil* p : proyectiles) {
        p->actualizar(deltaTiempo);

        if (fabs(p->getX()) > limiteMapaX) {
            p->setActivo(false);
        }

        float px = p->getX();
        float py = p->getY();

        if (p->getTipo() == Proyectil::TIPO_GRANADA) {
            if (p->yaExploto()) {
                float R = p->getRadioExplosion();

                if (p->getDeAliado()) {
                    for (Personaje* e : enemigos) {
                        if (e->getSalud() <= 0) continue;
                        float dx = e->getX() - px;
                        float dy = e->getY() - py;
                        float d  = sqrt(dx * dx + dy * dy);
                        if (d <= R) {
                            e->recibirDano(p->getDano());
                        }
                    }
                } else {
                    for (Personaje* a : aliados) {
                        if (a->getSalud() <= 0) continue;
                        float dx = a->getX() - px;
                        float dy = a->getY() - py;
                        float d  = sqrt(dx * dx + dy * dy);
                        if (d <= R) {
                            a->recibirDano(p->getDano());
                        }
                    }
                }
            }
        } else {
            if (p->getDeAliado()) {
                for (Personaje* e : enemigos) {
                    if (e->getSalud() <= 0) continue;
                    float dx = e->getX() - px;
                    float dy = e->getY() - py;
                    float d  = sqrt(dx * dx + dy * dy);
                    if (d <= radioImpacto) {
                        e->recibirDano(p->getDano());
                        p->setActivo(false);
                        break;
                    }
                }
            } else {
                for (Personaje* a : aliados) {
                    if (a->getSalud() <= 0) continue;
                    float dx = a->getX() - px;
                    float dy = a->getY() - py;
                    float d  = sqrt(dx * dx + dy * dy);
                    if (d <= radioImpacto) {
                        a->recibirDano(p->getDano());
                        p->setActivo(false);
                        break;
                    }
                }
            }
        }
    }

    vector<Proyectil*> vivos;
    vivos.reserve(proyectiles.size());
    for (Proyectil* p : proyectiles) {
        if (!p->estaActivo() && !p->yaExploto()) {
            delete p;
        } else {
            vivos.push_back(p);
        }
    }
    proyectiles.swap(vivos);
}

// Metodos publicos
void Nivel::iniciar() {
    tiempoActual = 0.0f;
    tiempoDesdeDisparoCom = 0.0f;
}

void Nivel::actualizarPaso() {
    if (estaTerminado()) return;

    tiempoActual        += deltaTiempo;
    tiempoDesdeDisparoCom += deltaTiempo;

    for (Personaje* a : aliados) {
        a->actualizar(deltaTiempo);
    }

    if (numeroNivel >= 1 && enemigo.getSalud() > 0) {
        enemigo.actualizarIA(&comandante, deltaTiempo);
    }
    if (numeroNivel >= 2 && granadero.getSalud() > 0) {
        granadero.actualizarIA(&comandante, deltaTiempo);
    }
    if (numeroNivel >= 3 && canon.getSalud() > 0) {
        canon.actualizarEstado(deltaTiempo);
    }

    Personaje* herido = buscarMasHeridoAliado();
    if (herido != nullptr && medico.getSalud() > 0) {
        medico.curar(herido);
    }

    Personaje* objetivo = primerEnemigoVivo();
    if (objetivo != nullptr) {
        if (tiempoDesdeDisparoCom >= cdComandante && comandante.getSalud() > 0) {
            Proyectil* p = comandante.disparar(objetivo, 50.0f);
            if (p != nullptr) {
                proyectiles.push_back(p);
                tiempoDesdeDisparoCom = 0.0f;
            }
        }

        if (fusilero.getSalud() > 0) {
            Proyectil* pF = fusilero.disparar(objetivo, 45.0f);
            if (pF != nullptr) {
                proyectiles.push_back(pF);
            }
        }

        if (francotirador.getSalud() > 0 && numeroNivel >= 2) {
            francotirador.comenzarApuntar(objetivo);
            Proyectil* pS = francotirador.disparar(objetivo, 60.0f);
            if (pS != nullptr) {
                proyectiles.push_back(pS);
            }
        }
    }

    if (enemigo.getSalud() > 0) {
        Proyectil* pE = enemigo.disparar(&comandante, 40.0f);
        if (pE != nullptr) {
            proyectiles.push_back(pE);
        }
    }
    if (numeroNivel >= 2 && granadero.getSalud() > 0) {
        Proyectil* pG = granadero.lanzarGranada(&comandante, 35.0f);
        if (pG != nullptr) {
            proyectiles.push_back(pG);
        }
    }
    if (numeroNivel >= 3 && canon.getSalud() > 0) {
        Proyectil* pC = canon.disparar(&comandante, 30.0f);
        if (pC != nullptr) {
            proyectiles.push_back(pC);
        }
    }

    actualizarProyectiles();
}

bool Nivel::hayVictoria() const {
    bool hayAlgunEnemigo = false;

    for (Personaje* e : enemigos) {
        if (!e) continue;
        hayAlgunEnemigo = true;
        if (e->getSalud() > 0) {
            return false;
        }
    }
    return hayAlgunEnemigo;
}


bool Nivel::hayDerrota() const {
    return comandante.getSalud() <= 0;
}

bool Nivel::hayTiempoAgotado() const {
    return tiempoActual >= tiempoMaximo;
}

bool Nivel::estaTerminado() const {
    return hayVictoria() || hayDerrota() || hayTiempoAgotado();
}


// Destructor
Nivel::~Nivel() {
    for (Proyectil* p : proyectiles) {
        delete p;
    }
    proyectiles.clear();
}

bool Nivel::verificarVictoria(const Comandante* comandante,
                              const std::vector<Personaje*>& enemigos,
                              const std::vector<Personaje*>& aliados,
                              float zonaXmin, float zonaXmax,
                              float zonaYmin, float zonaYmax)
{
    if (!comandante) return false;

    float xC = comandante->getX();
    float yC = comandante->getY();

    // Comandante debe estar dentro de la zona
    bool comandanteEnZona =
        (xC >= zonaXmin && xC <= zonaXmax &&
         yC >= zonaYmin && yC <= zonaYmax);

    if (!comandanteEnZona) return false;

    // Todos los enemigos vivos deben estar muertos
    for (const Personaje* e : enemigos) {
        if (e && e->getSalud() > 0) return false;
    }

    // Aliados vivos deben estar dentro de la zona
    for (const Personaje* a : aliados) {
        if (!a) continue;
        if (a == comandante) continue;
        if (a->getSalud() <= 0) continue;

        float xa = a->getX();
        float ya = a->getY();

        bool aliadoEnZona =
            (xa >= zonaXmin && xa <= zonaXmax &&
             ya >= zonaYmin && ya <= zonaYmax);

        if (!aliadoEnZona)
            return false;
    }

    return true;
}

Personaje* Nivel::obtenerEnemigoMasCercanoAl(
    const Personaje* referencia,
    const std::vector<Personaje*>& enemigos
    ) const
{
    if (!referencia) return nullptr;

    float rx = referencia->getX();
    float ry = referencia->getY();

    Personaje* enemigoMasCercano = nullptr;
    float distMinEnemigo = 1e9f;

    for (Personaje* ene : enemigos) {
        if (!ene) continue;
        if (ene->getSalud() <= 0) continue;

        float dx = ene->getX() - rx;
        float dy = ene->getY() - ry;
        float d  = std::sqrt(dx*dx + dy*dy);

        if (d < distMinEnemigo) {
            distMinEnemigo = d;
            enemigoMasCercano = ene;
        }
    }

    return enemigoMasCercano;
}

void Nivel::obtenerParametrosIAEnemigo(
    const Personaje* enemigo,
    float &rangoVision,
    float &distPref,
    float &velMovimiento,
    float &rangoDispMin,
    float &rangoDispMax,
    float &intervaloDisp,
    float &velProyectil
    ) const
{
    // Valores por defecto por si enemigo == nullptr
    rangoVision   = 0.0f;
    distPref      = 0.0f;
    velMovimiento = 0.0f;
    rangoDispMin  = 0.0f;
    rangoDispMax  = 0.0f;
    intervaloDisp = 1e9f;
    velProyectil  = 0.0f;

    if (!enemigo) return;

    const float VEL_REG      = 50.0f;
    const float VEL_GRAN     = 50.0f;
    const float VEL_PROY_REG = 260.0f;
    const float VEL_PROY_FR  = 360.0f;

    const float RANGO_VISION_REG    = 220.0f;
    const float RANGO_VISION_GRAN   = 280.0f;
    const float RANGO_VISION_FRAN   = 500.0f;

    const float DIST_PREF_REG       = 220.0f;
    const float DIST_PREF_GRAN      = 280.0f;
    const float RANGO_DISP_REG      = 360.0f;
    const float RANGO_DISP_GRAN     = 220.0f;
    const float RANGO_DISP_FR_MIN   = 0.0f;
    const float RANGO_DISP_FR_MAX   = 450.0f;

    const float INTERVALO_REG       = 3.0f;
    const float INTERVALO_GRAN      = 5.0f;
    const float INTERVALO_FRAN      = 4.0f;

    bool esFrancotirador = (dynamic_cast<const Francotirador*>(enemigo)    != nullptr);
    bool esGranadero     = (dynamic_cast<const SoldadoGranadero*>(enemigo) != nullptr);
    bool esRegular       = !esFrancotirador && !esGranadero;

    if (esFrancotirador) {
        rangoVision   = RANGO_VISION_FRAN;
        distPref      = 0.0f;
        velMovimiento = 0.0f;
        rangoDispMin  = RANGO_DISP_FR_MIN;
        rangoDispMax  = RANGO_DISP_FR_MAX;
        intervaloDisp = INTERVALO_FRAN;
        velProyectil  = VEL_PROY_FR;
    }
    else if (esGranadero) {
        rangoVision   = RANGO_VISION_GRAN;
        distPref      = DIST_PREF_GRAN;
        velMovimiento = VEL_GRAN;
        rangoDispMin  = 0.0f;
        rangoDispMax  = RANGO_DISP_GRAN;
        intervaloDisp = INTERVALO_GRAN;
        velProyectil  = 0.0f;
    }
    else if (esRegular) {
        rangoVision   = RANGO_VISION_REG;
        distPref      = DIST_PREF_REG;
        velMovimiento = VEL_REG;
        rangoDispMin  = 0.0f;
        rangoDispMax  = RANGO_DISP_REG;
        intervaloDisp = INTERVALO_REG;
        velProyectil  = VEL_PROY_REG;
    }
}


void Nivel::aplicarImpactoProyectil(Personaje* objetivo, int dano)
{
    if (!objetivo) return;
    if (dano <= 0) return;
    if (objetivo->getSalud() <= 0) return;

    objetivo->recibirDano(dano);
}

void Nivel::aplicarExplosionGranada(Personaje* objetivo,
                                    float xExpl,
                                    float yExpl,
                                    float radio,
                                    int dano)
{
    if (!objetivo) return;
    if (dano <= 0) return;
    if (radio < 0.0f) radio = 0.0f;
    if (objetivo->getSalud() <= 0) return;

    float dx = objetivo->getX() - xExpl;
    float dy = objetivo->getY() - yExpl;
    float d  = std::sqrt(dx*dx + dy*dy);

    if (d <= radio) {
        objetivo->recibirDano(dano);
    }
}

Proyectil* Nivel::dispararComandante(float velocidadInicial)
{
    if (comandante.getSalud() <= 0) return nullptr;

    Personaje* objetivo = primerEnemigoVivo();
    if (!objetivo) return nullptr;

    return comandante.disparar(objetivo, velocidadInicial);
}


Fusilero* Nivel::getFusilero()
{
    return &fusilero;
}

Medico* Nivel::getMedico()
{
    return &medico;
}


