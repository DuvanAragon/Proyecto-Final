#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QTimer>
#include <QKeyEvent>
#include <QLabel>
#include <QSoundEffect>
#include <QGraphicsItem>
#include <vector>

#include "Nivel.h"
#include "ControladorJuego.h"
#include "PersonajeGrafico.h"
#include "ProyectilGrafico.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void actualizarMovimiento();
    void actualizarProyectiles();
    void actualizarIAEnemigos();
    void actualizarIAAliados();


private:

    //  UI ESCENA
    Ui::MainWindow *ui;
    QGraphicsScene *escena;
    QGraphicsPixmapItem *fondo;

    //  CONTROLADOR Y NIVEL
    ControladorJuego controlador;
    Nivel *nivel;

    //  COMANDANTE MODELO Y SPRITE
    Comandante *comandanteModelo;
    PersonajeGrafico *comandanteSprite;

    //  ALIADOS
    Fusilero *fusileroModelo;
    PersonajeGrafico *fusileroSprite;

    Medico *medicoModelo;
    PersonajeGrafico *medicoSprite;

    std::vector<PersonajeGrafico*> aliadosSprites;

    //  VARIABLES DE MOVIMIENTO
    float velocidadJugador;
    float dirX;
    float dirY;
    bool upPressed;
    bool downPressed;
    bool leftPressed;
    bool rightPressed;

    QTimer *timerMovimiento;

    //  ENEMIGOS
    std::vector<Personaje*> enemigosModelo;
    std::vector<PersonajeGrafico*> enemigosSprites;

    QTimer *timerIAEnemigos;

    std::vector<float> tiempoDesdeUltDisparo;

    // Obstaculos en el mapa
    std::vector<QGraphicsItem*> obstaculos;

    //  PROYECTILES
    std::vector<Proyectil*> proyectilesModelo;
    std::vector<ProyectilGrafico*> proyectilesGraficos;

    QTimer *timerProyectiles;

    //  GRANADAS
    std::vector<float> granadasX;
    std::vector<float> granadasY;
    std::vector<float> granadasTiempo;
    std::vector<float> granadasRadio;
    std::vector<int>   granadasDano;
    std::vector<QGraphicsItem*> granadasItems;

    std::vector<QPixmap> explosionFrames;
    std::vector<QGraphicsPixmapItem*> explosionesItems;
    std::vector<float> explosionesTiempo;


    //  ZONA OBJETIVO / ESTADO NIVEL
    QGraphicsRectItem *zonaObjetivo;
    bool nivelCompletado;

    //  HUD
    QLabel *hudVidaLabel;
    QLabel *hudFusileroLabel;
    QLabel *hudMedicoLabel;
    QLabel *hudModoAliadosLabel;

    bool aliadosEnCobertura;
    float fusileroCobX, fusileroCobY;
    float medicoCobX,  medicoCobY;   

    //  FUNCIONES INTERNAS
    void inicializarEscena();
    void actualizarHUD();

    void crearComandante();
    void crearAliados();
    void crearEnemigos();
    void crearObstaculos();
    void crearZonaObjetivo();

    void dispararComandante();

    void verificarVictoria();


    void moverComandante(float dx, float dy);
    bool verificarColisionConObstaculos(float nx, float ny);

    bool hayLineaVision(float x1, float y1,
                        float x2, float y2) const;

    float tiempoRecargaDisparoCmd;
    float intervaloDisparoCmd;

    // SONIDOS
    QSoundEffect sfxExplosion;
    QSoundEffect sfxDisparo;
    QSoundEffect sfxDisparoSniper;
    QSoundEffect sfxPasos;


};

#endif // MAINWINDOW_H
