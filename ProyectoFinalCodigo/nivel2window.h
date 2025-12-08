#ifndef NIVEL2WINDOW_H
#define NIVEL2WINDOW_H

#include <QMainWindow>

#include "personaje.h"

#include <QLabel>
#include <algorithm>
#include "SoldadoGranadero.h"
#include <QVector>
#include <QPixmap>
#include <Qmap>
#include <unordered_set>


class Proyectil;
class ProyectilGrafico;
class personaje;


class QGraphicsView;
class QGraphicsScene;
class QGraphicsPixmapItem;
class QGraphicsRectItem;
class QTimer;
class Nivel;
class Comandante;
class Fusilero;
class PersonajeLateralGrafico;

class Nivel2Window : public QMainWindow
{
    Q_OBJECT
public:
    explicit Nivel2Window(QWidget *parent = nullptr);
    ~Nivel2Window();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void actualizarLogica();

private:
    void inicializarEscena();
    void crearAliados();
    void crearEnemigos();
    void actualizarMovimiento(float dt);

    QGraphicsView       *vista;
    QGraphicsScene      *escena;
    QGraphicsPixmapItem *fondo;

    QGraphicsRectItem   *sacoColliderSup;
    QGraphicsRectItem   *sacoColliderInf;

    Nivel      *nivel;
    Comandante *comandanteModelo;
    Fusilero   *fusileroModelo;

    PersonajeLateralGrafico *comandanteSprite;
    PersonajeLateralGrafico *fusileroSprite;

    PersonajeLateralGrafico* enemigoSprite;
    PersonajeLateralGrafico* granaderoSprite;

    QTimer *timerLogica;

    bool izquierdaPresionada;
    bool derechaPresionada;
    bool agacharPresionado;

    float velocidad;
    float velocidadCrouch;

    float limiteIzq;
    float limiteDer;
    float limiteArriba;
    float limiteAbajo;

    float limiteSacoJugador;
    float limiteSacoEnemigo;
    float distanciaEnemigosSaco;


    std::vector<Proyectil*>       proyectilesModelo;
    std::vector<ProyectilGrafico*> proyectilesGraficos;
    std::map<Proyectil*, ProyectilGrafico*> proyectilesNivelGraficos;


    float tiempoRecargaCmd;
    float tiempoRecargaFus;
    float intervaloDisparoCmd;
    float intervaloDisparoFus;

    int dirHorizontalCmd;

    // animacion de disparo
    QVector<QPixmap> framesDisparoCmd;
    QVector<QPixmap> framesDisparoFus;
    bool cmdDisparando;
    bool fusDisparando;
    float tiempoDisparoCmd;
    float tiempoDisparoFus;

    void cargarAnimDisparo();
    void actualizarAnimDisparo(float dt);
    void dispararComandante();
    void dispararFusilero();
    void actualizarProyectiles(float dt);
    void sincronizarProyectilesNivel();


    float tiempoNivel;
    float duracionNivel;
    float getFactorDificultad() const;
    void sincronizarSpritesDesdeModelo();

    // Modelos logicos
    Personaje* enemigoModelo   = nullptr;
    Personaje* granaderoModelo = nullptr;

    // Animacion lanzar granada
    std::vector<PersonajeLateralGrafico*> spritesEnemigos;
    QVector<QPixmap> framesLanzarGranadero;
    bool  granaderoLanzando;
    float tiempoLanzarGranadero;
    int   dirHorizontalGranadero;

    void iniciarAnimLanzarGranadero();

    QLabel* hudVidaLabel = nullptr;
    QLabel* hudTiempoLabel;


    void inicializarHUD();
    void actualizarHUD();
    void dispararEnemigoBasico();


};

#endif
