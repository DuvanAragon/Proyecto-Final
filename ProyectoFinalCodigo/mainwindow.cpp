#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QShowEvent>

#include <QGraphicsView>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QPixmap>
#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>
#include <QPushButton>
#include <QMessageBox>
#include <QLabel>
#include <cmath>
#include <QPainterPath>
#include <QMouseEvent>
#include <QSoundEffect>
#include <QVBoxLayout>
#include <QPropertyAnimation>
#include <QApplication>

#include "MenuWindow.h"
#include "PersonajeGrafico.h"
#include "ProyectilGrafico.h"
#include "Nivel.h"
#include "nivel2window.h"


// Constantes de tiempo
static const float DT_MOVIMIENTO = 0.016f;
static const float DT_IA         = 0.03f;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , escena(nullptr)
    , fondo(nullptr)
    , nivel(nullptr)
    , comandanteModelo(nullptr)
    , comandanteSprite(nullptr)
    , fusileroModelo(nullptr)
    , fusileroSprite(nullptr)
    , medicoModelo(nullptr)
    , medicoSprite(nullptr)
    , velocidadJugador(1.5f)
    , dirX(1.0f)
    , dirY(0.0f)
    , upPressed(false)
    , downPressed(false)
    , leftPressed(false)
    , rightPressed(false)
    , timerMovimiento(nullptr)
    , enemigosModelo()
    , enemigosSprites()
    , timerIAEnemigos(nullptr)
    , tiempoDesdeUltDisparo()
    , obstaculos()
    , proyectilesModelo()
    , proyectilesGraficos()
    , timerProyectiles(nullptr)
    , granadasX()
    , granadasY()
    , granadasTiempo()
    , granadasRadio()
    , granadasDano()
    , granadasItems()
    , zonaObjetivo(nullptr)
    , nivelCompletado(false)
    , hudVidaLabel(nullptr)
    , hudFusileroLabel(nullptr)
    , hudMedicoLabel(nullptr)
    , hudModoAliadosLabel(nullptr)
    , aliadosEnCobertura(false)
    , fusileroCobX(0.0f)
    , fusileroCobY(0.0f)
    , medicoCobX(0.0f)
    , medicoCobY(0.0f)
{
    ui->setupUi(this);

    tiempoRecargaDisparoCmd = 0.0f;
    intervaloDisparoCmd     = 0.25f;

    // Sonidos
    sfxExplosion.setSource(QUrl("qrc:/Proyectiles/ExplosionGranada"));
    sfxExplosion.setVolume(0.8f);

    sfxDisparo.setSource(QUrl("qrc:/Proyectiles/Disparo"));
    sfxDisparo.setVolume(0.7f);

    sfxDisparoSniper.setSource(QUrl("qrc:/Proyectiles/DisparoFrancotirador"));
    sfxDisparoSniper.setVolume(0.9f);

    // Pasos del comandante
    sfxPasos.setSource(QUrl("qrc:/Personas/Pasos"));
    sfxPasos.setLoopCount(QSoundEffect::Infinite);
    sfxPasos.setVolume(0.2f);


    nivel = new Nivel();

    inicializarEscena();

    // Crear personajes graficos desde el modelo
    crearComandante();
    crearAliados();
    crearEnemigos();
    crearObstaculos();
    crearZonaObjetivo();

    if (ui->graphicsView && comandanteSprite) {
        ui->graphicsView->centerOn(comandanteSprite);
    }

    // Timers de juego

    // Movimiento del jugador
    timerMovimiento = new QTimer(this);
    connect(timerMovimiento, &QTimer::timeout,
            this, &MainWindow::actualizarMovimiento);
    timerMovimiento->start(static_cast<int>(DT_MOVIMIENTO * 1000));

    // Actualizacion de proyectiles
    timerProyectiles = new QTimer(this);
    connect(timerProyectiles, &QTimer::timeout,
            this, &MainWindow::actualizarProyectiles);
    timerProyectiles->start(static_cast<int>(DT_MOVIMIENTO * 1000));

    // IA de enemigos
    timerIAEnemigos = new QTimer(this);
    connect(timerIAEnemigos, &QTimer::timeout,
            this, &MainWindow::actualizarIAEnemigos);
    timerIAEnemigos->start(static_cast<int>(DT_IA * 1000));



    // Configurar focus de la vista
    ui->graphicsView->setFocusPolicy(Qt::NoFocus);
    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    ui->graphicsView->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);

    // HUD
    // Vida del comandante
    hudVidaLabel = new QLabel(this);
    hudVidaLabel->setStyleSheet(
        "color: white; "
        "background-color: rgba(0, 0, 0, 150); "
        "padding: 4px; "
        "font-weight: bold;"
        );
    hudVidaLabel->move(10, 10);
    hudVidaLabel->resize(200, 24);
    hudVidaLabel->show();

    // Estado del fusilero
    hudFusileroLabel = new QLabel(this);
    hudFusileroLabel->setStyleSheet(
        "color: lightgreen; "
        "background-color: rgba(0, 0, 0, 150); "
        "padding: 3px; "
        "font-weight: bold;"
        );
    hudFusileroLabel->move(10, 40);
    hudFusileroLabel->resize(220, 22);
    hudFusileroLabel->show();

    // Estado del medico
    hudMedicoLabel = new QLabel(this);
    hudMedicoLabel->setStyleSheet(
        "color: cyan; "
        "background-color: rgba(0, 0, 0, 150); "
        "padding: 3px; "
        "font-weight: bold;"
        );
    hudMedicoLabel->move(10, 65);
    hudMedicoLabel->resize(220, 22);
    hudMedicoLabel->show();

    // Modo de aliados
    hudModoAliadosLabel = new QLabel(this);
    hudModoAliadosLabel->setStyleSheet(
        "color: yellow; "
        "background-color: rgba(0, 0, 0, 150); "
        "padding: 3px; "
        "font-weight: bold;"
        );
    hudModoAliadosLabel->move(10, 90);
    hudModoAliadosLabel->resize(240, 22);
    hudModoAliadosLabel->show();

    // Cargar frames de la explosion de granada
    for (int i = 0; i < 6; ++i) {
        QString ruta = QString(":/SpriteExplosionGranada/Explosion%1").arg(i);
        QPixmap frame;
        if (frame.load(ruta)) {
            explosionFrames.push_back(frame);
        }
    }


    // Actualizar textos iniciales del HUD
    actualizarHUD();
}


MainWindow::~MainWindow()
{
    // Detener timers
    if (timerMovimiento)     timerMovimiento->stop();
    if (timerProyectiles)    timerProyectiles->stop();
    if (timerIAEnemigos)     timerIAEnemigos->stop();

    delete timerMovimiento;
    delete timerProyectiles;
    delete timerIAEnemigos;

    // Eliminar sprites del comandante y aliados
    delete comandanteSprite;
    delete fusileroSprite;
    delete medicoSprite;

    for (auto *pg : aliadosSprites)
        delete pg;
    aliadosSprites.clear();


    // Eliminar sprites de enemigos
    for (auto *eg : enemigosSprites)
        delete eg;
    enemigosSprites.clear();

    // Eliminar sprites de proyectiles
    for (auto *pg : proyectilesGraficos)
        delete pg;
    proyectilesGraficos.clear();

    // Eliminar objetos gráficos “sueltos”
    if (zonaObjetivo)
        delete zonaObjetivo;

    for (QGraphicsItem *it : obstaculos)
        delete it;
    obstaculos.clear();

    for (QGraphicsItem *g : granadasItems)
        delete g;
    granadasItems.clear();

    // Eliminar escena
    delete escena;

    // Eliminar modelo lógico (Nivel)
    delete nivel;

    // Finalmente, eliminar la UI
    delete ui;
}

//   Inicializacion del fondo
void MainWindow::inicializarEscena()
{
    escena = new QGraphicsScene(this);

    QPixmap mapa(":/Nivel1/FondoNivel1");
    if (mapa.isNull()) {
        mapa.load("Mapa nivel 1.png");
    }

    fondo = escena->addPixmap(mapa);
    fondo->setZValue(-10);
    escena->setSceneRect(mapa.rect());

    ui->graphicsView->setScene(escena);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    ui->graphicsView->resetTransform();
    ui->graphicsView->scale(2, 2);
    ui->graphicsView->centerOn(mapa.width()/2.0, mapa.height()/2.0);
}

//   Crear Obstaculos
void MainWindow::crearObstaculos()
{
    auto addCircle = [this](qreal cx, qreal cy, qreal radius) {
        auto *c = escena->addEllipse(
            cx - radius, cy - radius,
            2*radius, 2*radius,
            QPen(Qt::NoPen),
            QBrush(Qt::NoBrush)
            );
        c->setZValue(-5);
        obstaculos.push_back(c);
        return c;
    };

    // Árboles / piedras
    addCircle(123.333, 100,      50);
    addCircle(90.8333, 359.167,  60);
    addCircle(43.3333, 533.333,  40);
    addCircle(182.5,   488.167,  56);
    addCircle(333.333, 441.667,  57);
    addCircle(600.833, 395,      55);
    addCircle(593.333, 113.333,  43);
    addCircle(880,     108.333,  56);
    addCircle(1197.5,  72.5,     51);
    addCircle(1190,    355.833,  49);
    addCircle(1378.33, 490.833,  55);
    addCircle(1628.33, 113.333,  55);
    addCircle(1809.17, 169.167,  52);
    addCircle(1817.5,  399.167,  43);
    addCircle(862.5,   555,      53);
    addCircle(610,     568.333,  28);
    addCircle(944.167, 435,      26);
    addCircle(1446.67, 73.3333,  28);

    {
        qreal cx    = 1609.17;
        qreal cy    = 475.0;
        qreal width = 206.0;
        qreal height= 200.0;
        qreal left  = cx - width/2.0;
        qreal top   = cy - height/2.0;
        qreal right = cx + width/2.0;
        qreal bottom= cy + height/2.0;
        qreal grosor= 20.0;

        auto addWall = [this](qreal x, qreal y, qreal w, qreal h) {
            auto *r = escena->addRect(
                x, y, w, h,
                QPen(Qt::NoPen),
                QBrush(Qt::NoBrush)
                );
            r->setZValue(-5);
            obstaculos.push_back(r);
            return r;
        };

        // Techo
        addWall(left, top, width, grosor);
        // Piso
        addWall(left, bottom - grosor, width, grosor);
        // Pared derecha
        addWall(right - grosor, top, grosor, height);
    }
}

//   Manejo Entradas Teclado
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (!event) return;

    switch (event->key())
    {
    // Movimiento
    case Qt::Key_W:  upPressed    = true;  break;
    case Qt::Key_S:  downPressed  = true;  break;
    case Qt::Key_A:  leftPressed  = true;  break;
    case Qt::Key_D:  rightPressed = true;  break;

    // Disparo
    case Qt::Key_Space:
        dispararComandante();
        break;

    // Cubrirse
    case Qt::Key_C:
        aliadosEnCobertura = true;
        actualizarHUD();
        break;

    // Atacar / Seguir
    case Qt::Key_V:
        aliadosEnCobertura = false;
        actualizarHUD();
        break;

    default:
        QMainWindow::keyPressEvent(event);
        break;
    }
}

//   Manejo Entradas Teclado
void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) return;

    switch (event->key()) {
    case Qt::Key_W: upPressed    = false; break;
    case Qt::Key_S: downPressed  = false; break;
    case Qt::Key_A: leftPressed  = false; break;
    case Qt::Key_D: rightPressed = false; break;
    default:
        QMainWindow::keyReleaseEvent(event);
        break;
    }
}

//   Resize
void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    if (ui->graphicsView && centralWidget()) {
        ui->graphicsView->setGeometry(centralWidget()->rect());
    }
}

//   Zona objetivo
void MainWindow::crearZonaObjetivo()
{
    if (!escena) return;

    QRectF r = escena->sceneRect();

    qreal width  = 150.0;
    qreal height = 550.0;

    qreal x = r.right() - width - 30.0;
    qreal y = r.center().y() - height/2.0;

    zonaObjetivo = escena->addRect(
        x, y,
        width, height,
        QPen(Qt::NoPen),
        QBrush(Qt::NoBrush)
        );
    zonaObjetivo->setZValue(-4);
}

//   HUD vida
void MainWindow::actualizarHUD()
{
    // Comandante
    if (hudVidaLabel) {
        if (comandanteModelo) {
            int vida    = comandanteModelo->getSalud();
            int vidaMax = comandanteModelo->getSaludMax();
            if (vida > 0) {
                hudVidaLabel->setText(
                    QString("Comandante: %1 / %2").arg(vida).arg(vidaMax)
                    );
            } else {
                hudVidaLabel->setText("Comandante: MUERTO");
            }
        } else {
            hudVidaLabel->setText("Comandante: -");
        }
    }

    // Fusilero
    if (hudFusileroLabel) {
        if (fusileroModelo) {
            int vida    = fusileroModelo->getSalud();
            int vidaMax = fusileroModelo->getSaludMax();
            if (vida > 0) {
                hudFusileroLabel->setText(
                    QString("Fusilero: %1 / %2").arg(vida).arg(vidaMax)
                    );
            } else {
                hudFusileroLabel->setText("Fusilero: MUERTO");
            }
        } else {
            hudFusileroLabel->setText("Fusilero: -");
        }
    }

    // Medico
    if (hudMedicoLabel) {
        if (medicoModelo) {
            int vida    = medicoModelo->getSalud();
            int vidaMax = medicoModelo->getSaludMax();
            if (vida > 0) {
                hudMedicoLabel->setText(
                    QString("Medico: %1 / %2").arg(vida).arg(vidaMax)
                    );
            } else {
                hudMedicoLabel->setText("Medico: MUERTO");
            }
        } else {
            hudMedicoLabel->setText("Medico: -");
        }
    }

    // modo de aliados (atacando / cubriendose)
    if (hudModoAliadosLabel) {
        if (aliadosEnCobertura) {
            hudModoAliadosLabel->setText("Aliados: CUBRIENDOSE (C)");
        } else {
            hudModoAliadosLabel->setText("Aliados: ATACANDO / SIGUIENDO (V)");
        }
    }
}

//   Movimiento Continuo
void MainWindow::actualizarMovimiento()
{
    float dx = 0.0f;
    float dy = 0.0f;

    // Direcciones segun teclas
    if (upPressed)    dy -= 1.0f;
    if (downPressed)  dy += 1.0f;
    if (leftPressed)  dx -= 1.0f;
    if (rightPressed) dx += 1.0f;

    float len = std::sqrt(dx*dx + dy*dy);
    bool seMueve = (len > 0.0f);

    // Solo si ya existe el comandante
    if (comandanteModelo && comandanteSprite) {
        if (seMueve) {
            if (!sfxPasos.isPlaying()) {
                sfxPasos.play();
            }
        } else {
            if (sfxPasos.isPlaying()) {
                sfxPasos.stop();
            }
        }
    }

    if (len > 0.0f) {
        dx /= len;
        dy /= len;
        dirX = dx;
        dirY = dy;
    }

    if (!comandanteModelo || !comandanteSprite)
        return;

    const float dt = DT_MOVIMIENTO;
    const float PI = 3.14159265f;

    if (len > 0.0f) {
        float vx = dx * velocidadJugador / dt;
        float vy = dy * velocidadJugador / dt;
        comandanteModelo->setVelocidad(vx, vy);

        float angRad = std::atan2(dy, dx);
        float angDeg = angRad * 180.0f / PI;
        comandanteSprite->setRotation(angDeg);

        moverComandante(dx * velocidadJugador,
                        dy * velocidadJugador);
    } else {
        // Quieto: velocidad logica cero
        comandanteModelo->setVelocidad(0.0f, 0.0f);
    }

    // Actualizar animacion del comandante
    comandanteSprite->actualizarDesdeModelo(dt);

    // Aliados: actualizar animacion tambien
    if (fusileroSprite) {
        fusileroSprite->actualizarDesdeModelo(dt);
    }
    if (medicoSprite) {
        medicoSprite->actualizarDesdeModelo(dt);
    }
    for (auto *pg : aliadosSprites) {
        if (pg) pg->actualizarDesdeModelo(dt);
    }

    if (tiempoRecargaDisparoCmd > 0.0f) {
        tiempoRecargaDisparoCmd -= DT_MOVIMIENTO;
        if (tiempoRecargaDisparoCmd < 0.0f)
            tiempoRecargaDisparoCmd = 0.0f;
    }
}

//   Linea de vision
bool MainWindow::hayLineaVision(float x1, float y1, float x2, float y2) const
{
    if (!escena) return true;

    // Creamos un camino entre los dos puntos en coordenadas de escena
    QPainterPath path;
    path.moveTo(x1, y1);
    path.lineTo(x2, y2);

    // Revisamos si algun obstaculo intersecta esa línea
    for (std::size_t i = 0; i < obstaculos.size(); ++i) {
        QGraphicsItem* ob = obstaculos[i];
        if (!ob) continue;

        // Si la forma del obstaculo intersecta la linea, no hay vision
        if (ob->collidesWithPath(path, Qt::IntersectsItemShape)) {
            return false;
        }
    }

    // No hubo obstaculos en medio - hay linea de vision
    return true;
}

//   Crear Comandante
void MainWindow::crearComandante()
{
    if (!escena) return;

    comandanteModelo = controlador.getComandante();

    comandanteModelo->setPosicion(78.0f, 260.0f);

    comandanteSprite = new PersonajeGrafico(comandanteModelo);
    escena->addItem(comandanteSprite);
    comandanteSprite->setPos(78.0f, 260.0f);
}

//   Movimiento del comandante
void MainWindow::moverComandante(float dx, float dy)
{
    if (!comandanteModelo || !comandanteSprite) return;
    if (nivelCompletado) return;
    if (dx == 0.0f && dy == 0.0f) return;

    QPointF posInicial = comandanteSprite->pos();

    auto intentaMovimiento = [this, posInicial](float mx, float my) -> bool
    {
        // Colocar en posición candidata
        comandanteSprite->setPos(posInicial.x() + mx,
                                 posInicial.y() + my);

        bool hayColision = false;

        // Limites del mapa
        if (escena) {
            QRectF bounds = escena->sceneRect();
            if (!bounds.contains(comandanteSprite->sceneBoundingRect())) {
                hayColision = true;
            }
        }

        // Obstaculos
        if (!hayColision) {
            for (QGraphicsItem *obs : obstaculos) {
                if (!obs) continue;
                if (comandanteSprite->collidesWithItem(obs)) {
                    hayColision = true;
                    break;
                }
            }
        }

        if (hayColision) {
            // Volver a la posición inicial
            comandanteSprite->setPos(posInicial);
            return false;
        }

        return true;
    };

    bool movido = false;

    // Intento movimiento completo
    if (intentaMovimiento(dx, dy)) {
        movido = true;
    }
    // Intento solo X
    else if (dx != 0.0f && intentaMovimiento(dx, 0.0f)) {
        movido = true;
    }
    // Intento solo Y
    else if (dy != 0.0f && intentaMovimiento(0.0f, dy)) {
        movido = true;
    }

    if (!movido) {
        // No se pudo mover en ninguna dirección
        comandanteModelo->setVelocidad(0.0f, 0.0f);
        return;
    }

    // Sincronizar modelo logico con la posicion grafica definitiva
    comandanteModelo->setPosicion(
        static_cast<float>(comandanteSprite->x()),
        static_cast<float>(comandanteSprite->y())
        );

    // Centrar la camara en el comandante
    if (ui && ui->graphicsView) {
        ui->graphicsView->centerOn(comandanteSprite);
    }

    // Verificar victoria igual que antes
    if (!nivelCompletado) {
        verificarVictoria();
    }
}

//   Disparo del Comandante
void MainWindow::dispararComandante()
{
    if (!comandanteModelo || !comandanteSprite || !escena) return;
    if (nivelCompletado) return;

    if (tiempoRecargaDisparoCmd > 0.0f) return;

    // Direccion de disparo
    float dx = dirX;
    float dy = dirY;

    float len = std::sqrt(dx*dx + dy*dy);

    if (len <= 0.0001f) {
        dx = 1.0f;
        dy = 0.0f;
        len = 1.0f;
    }

    dx /= len;
    dy /= len;

    // Posicion de salida: centro del personaje
    float spawnX = comandanteModelo->getX();
    float spawnY = comandanteModelo->getY();

    // Crear proyectil
    int   dano      = comandanteModelo->getDanoBase();
    const float VEL_BALA = 260.0f;

    float vx = dx * VEL_BALA;
    float vy = dy * VEL_BALA;

    Proyectil* p = new Proyectil(
        spawnX, spawnY,
        vx, vy,
        dano,
        Proyectil::TIPO_BALA,
        true
        );
    p->setTiempoVidaMax(3.0f);

    proyectilesModelo.push_back(p);

    ProyectilGrafico* pg = new ProyectilGrafico(p);
    pg->setPos(spawnX, spawnY);
    pg->setZValue(5);
    escena->addItem(pg);
    proyectilesGraficos.push_back(pg);

    sfxDisparo.play();
    tiempoRecargaDisparoCmd = intervaloDisparoCmd;

    // Animacion de disparo
    if (comandanteSprite) {
        comandanteSprite->notificarDisparo(0.15f);
    }
}

//   IA Aliados
void MainWindow::actualizarIAAliados()
{
    if (!comandanteModelo || !comandanteSprite) return;
    if (comandanteModelo->getSalud() <= 0) return;

    float cx = comandanteModelo->getX();
    float cy = comandanteModelo->getY();

    auto moverPersonajeHacia = [this, cx, cy](Personaje* modelo,
                                              PersonajeGrafico* sprite,
                                              float objetivoX,
                                              float objetivoY,
                                              float velocidad,
                                              float radioQuieto) -> void
    {
        if (!modelo || !sprite) return;

        float x = modelo->getX();
        float y = modelo->getY();

        float dx = objetivoX - x;
        float dy = objetivoY - y;
        float dist = std::sqrt(dx*dx + dy*dy);
        if (dist < radioQuieto) {
            modelo->setVelocidad(0.0f, 0.0f);
            return;
        }

        float nx = dx / dist;
        float ny = dy / dist;

        float paso = velocidad * DT_IA;
        if (paso > dist) paso = dist;

        // Distancia al comandante
        float distAlCmdX = x - cx;
        float distAlCmdY = y - cy;
        float distAlCmd  = std::sqrt(distAlCmdX*distAlCmdX + distAlCmdY*distAlCmdY);

        bool lejosDelComandante = (distAlCmd > 120.0f);

        auto puedeMover = [this, sprite, modelo, lejosDelComandante](float px, float py) -> bool {
            float oldX = modelo->getX();
            float oldY = modelo->getY();

            modelo->setPosicion(px, py);
            sprite->actualizarDesdeModelo();

            bool choca = false;

            // Limites del mapa
            if (escena) {
                QRectF bounds = escena->sceneRect();
                if (!bounds.contains(sprite->sceneBoundingRect())) {
                    choca = true;
                }
            }

            // Obstaculos
            if (!choca) {
                for (QGraphicsItem* ob : obstaculos) {
                    if (!ob) continue;
                    if (sprite->collidesWithItem(ob)) {
                        choca = true;
                        break;
                    }
                }
            }

            // Comandante
            if (!choca && comandanteSprite &&
                sprite->collidesWithItem(comandanteSprite)) {
                choca = true;
            }

            // Aliados entre si
            if (!lejosDelComandante && !choca) {
                if (fusileroSprite && sprite != fusileroSprite &&
                    fusileroModelo && fusileroModelo->getSalud() > 0 &&
                    sprite->collidesWithItem(fusileroSprite)) {
                    choca = true;
                }
                if (medicoSprite && sprite != medicoSprite &&
                    medicoModelo && medicoModelo->getSalud() > 0 &&
                    sprite->collidesWithItem(medicoSprite)) {
                    choca = true;
                }
            }

            modelo->setPosicion(oldX, oldY);
            sprite->actualizarDesdeModelo();
            return !choca;
        };

        // Desplazamiento deseado
        float movX = nx * paso;
        float movY = ny * paso;

        float nuevoX = x + movX;
        float nuevoY = y + movY;

        auto aplicarMovimiento = [&](float destX, float destY, float dirX, float dirY) {
            float vx = (destX - x) / DT_IA;
            float vy = (destY - y) / DT_IA;
            modelo->setVelocidad(vx, vy);

            modelo->setPosicion(destX, destY);
            sprite->actualizarDesdeModelo();

            const float PI = 3.14159265f;
            float angRad = std::atan2(dirY, dirX);
            float angDeg = angRad * 180.0f / PI;
            sprite->setRotation(angDeg);
        };

        // Intento mov completo (X+Y)
        if (puedeMover(nuevoX, nuevoY)) {
            aplicarMovimiento(nuevoX, nuevoY, movX, movY);
            return;
        }

        // Intento solo X
        if (puedeMover(x + movX, y)) {
            aplicarMovimiento(x + movX, y, movX, 0.0f);
            return;
        }

        // Intento solo Y
        if (puedeMover(x, y + movY)) {
            aplicarMovimiento(x, y + movY, 0.0f, movY);
            return;
        }

        // No se pudo mover
        modelo->setVelocidad(0.0f, 0.0f);
    };

    // Enemigo mas cercano (para fuego / cobertura)
    Personaje* enemigoMasCercano = nullptr;

    if (nivel) {
        enemigoMasCercano = nivel->obtenerEnemigoMasCercanoAl(comandanteModelo,enemigosModelo);
    }

    // Modo cubrirse
    if (aliadosEnCobertura) {
        // Elegimos un obstaculo cercano al comandante
        QGraphicsItem* obstMasCercano = nullptr;
        float distObsMin = 1e9f;

        for (QGraphicsItem* ob : obstaculos) {
            if (!ob) continue;
            QRectF br = ob->boundingRect();
            QPointF centro = ob->mapToScene(br.center());

            float dx = static_cast<float>(centro.x()) - cx;
            float dy = static_cast<float>(centro.y()) - cy;
            float d  = std::sqrt(dx*dx + dy*dy);
            if (d < distObsMin) {
                distObsMin = d;
                obstMasCercano = ob;
            }
        }

        if (obstMasCercano) {
            // Si existe obstaculo cubrirse detras de el
            QRectF br = obstMasCercano->boundingRect();
            QPointF centro = obstMasCercano->mapToScene(br.center());

            float dxE, dyE;
            if (enemigoMasCercano) {
                // direccion desde enemigo hacia el obstaculo
                dxE = static_cast<float>(centro.x()) - enemigoMasCercano->getX();
                dyE = static_cast<float>(centro.y()) - enemigoMasCercano->getY();
            } else {
                // si no hay enemigo, asumimos frente general desde el comandante
                dxE = static_cast<float>(centro.x()) - cx;
                dyE = static_cast<float>(centro.y()) - cy;
            }

            float d = std::sqrt(dxE*dxE + dyE*dyE);
            if (d < 1.0f) d = 1.0f;
            float nx = dxE / d;
            float ny = dyE / d;

            // Punto base de cobertura detrás del obstaculo
            float baseCobX = static_cast<float>(centro.x()) + nx * 40.0f;
            float baseCobY = static_cast<float>(centro.y()) + ny * 40.0f;

            // Fusilero: justo en el punto de cobertura
            if (fusileroModelo && fusileroSprite && fusileroModelo->getSalud() > 0) {
                moverPersonajeHacia(
                    fusileroModelo, fusileroSprite,
                    baseCobX, baseCobY,
                    90.0f, 5.0f
                    );
            }

            // Médico: un poco mas atras que el fusilero en la misma direccion
            if (medicoModelo && medicoSprite && medicoModelo->getSalud() > 0) {
                float medCobX = baseCobX + nx * 50.0f;
                float medCobY = baseCobY + ny * 20.0f;

                moverPersonajeHacia(
                    medicoModelo, medicoSprite,
                    medCobX, medCobY,
                    90.0f, 5.0f
                    );
            }
        } else {
            // NO hay obstaculos - se forman detras del comandante
            if (fusileroModelo && fusileroSprite && fusileroModelo->getSalud() > 0) {
                float objXF = cx - 60.0f;
                float objYF = cy + 10.0f;
                moverPersonajeHacia(
                    fusileroModelo, fusileroSprite,
                    objXF, objYF,
                    90.0f, 10.0f
                    );
            }

            if (medicoModelo && medicoSprite && medicoModelo->getSalud() > 0) {
                // Medico un poco mas atras que el fusilero
                float objXM = cx - 90.0f;
                float objYM = cy + 30.0f;
                moverPersonajeHacia(
                    medicoModelo, medicoSprite,
                    objXM, objYM,
                    90.0f, 10.0f
                    );
            }
        }
        return;
    }

    // Modo de ataque o seguir

    //  Fusilero
    float objXF = cx - 70.0f;
    float objYF = cy + 15.0f;

    if (fusileroModelo && fusileroSprite && fusileroModelo->getSalud() > 0) {
        fusileroModelo->actualizar(DT_IA);

        moverPersonajeHacia(
            fusileroModelo, fusileroSprite,
            objXF, objYF,
            90.0f, 15.0f
            );

        // Disparo al enemigo mas cercano
        if (enemigoMasCercano) {
            float sx = fusileroModelo->getX();
            float sy = fusileroModelo->getY();
            float tx = enemigoMasCercano->getX();
            float ty = enemigoMasCercano->getY();

            float dxE = tx - sx;
            float dyE = ty - sy;
            float distE = std::sqrt(dxE*dxE + dyE*dyE);

            float alcance = fusileroModelo->getAlcanceDisparo();

            if (distE <= alcance &&
                hayLineaVision(sx, sy, tx, ty))

            {
                Proyectil* p = fusileroModelo->disparar(enemigoMasCercano, 320.0f);
                if (p != nullptr) {
                    ProyectilGrafico* pg = new ProyectilGrafico(p);
                    pg->setZValue(5);
                    escena->addItem(pg);

                    proyectilesModelo.push_back(p);
                    proyectilesGraficos.push_back(pg);

                    // Sonido
                    sfxDisparo.play();
                }
            }
        }

    }

    // Medico
    if (medicoModelo && medicoSprite && medicoModelo->getSalud() > 0) {
        medicoModelo->actualizar(DT_IA);

        // Posicion base: un poco detras del fusilero
        float objXM;
        float objYM;

        if (fusileroModelo && fusileroModelo->getSalud() > 0) {
            float fx = fusileroModelo->getX();
            float fy = fusileroModelo->getY();

            float vx = fx - cx;
            float vy = fy - cy;
            float d  = std::sqrt(vx*vx + vy*vy);
            if (d < 1.0f) d = 1.0f;
            float nx = vx / d;
            float ny = vy / d;

            // Médico un poco mas atras en la misma linea
            objXM = fx - nx * 30.0f;
            objYM = fy - ny * 10.0f;
        } else {
            // Si no hay fusilero, se ubica cerca del comandante
            objXM = cx - 50.0f;
            objYM = cy + 25.0f;
        }

        // Curacion: se adapta si hay objetivo herido
        Personaje* aliados[2];
        int nAliados = 0;
        if (comandanteModelo && comandanteModelo->getSalud() > 0)
            aliados[nAliados++] = comandanteModelo;
        if (fusileroModelo && fusileroModelo->getSalud() > 0)
            aliados[nAliados++] = fusileroModelo;

        Personaje* objetivoCuracion = nullptr;
        if (nAliados > 0) {
            objetivoCuracion = medicoModelo->seleccionarAliadoMasHerido(
                aliados, nAliados
                );
        }

        if (objetivoCuracion) {
            objXM = objetivoCuracion->getX() - 15.0f;
            objYM = objetivoCuracion->getY() + 10.0f;
        }

        moverPersonajeHacia(
            medicoModelo, medicoSprite,
            objXM, objYM,
            80.0f, 15.0f
            );

        if (objetivoCuracion) {
            int vidaAntes = objetivoCuracion->getSalud();
            medicoModelo->curar(objetivoCuracion);
            int vidaDespues = objetivoCuracion->getSalud();

            if (vidaDespues != vidaAntes) {
                if (objetivoCuracion == comandanteModelo) {
                    actualizarHUD();
                }
                actualizarHUD();
            }
        }
    }
}

//   IA Enemigos
void MainWindow::actualizarIAEnemigos()
{
    if (!comandanteModelo || !comandanteSprite) return;
    if (comandanteModelo->getSalud() <= 0) return;

    float cx = comandanteModelo->getX();
    float cy = comandanteModelo->getY();

    if (tiempoDesdeUltDisparo.size() < enemigosModelo.size()) {
        tiempoDesdeUltDisparo.resize(enemigosModelo.size(), 0.0f);
    }

    const float DT            = DT_IA;
    const float MARGEN_DIST   = 20.0f;
    const float DIST_MIN_ALIADO = 150.0f;   // distancia minima respecto a comandante / fusilero / medico

    for (std::size_t i = 0; i < enemigosModelo.size(); ++i) {

        Personaje* enemigo = enemigosModelo[i];
        PersonajeGrafico* sprite = (i < enemigosSprites.size()) ? enemigosSprites[i] : nullptr;

        if (!enemigo || !sprite) continue;
        if (enemigo->getSalud() <= 0) {
            sprite->actualizarDesdeModelo(DT);
            continue;
        }

        float ex = enemigo->getX();
        float ey = enemigo->getY();

        // puede ser comandante, fusilero o medico
        Personaje* candidatos[3];
        int nCandidatos = 0;

        if (comandanteModelo && comandanteModelo->getSalud() > 0)
            candidatos[nCandidatos++] = comandanteModelo;
        if (fusileroModelo && fusileroModelo->getSalud() > 0)
            candidatos[nCandidatos++] = fusileroModelo;
        if (medicoModelo && medicoModelo->getSalud() > 0)
            candidatos[nCandidatos++] = medicoModelo;

        Personaje* objetivoDisparo = nullptr;
        float objX = cx;
        float objY = cy;

        if (nCandidatos > 0) {
            float mejorDist2 = 1e9f;
            for (int k = 0; k < nCandidatos; ++k) {
                float tx = candidatos[k]->getX();
                float ty = candidatos[k]->getY();
                float ddx = tx - ex;
                float ddy = ty - ey;
                float d2  = ddx*ddx + ddy*ddy;
                if (d2 < mejorDist2) {
                    mejorDist2     = d2;
                    objetivoDisparo = candidatos[k];
                    objX = tx;
                    objY = ty;
                }
            }
        } else {
            objetivoDisparo = comandanteModelo;
            objX = cx;
            objY = cy;
        }

        float dx = cx - ex;
        float dy = cy - ey;
        float distCmd = std::sqrt(dx*dx + dy*dy);
        if (distCmd < 1.0f) distCmd = 1.0f;

        float dxDisp = objX - ex;
        float dyDisp = objY - ey;
        float distDisp = std::sqrt(dxDisp*dxDisp + dyDisp*dyDisp);
        if (distDisp < 1.0f) distDisp = 1.0f;

        bool esFrancotirador = (dynamic_cast<Francotirador*>(enemigo)    != nullptr);
        bool esGranadero     = (dynamic_cast<SoldadoGranadero*>(enemigo) != nullptr);

        float rangoVision   = 0.0f;
        float distPref      = 0.0f;
        float velMovimiento = 0.0f;
        float rangoDispMin  = 0.0f;
        float rangoDispMax  = 0.0f;
        float intervaloDisp = 0.0f;
        float velProyectil  = 0.0f;

        if (nivel) {
            nivel->obtenerParametrosIAEnemigo(
                enemigo,
                rangoVision,
                distPref,
                velMovimiento,
                rangoDispMin,
                rangoDispMax,
                intervaloDisp,
                velProyectil
                );
        } else {
            // Valores por defecto
            rangoVision   = 420.0f;
            distPref      = 220.0f;
            velMovimiento = 80.0f;
            rangoDispMin  = 0.0f;
            rangoDispMax  = 360.0f;
            intervaloDisp = 1.2f;
            velProyectil  = 260.0f;
        }


        auto puedeMoverA = [this, sprite, enemigo, i](float mx, float my)->bool {

            float oldX = enemigo->getX();
            float oldY = enemigo->getY();

            enemigo->setPosicion(mx, my);
            sprite->actualizarDesdeModelo();

            bool choca = false;

            // Obstaculos
            for (QGraphicsItem* ob : obstaculos) {
                if (!ob) continue;
                if (sprite->collidesWithItem(ob)) {
                    choca = true;
                    break;
                }
            }

            // No pasar por el comandante
            if (!choca && comandanteSprite &&
                sprite->collidesWithItem(comandanteSprite)) {
                choca = true;
            }

            // No pasar por fusilero y medico aliados
            if (!choca && fusileroSprite &&
                sprite->collidesWithItem(fusileroSprite)) {
                choca = true;
            }
            if (!choca && medicoSprite &&
                sprite->collidesWithItem(medicoSprite)) {
                choca = true;
            }

            // No chocar con otros enemigos
            if (!choca) {
                for (std::size_t j = 0; j < enemigosSprites.size(); ++j) {
                    if (j == i) continue;
                    PersonajeGrafico* otro = enemigosSprites[j];
                    if (!otro) continue;
                    if (sprite->collidesWithItem(otro)) {
                        choca = true;
                        break;
                    }
                }
            }

            enemigo->setPosicion(oldX, oldY);
            sprite->actualizarDesdeModelo();
            return !choca;
        };

        // Moviento
        if (!esFrancotirador) {
            // Enemigo normal / granadero intenta acercarse al COMANDANTE

            // Vector hacia el comandante
            float nx = dx / distCmd;
            float ny = dy / distCmd;

            float paso = velMovimiento * DT;

            // Separacion entre enemigos
            const float DIST_SEP   = 50.0f;
            const float FUERZA_SEP = 0.4f;

            float sepX = 0.0f;
            float sepY = 0.0f;

            for (std::size_t j = 0; j < enemigosModelo.size(); ++j) {
                if (j == i) continue;

                Personaje* otro = enemigosModelo[j];
                if (!otro) continue;
                if (otro->getSalud() <= 0) continue;

                float ox = otro->getX();
                float oy = otro->getY();

                float ddx = ex - ox;
                float ddy = ey - oy;
                float d2  = ddx*ddx + ddy*ddy;
                if (d2 < 1.0f) continue;

                float d = std::sqrt(d2);

                if (d < DIST_SEP) {
                    float inv = 1.0f / d;
                    sepX += (ddx * inv) * (DIST_SEP - d) * FUERZA_SEP;
                    sepY += (ddy * inv) * (DIST_SEP - d) * FUERZA_SEP;
                }
            }

            // Movimiento deseado
            float movX = nx * paso + sepX;
            float movY = ny * paso + sepY;

            float candidatoX = ex + movX;
            float candidatoY = ey + movY;

            auto distAl = [&](Personaje* pj)->float {
                if (!pj || pj->getSalud() <= 0) return 1e6f;
                float ax = pj->getX();
                float ay = pj->getY();
                float ddx = candidatoX - ax;
                float ddy = candidatoY - ay;
                return std::sqrt(ddx*ddx + ddy*ddy);
            };

            float minAliado = distAl(comandanteModelo);
            float dF        = distAl(fusileroModelo);
            float dM        = distAl(medicoModelo);

            if (dF < minAliado) minAliado = dF;
            if (dM < minAliado) minAliado = dM;

            if (minAliado >= DIST_MIN_ALIADO) {

                bool movido = false;

                if (puedeMoverA(candidatoX, candidatoY)) {
                    enemigo->setPosicion(candidatoX, candidatoY);
                    movido = true;
                } else {
                    if (puedeMoverA(ex + movX, ey)) {
                        enemigo->setPosicion(ex + movX, ey);
                        movido = true;
                    }
                    else if (puedeMoverA(ex, ey + movY)) {
                        enemigo->setPosicion(ex, ey + movY);
                        movido = true;
                    }
                }
            }
        }
        else {
            float nx = dx / distCmd;
            float ny = dy / distCmd;

            float movX = 0.0f;
            float movY = 0.0f;

            // Si esta demasiado LEJOS, se acerca
            if (distCmd > distPref + MARGEN_DIST) {
                float paso = velMovimiento * DT;
                movX = nx * paso;
                movY = ny * paso;
            }
            // Si esta demasiado CERCA, se aleja
            else if (distCmd < distPref - MARGEN_DIST) {
                float paso = velMovimiento * DT;
                movX = -nx * paso;
                movY = -ny * paso;
            }

            if (movX != 0.0f || movY != 0.0f) {
                float candidatoX = ex + movX;
                float candidatoY = ey + movY;

                bool movido = false;

                // Intento movimiento completo
                if (puedeMoverA(candidatoX, candidatoY)) {
                    enemigo->setPosicion(candidatoX, candidatoY);
                    movido = true;
                } else {
                    // Intento solo X
                    if (puedeMoverA(ex + movX, ey)) {
                        enemigo->setPosicion(ex + movX, ey);
                        movido = true;
                    }
                    // Intento solo Y
                    else if (puedeMoverA(ex, ey + movY)) {
                        enemigo->setPosicion(ex, ey + movY);
                        movido = true;
                    }
                }
            }
        }

        // Disparo
        tiempoDesdeUltDisparo[i] += DT;

        if (objetivoDisparo &&
            distDisp <= rangoVision &&
            hayLineaVision(ex, ey, objX, objY) &&
            tiempoDesdeUltDisparo[i] >= intervaloDisp)
        {
            float angulo = std::atan2(dyDisp, dxDisp);
            float angDeg = angulo * 180.0f / 3.14159265f;
            if (angDeg < 0.0f) {
                angDeg += 360.0f;
            }

            bool dentroRangoAngular = false;
            if (rangoDispMin <= rangoDispMax) {
                dentroRangoAngular = (angDeg >= rangoDispMin && angDeg <= rangoDispMax);
            } else {
                dentroRangoAngular =
                    (angDeg >= rangoDispMin && angDeg <= 360.0f) ||
                    (angDeg >= 0.0f && angDeg <= rangoDispMax);
            }

            if (dentroRangoAngular) {
                tiempoDesdeUltDisparo[i] = 0.0f;

                if (esGranadero) {
                    // GRANADA
                    float tiempoExpl = 1.2f;
                    float radio      = 70.0f;
                    int   dano       = enemigo->getDanoBase() * 2;

                    granadasX.push_back(objX);
                    granadasY.push_back(objY);
                    granadasTiempo.push_back(tiempoExpl);
                    granadasRadio.push_back(radio);
                    granadasDano.push_back(dano);

                    QGraphicsEllipseItem* marcaEllipse = escena->addEllipse(
                        objX - radio, objY - radio,
                        2*radio, 2*radio,
                        QPen(QColor(255, 0, 0, 200)),
                        QBrush(QColor(255, 0, 0, 60))
                        );
                    granadasItems.push_back(marcaEllipse);
                }
                else {
                    // BALA normal hacia el objetivo elegido
                    float ndx = dxDisp / distDisp;
                    float ndy = dyDisp / distDisp;

                    Proyectil* p = new Proyectil(
                        enemigo->getX(),
                        enemigo->getY(),
                        ndx * velProyectil,
                        ndy * velProyectil,
                        enemigo->getDanoBase(),
                        Proyectil::TIPO_BALA,
                        false
                        );
                    p->setTiempoVidaMax(3.0f);

                    ProyectilGrafico* pg = new ProyectilGrafico(p);
                    pg->setZValue(5);
                    escena->addItem(pg);

                    proyectilesModelo.push_back(p);
                    proyectilesGraficos.push_back(pg);

                    if (esFrancotirador) {
                        sfxDisparoSniper.play();
                    } else {
                        sfxDisparo.play();
                    }

                    if (sprite) {
                        sprite->notificarDisparo(0.15f);
                    }
                }
            }
        }

        sprite->actualizarDesdeModelo(DT);
    }

    actualizarIAAliados();
}

//   Proyectiles
void MainWindow::actualizarProyectiles()
{
    if (!escena) return;

    QRectF bounds = escena->sceneRect();
    const float dt = DT_MOVIMIENTO;

    //  Granadas
    for (int i = static_cast<int>(granadasTiempo.size()) - 1; i >= 0; --i) {
        granadasTiempo[i] -= dt;

        if (granadasTiempo[i] <= 0.0f) {
            float gx    = granadasX[i];
            float gy    = granadasY[i];
            float radio = granadasRadio[i];
            int   dano  = granadasDano[i];

            sfxExplosion.play();

            // Dano al comandante si esta dentro del radio
            if (comandanteModelo && comandanteSprite && nivel) {
                nivel->aplicarExplosionGranada(
                    comandanteModelo,
                    gx,
                    gy,
                    radio,
                    dano
                    );

                comandanteSprite->actualizarDesdeModelo();
                actualizarHUD();

                if (comandanteModelo->getSalud() <= 0) {
                    comandanteSprite->setOpacity(0.3);
                    nivelCompletado = true;

                    if (timerMovimiento)  timerMovimiento->stop();
                    if (timerProyectiles) timerProyectiles->stop();
                    if (timerIAEnemigos)  timerIAEnemigos->stop();

                    mostrarMenuDerrota();
                    return;
                }
            }

            // Borrar marca visual si existe
            if (i < static_cast<int>(granadasItems.size())) {
                QGraphicsItem* marca = granadasItems[i];
                if (marca) {
                    if (marca->scene()) {
                        marca->scene()->removeItem(marca);
                    }
                    delete marca;
                }
                granadasItems.erase(granadasItems.begin() + i);
            }

            if (!explosionFrames.empty() && escena) {
                QGraphicsPixmapItem* boom = new QGraphicsPixmapItem();
                boom->setZValue(6);

                boom->setPixmap(explosionFrames[0]);

                float escala = 0.4f;
                boom->setScale(escala);

                QSize s = explosionFrames[0].size();
                boom->setOffset(- (s.width()  * escala) / 2.0,
                                - (s.height() * escala) / 2.0);
                boom->setPos(gx, gy);

                escena->addItem(boom);

                explosionesItems.push_back(boom);
                explosionesTiempo.push_back(0.0f);
            }

            granadasX.erase(granadasX.begin() + i);
            granadasY.erase(granadasY.begin() + i);
            granadasTiempo.erase(granadasTiempo.begin() + i);
            granadasRadio.erase(granadasRadio.begin() + i);
            granadasDano.erase(granadasDano.begin() + i);
        }
    }


    //  Balas normales
    if (proyectilesModelo.empty()) return;

    for (int i = static_cast<int>(proyectilesModelo.size()) - 1; i >= 0; --i) {
        Proyectil*        p  = proyectilesModelo[i];
        ProyectilGrafico* pg = proyectilesGraficos[i];

        p->actualizar(dt);
        pg->actualizarDesdeModelo();

        bool impacto = false;

        // Proyectiles aliados - danan ENEMIGOS
        if (p->getDeAliado()) {
            for (std::size_t j = 0; j < enemigosSprites.size(); ++j) {
                Personaje*        enemigo = enemigosModelo[j];
                PersonajeGrafico* spr     = enemigosSprites[j];
                if (!enemigo || !spr) continue;
                if (enemigo->getSalud() <= 0) continue;

                if (pg->collidesWithItem(spr)) {
                    if (nivel) {
                        nivel->aplicarImpactoProyectil(enemigo, p->getDano());
                    }

                    spr->actualizarDesdeModelo();
                    p->setActivo(false);
                    impacto = true;

                    if (enemigo->getSalud() <= 0) {
                        spr->setOpacity(0.3);
                    }
                    break;
                }
            }
        } else {
            // Proyectiles enemigos - danan comandante o aliados
            if (comandanteSprite && comandanteModelo &&
                pg->collidesWithItem(comandanteSprite)) {

                if (nivel) {
                    nivel->aplicarImpactoProyectil(comandanteModelo, p->getDano());
                }

                comandanteSprite->actualizarDesdeModelo();
                actualizarHUD();
                p->setActivo(false);
                impacto = true;

                if (comandanteModelo->getSalud() <= 0) {
                    comandanteSprite->setOpacity(0.3);
                    nivelCompletado = true;

                    if (timerMovimiento)  timerMovimiento->stop();
                    if (timerProyectiles) timerProyectiles->stop();
                    if (timerIAEnemigos)  timerIAEnemigos->stop();

                    mostrarMenuDerrota();
                    return;
                }
            }

            else if (fusileroSprite && fusileroModelo &&
                     fusileroModelo->getSalud() > 0 &&
                     pg->collidesWithItem(fusileroSprite)) {

                if (nivel) {
                    nivel->aplicarImpactoProyectil(fusileroModelo, p->getDano());
                }

                fusileroSprite->actualizarDesdeModelo();
                actualizarHUD();
                p->setActivo(false);
                impacto = true;

                if (fusileroModelo->getSalud() <= 0) {
                    fusileroSprite->setOpacity(0.3);
                }
            }
            else if (medicoSprite && medicoModelo &&
                     medicoModelo->getSalud() > 0 &&
                     pg->collidesWithItem(medicoSprite)) {

                if (nivel) {
                    nivel->aplicarImpactoProyectil(medicoModelo, p->getDano());
                }

                medicoSprite->actualizarDesdeModelo();
                actualizarHUD();
                p->setActivo(false);
                impacto = true;

                if (medicoModelo->getSalud() <= 0) {
                    medicoSprite->setOpacity(0.3);
                }
            }
        }

        // Colision con obstaculos
        if (!impacto) {
            for (QGraphicsItem* ob : obstaculos) {
                if (!ob) continue;
                if (pg->collidesWithItem(ob)) {
                    p->setActivo(false);
                    impacto = true;
                    break;
                }
            }
        }

        // Fuera de la escena
        QPointF pos = pg->pos();
        bool fuera =
            pos.x() < bounds.left()  - 50.0 ||
            pos.x() > bounds.right() + 50.0 ||
            pos.y() < bounds.top()   - 50.0 ||
            pos.y() > bounds.bottom()+ 50.0;

        if (!p->estaActivo() || impacto || fuera) {
            escena->removeItem(pg);
            delete pg;
            delete p;
            proyectilesModelo.erase(proyectilesModelo.begin() + i);
            proyectilesGraficos.erase(proyectilesGraficos.begin() + i);
        }
    }

    //  Animar explosiones de granada
    if (!explosionesItems.empty() && !explosionFrames.empty()) {

        const float duracionTotal = 0.4f;
        const int   numFrames     = explosionFrames.size();
        const float durFrame      = duracionTotal / numFrames;

        for (int i = static_cast<int>(explosionesItems.size()) - 1; i >= 0; --i) {
            QGraphicsPixmapItem* boom = explosionesItems[i];
            if (!boom) {
                explosionesItems.erase(explosionesItems.begin() + i);
                explosionesTiempo.erase(explosionesTiempo.begin() + i);
                continue;
            }

            explosionesTiempo[i] += dt;
            float t = explosionesTiempo[i];

            if (t >= duracionTotal) {
                if (boom->scene()) {
                    boom->scene()->removeItem(boom);
                }
                delete boom;

                explosionesItems.erase(explosionesItems.begin() + i);
                explosionesTiempo.erase(explosionesTiempo.begin() + i);
            } else {
                int frameIndex = static_cast<int>(t / durFrame);
                if (frameIndex < 0) frameIndex = 0;
                if (frameIndex >= numFrames) frameIndex = numFrames - 1;

                boom->setPixmap(explosionFrames[frameIndex]);
            }
        }
    }

}

//   Criterio de victoria
void MainWindow::verificarVictoria()
{
    if (nivelCompletado) return;
    if (!zonaObjetivo || !comandanteSprite) return;

    // El comandante debe estar en la zona objetivo
    if (!comandanteSprite->collidesWithItem(zonaObjetivo))
        return;

    // Todos los enemigos vivos deben estar muertos
    bool todosEnemigosMuertos = true;
    for (std::size_t i = 0; i < enemigosModelo.size(); ++i) {
        Personaje* e = enemigosModelo[i];
        if (!e) continue;
        if (e->getSalud() > 0) {
            todosEnemigosMuertos = false;
            break;
        }
    }
    if (!todosEnemigosMuertos) return;

    // Aliados vivos deben estar dentro de la zona
    bool aliadosOk = true;

    if (fusileroModelo && fusileroModelo->getSalud() > 0 && fusileroSprite) {
        if (!fusileroSprite->collidesWithItem(zonaObjetivo)) {
            aliadosOk = false;
        }
    }
    if (medicoModelo && medicoModelo->getSalud() > 0 && medicoSprite) {
        if (!medicoSprite->collidesWithItem(zonaObjetivo)) {
            aliadosOk = false;
        }
    }

    if (!aliadosOk) return;

    nivelCompletado = true;

    if (timerMovimiento)  timerMovimiento->stop();
    if (timerProyectiles) timerProyectiles->stop();
    if (timerIAEnemigos)  timerIAEnemigos->stop();

    QMessageBox::information(
        this,
        "Nivel 1 completado",
        "Has eliminado a todos los enemigos y tu escuadrón llegó a la cima de la montaño.\n¡Nivel 1 superado!"
        );

    auto *nivel2 = new Nivel2Window();
    nivel2->setWindowOpacity(0.0);
    nivel2->showMaximized();

    QPropertyAnimation *animIn = new QPropertyAnimation(nivel2, "windowOpacity");
    animIn->setDuration(450);
    animIn->setStartValue(0.0);
    animIn->setEndValue(1.0);
    animIn->start(QAbstractAnimation::DeleteWhenStopped);

    // Cerrar la ventana del Nivel 1
    this->close();
}

//   Crear aliados
void MainWindow::crearAliados()
{
    if (!nivel || !escena) return;

    // Obtener modelos desde el nivel
    fusileroModelo = nivel->getFusilero();
    medicoModelo   = nivel->getMedico();

    //  Fusilero aliado
    if (fusileroModelo) {

        fusileroModelo->setPosicion(44.0f, 270.0f);

        fusileroSprite = new PersonajeGrafico(fusileroModelo);
        fusileroSprite->setZValue(0);
        fusileroSprite->setPos(44.0f, 270.0f);
        escena->addItem(fusileroSprite);
    }

    //  Medico aliado
    if (medicoModelo) {

        medicoModelo->setPosicion(20.0f, 267.0f);

        medicoSprite = new PersonajeGrafico(medicoModelo);
        medicoSprite->setZValue(0);
        medicoSprite->setPos(27.0f, 203.0f);
        escena->addItem(medicoSprite);
    }
}

//   Crear enemigos
void MainWindow::crearEnemigos()
{
    if (!escena) return;

    for (PersonajeGrafico* spr : enemigosSprites) {
        if (spr) {
            escena->removeItem(spr);
            delete spr;
        }
    }
    enemigosSprites.clear();

    for (Personaje* mod : enemigosModelo) {
        delete mod;
    }
    enemigosModelo.clear();

    auto agregarEnemigo = [&](Personaje* modelo,
                                 float x, float y,
                                 qreal zValue = 0.0)
    {
        if (!modelo) return;

        modelo->setPosicion(x, y);

        PersonajeGrafico* sprite = new PersonajeGrafico(modelo);
        sprite->setZValue(zValue);
        sprite->setPos(x, y);
        escena->addItem(sprite);

        enemigosModelo.push_back(modelo);
        enemigosSprites.push_back(sprite);
    };

    agregarEnemigo(new SoldadoEnemigo(), 701.0f, 150.0f);
    agregarEnemigo(new SoldadoEnemigo(), 711.0f, 249.0f);
    agregarEnemigo(new SoldadoEnemigo(), 720.0f, 356.0f);

    agregarEnemigo(new SoldadoEnemigo(), 1045.0f, 381.0f);
    agregarEnemigo(new SoldadoEnemigo(), 1046.0f, 116.0f);

    agregarEnemigo(new Francotirador(), 1351.0f, 136.0f);
    agregarEnemigo(new Francotirador(), 1460.0f, 414.0f);

    agregarEnemigo(new SoldadoGranadero(), 1766.0, 270.0f);
}

//   Cartel inicio
void MainWindow::mostrarCartelInicioNivel()
{
    QWidget *view = ui->graphicsView->viewport();

    QLabel *cartel = new QLabel(view);
    cartel->setText(
        "Nivel 1 – Camino a Old Baldy\n\n"
        "Objetivo:\n"
        "• Avanza con tu escuadrón hasta la zona aliada.\n"
        "• Evita granadas y elimina patrullas enemigas.\n\n"
        "Controles:\n"
        "WASD: Moverse\n"
        "Espacio: Disparar\n"
        "V: Atacar\n"
        "C: Cubrirse"
        );

    cartel->setStyleSheet(
        "QLabel {"
        " background-color: rgba(0, 0, 0, 180);"
        " color: white;"
        " padding: 16px;"
        " border-radius: 14px;"
        " font-size: 14px;"
        " font-weight: bold;"
        "}"
        );

    cartel->setAlignment(Qt::AlignCenter);
    cartel->setWordWrap(true);
    cartel->adjustSize();

    int minW = 420;
    if (cartel->width() < minW)
        cartel->setFixedWidth(minW);
    cartel->adjustSize();

    int w = view->width();
    int h = view->height();
    int x = (w - cartel->width()) / 2;
    int y = (h - cartel->height()) / 2;

    cartel->move(x, y);
    cartel->show();
    cartel->raise();

    QPropertyAnimation *anim = new QPropertyAnimation(cartel, "windowOpacity");
    anim->setDuration(5000);
    anim->setStartValue(1.0);
    anim->setEndValue(0.0);

    connect(anim, &QPropertyAnimation::finished, cartel, &QLabel::deleteLater);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);

    static bool mostrado = false;
    if (!mostrado) {
        mostrado = true;
        mostrarCartelInicioNivel();
    }
}

void MainWindow::mostrarMenuDerrota()
{
    QWidget *view = ui->graphicsView->viewport();

    QWidget *panel = new QWidget(view);
    panel->setStyleSheet(
        "QWidget {"
        "  background-color: rgba(0, 0, 0, 200);"
        "  border-radius: 18px;"
        "}"
        );

    QVBoxLayout *layout = new QVBoxLayout(panel);
    layout->setContentsMargins(30, 25, 30, 25);
    layout->setSpacing(15);

    QLabel *titulo = new QLabel("Has sido derrotado", panel);
    titulo->setStyleSheet(
        "QLabel {"
        "  background-color: transparent;"
        "  color: white;"
        "  font-size: 20px;"
        "  font-weight: bold;"
        "}"
        );
    titulo->setAlignment(Qt::AlignCenter);

    QLabel *sub = new QLabel("¿Qué deseas hacer?", panel);
    sub->setStyleSheet(
        "QLabel {"
        "  background-color: transparent;"
        "  color: white;"
        "  font-size: 14px;"
        "}"
        );
    sub->setAlignment(Qt::AlignCenter);

    QPushButton *btnReintentar = new QPushButton("Reiniciar nivel", panel);
    QPushButton *btnMenu       = new QPushButton("Volver al menu", panel);

    QString estiloBoton =
        "QPushButton {"
        "  background-color: #ffaa00;"
        "  color: black;"
        "  padding: 6px 14px;"
        "  border-radius: 8px;"
        "  font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "  background-color: #ffcc33;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #cc8800;"
        "}";

    btnReintentar->setStyleSheet(estiloBoton);
    btnMenu->setStyleSheet(estiloBoton);

    btnReintentar->setCursor(Qt::PointingHandCursor);
    btnMenu->setCursor(Qt::PointingHandCursor);

    layout->addWidget(titulo);
    layout->addWidget(sub);
    layout->addSpacing(8);
    layout->addWidget(btnReintentar);
    layout->addWidget(btnMenu);

    panel->setLayout(layout);
    panel->adjustSize();

    int w = view->width();
    int h = view->height();
    int x = (w - panel->width()) / 2;
    int y = (h - panel->height()) / 2;
    panel->move(x, y);

    panel->show();
    panel->raise();


    // Reiniciar nivel
    connect(btnReintentar, &QPushButton::clicked, this, [this, panel]() {
        panel->deleteLater();
        reiniciarNivel();
    });

    // Volver al menu
    connect(btnMenu, &QPushButton::clicked, this, [this, panel]() {
        panel->deleteLater();

        QWidget *menuWidget = qobject_cast<QWidget*>(parent());
        if (!menuWidget) {
            MenuWindow *nuevoMenu = new MenuWindow();
            nuevoMenu->show();
        } else {
            menuWidget->show();
        }

        this->close();
    });
}

//   Reiniciar nivel
void MainWindow::reiniciarNivel()
{
    QWidget *p = parentWidget();
    MainWindow *nuevo = new MainWindow(p);
    nuevo->show();

    this->close();
}
