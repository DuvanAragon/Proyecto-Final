#include "nivel2window.h"

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QSizePolicy>
#include <QPainter>
#include <QTimer>
#include <QPixmap>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QEvent>
#include <QTransform>
#include <cmath>
#include <QDebug>

#include "SoldadoGranadero.h"
#include "Proyectil.h"
#include "ProyectilGrafico.h"
#include "Nivel.h"
#include "Comandante.h"
#include "Fusilero.h"
#include "personajelateralgrafico.h"

Nivel2Window::Nivel2Window(QWidget *parent)
    : QMainWindow(parent)
    , vista(nullptr)
    , escena(nullptr)
    , fondo(nullptr)
    , sacoColliderSup(nullptr)
    , sacoColliderInf(nullptr)
    , nivel(nullptr)
    , comandanteModelo(nullptr)
    , fusileroModelo(nullptr)
    , comandanteSprite(nullptr)
    , fusileroSprite(nullptr)
    , enemigoSprite(nullptr)
    , granaderoSprite(nullptr)
    , timerLogica(nullptr)
    , izquierdaPresionada(false)
    , derechaPresionada(false)
    , agacharPresionado(false)
    , velocidad(220.0f)
    , velocidadCrouch(100.0f)
    , limiteIzq(0.0f)
    , limiteDer(0.0f)
    , limiteArriba(0.0f)
    , limiteAbajo(0.0f)
    , limiteSacoJugador(0.0f)
    , limiteSacoEnemigo(0.0f)
    , distanciaEnemigosSaco(200.0f)
    , tiempoRecargaCmd(0.0f)
    , tiempoRecargaFus(0.0f)
    , intervaloDisparoCmd(0.35f)
    , intervaloDisparoFus(0.50f)
    , dirHorizontalCmd(1)
    , cmdDisparando(false)
    , fusDisparando(false)
    , tiempoDisparoCmd(0.0f)
    , tiempoDisparoFus(0.0f)
    , tiempoNivel(0.0f)
    , duracionNivel(300.0f)
    , granaderoLanzando(false)
    , tiempoLanzarGranadero(0.0f)
    , dirHorizontalGranadero(-1)

{
    resize(1280, 720);
    setWindowTitle("Nivel 2 - Defensa de la colina");

    escena = new QGraphicsScene(this);
    vista  = new QGraphicsView(escena, this);
    setCentralWidget(vista);

    vista->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    vista->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vista->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vista->setRenderHints(QPainter::Antialiasing |
                          QPainter::SmoothPixmapTransform);

    vista->viewport()->installEventFilter(this);

    nivel = new Nivel();

    nivel->setNumeroNivel(2);

    nivel->iniciar();

    inicializarEscena();
    crearAliados();
    crearEnemigos();
    cargarAnimDisparo();

    timerLogica = new QTimer(this);
    connect(timerLogica, &QTimer::timeout, this, &Nivel2Window::actualizarLogica);
    timerLogica->start(16);

    showMaximized();

    inicializarHUD();
    actualizarHUD();
}

Nivel2Window::~Nivel2Window()
{
    // Limpiar proyectiles graficos que dependen del modelo Nivel
    for (auto &par : proyectilesNivelGraficos) {
        ProyectilGrafico* g = par.second;
        if (g) {
            if (escena) {
                escena->removeItem(g);
            }
            delete g;
        }
    }
    proyectilesNivelGraficos.clear();

    // Limpiar proyectiles propios de la ventana
    for (ProyectilGrafico* g : proyectilesGraficos) {
        if (g) {
            if (escena) escena->removeItem(g);
            delete g;
        }
    }
    proyectilesGraficos.clear();

    for (Proyectil* p : proyectilesModelo) {
        delete p;
    }
    proyectilesModelo.clear();

    for (PersonajeLateralGrafico* s : spritesEnemigos) {
        if (s) {
            if (escena) escena->removeItem(s);
            delete s;
        }
    }
    spritesEnemigos.clear();


    delete nivel;
}

void Nivel2Window::inicializarEscena()
{
    QPixmap pix;
    pix.load(":/Nivel2/FondoNivel2");

    fondo = escena->addPixmap(pix);
    fondo->setZValue(-10);
    fondo->setPos(0, 0);

    escena->setSceneRect(fondo->boundingRect());
    vista->fitInView(fondo, Qt::KeepAspectRatioByExpanding);

    QRectF r = escena->sceneRect();
    qreal ancho = r.width();
    qreal alto  = r.height();

    limiteIzq    = static_cast<float>(r.left() + 25.0f);
    limiteDer    = static_cast<float>(r.right() - 25.0f);
    limiteArriba = static_cast<float>(r.top() + 50.0f);
    limiteAbajo  = static_cast<float>(r.bottom() - 25.0f);

    {
        qreal h = 65.0;
        qreal w = 90.0;
        qreal x = r.left() + ancho * 0.55;
        qreal y = limiteAbajo - 120.0;

        QPen   penSup(Qt::NoPen);
        QBrush brushSup(Qt::NoBrush);

        sacoColliderSup = escena->addRect(
            QRectF(x - w/2.0, y, w, h),
            penSup,
            brushSup
            );
    }

    {
        qreal h = 80.0;
        qreal w = 90.0;
        qreal x = r.left() + ancho * 0.55;
        qreal y = limiteAbajo - h;

        QPen penInf(Qt::red);
        penInf.setWidth(2);
        QBrush brushInf(Qt::NoBrush);

        sacoColliderInf = escena->addRect(
            QRectF(x - w/2.0, y, w, h),
            penInf,
            brushInf
            );
    }

    if (sacoColliderInf)
    {
        QRectF rc = sacoColliderInf->rect();
        float cx = static_cast<float>(rc.center().x());

        limiteSacoJugador = cx - 32.0f;
        limiteSacoEnemigo = cx + 32.0f;
    }

    QPointF centro(r.left() + ancho * 0.5,
                   r.bottom() - alto * 0.22);
    vista->centerOn(centro);
}

void Nivel2Window::crearAliados()
{
    if (!nivel || !escena)
        return;

    comandanteModelo = nivel->getComandante();

    float xInicial = limiteIzq + 50.0f;
    float yInicial = limiteAbajo;

    if (comandanteModelo)
    {
        comandanteModelo->setPosicion(xInicial, yInicial);

        comandanteSprite = new PersonajeLateralGrafico(
            comandanteModelo,
            ":/ComandanteLateral/ComandanteLateral",
            11,
            ":/ComandanteLateral/ComandanteLateralAgachado"
            );
        escena->addItem(comandanteSprite);
        comandanteSprite->actualizarDesdeModelo();
    }
}

void Nivel2Window::actualizarMovimiento(float dt)
{
    if (!comandanteModelo)
        return;

    float xActual = comandanteModelo->getX();
    float yActual = comandanteModelo->getY();

    float v = agacharPresionado ? velocidadCrouch : velocidad;

    float dx = 0.0f;
    if (izquierdaPresionada) dx -= v;
    if (derechaPresionada)   dx += v;

    float xNuevo = xActual + dx * dt;

    if (xNuevo < limiteIzq)  xNuevo = limiteIzq;
    if (xNuevo > limiteDer)  xNuevo = limiteDer;

    if (dx > 0.0f && limiteSacoJugador > 0.0f && xNuevo > limiteSacoJugador)
        xNuevo = limiteSacoJugador;

    if (yActual < limiteArriba) yActual = limiteArriba;
    if (yActual > limiteAbajo)  yActual = limiteAbajo;

    comandanteModelo->setPosicion(xNuevo, yActual);

    if (fusileroModelo)
        fusileroModelo->setPosicion(xNuevo - 40.0f, yActual);

    bool seMueve = (dx != 0.0f);

    if (seMueve) {
        dirHorizontalCmd = (dx > 0.0f) ? 1 : -1;
    }

    if (comandanteSprite)
    {
        if (seMueve)
            comandanteSprite->setMirandoDerecha(dx > 0.0f);
        comandanteSprite->setEnMovimiento(seMueve && !agacharPresionado);
        comandanteSprite->setAgachado(agacharPresionado);
        comandanteSprite->actualizarDesdeModelo();
    }

    if (fusileroSprite)
    {
        if (seMueve)
            fusileroSprite->setMirandoDerecha(dx > 0.0f);
        fusileroSprite->setEnMovimiento(seMueve && !agacharPresionado);
        fusileroSprite->setAgachado(agacharPresionado);
        fusileroSprite->actualizarDesdeModelo();
    }
    if (nivel) {
        nivel->setJugadorAgachado(agacharPresionado);
    }
}

void Nivel2Window::actualizarLogica()
{
    const float dt = 0.016f;

    tiempoNivel += dt;

    actualizarMovimiento(dt);

    if (nivel) {
        nivel->actualizarPaso();

        const std::vector<Personaje*>& enemigosVec = nivel->getEnemigos();

        if (enemigosVec.size() != spritesEnemigos.size()) {
            crearEnemigos();
        }

        auto syncYBloquear = [&](Personaje* modelo,
                                 PersonajeLateralGrafico* sprite)
        {
            if (!modelo || !sprite) return;

            float x = modelo->getX();
            float y = modelo->getY();

            y = limiteAbajo;
            modelo->setPosicion(x, y);

            if (limiteSacoEnemigo > 0.0f) {
                float xMin = limiteSacoEnemigo + distanciaEnemigosSaco;
                if (x < xMin) {
                    x = xMin;
                    modelo->setPosicion(x, y);
                }
            }

            if (modelo->getSalud() > 0) {
                sprite->setVisible(true);
                sprite->actualizarDesdeModelo();
            } else {
                sprite->setVisible(false);
            }
        };

        std::size_t n = std::min(enemigosVec.size(), spritesEnemigos.size());

        for (std::size_t i = 0; i < n; ++i) {
            syncYBloquear(enemigosVec[i], spritesEnemigos[i]);
        }

        for (std::size_t i = 0; i < n; ++i) {
            Personaje* modelo = enemigosVec[i];
            auto* g = dynamic_cast<SoldadoGranadero*>(modelo);
            if (g && g->getLanzoEnEsteFrame()) {
                granaderoModelo = modelo;
                granaderoSprite = spritesEnemigos[i];

                iniciarAnimLanzarGranadero();
                g->resetLanzoEnEsteFrame();
            }
        }
    }

    actualizarAnimDisparo(dt);
    actualizarProyectiles(dt);
    sincronizarProyectilesNivel();
    actualizarHUD();

}

void Nivel2Window::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_A: izquierdaPresionada = true; break;
    case Qt::Key_D: derechaPresionada = true; break;
    case Qt::Key_S: agacharPresionado = true; break;
    case Qt::Key_Space:
        dispararComandante();
        dispararFusilero();
        break;
    default:
        QMainWindow::keyPressEvent(event);
        break;
    }
}

void Nivel2Window::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_A: izquierdaPresionada = false; break;
    case Qt::Key_D: derechaPresionada = false; break;
    case Qt::Key_S: agacharPresionado = false; break;
    default:
        QMainWindow::keyReleaseEvent(event);
        break;
    }
}

void Nivel2Window::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    if (vista && fondo)
    {
        vista->fitInView(fondo, Qt::KeepAspectRatioByExpanding);

        QRectF r = escena->sceneRect();
        qreal ancho = r.width();
        qreal alto  = r.height();

        QPointF centro(r.left() + ancho * 0.5,
                       r.bottom() - alto * 0.22);
        vista->centerOn(centro);
    }
}

bool Nivel2Window::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == vista->viewport() && event->type() == QEvent::Wheel)
        return true;

    return QMainWindow::eventFilter(obj, event);
}

void Nivel2Window::cargarAnimDisparo()
{
    framesDisparoCmd.clear();
    framesDisparoFus.clear();
    framesLanzarGranadero.clear();

    // Disparo comandante
    for (int i = 1; i <= 6; ++i) {
        QString ruta = QString(":/ComandanteLateralDisparo/ComandanteDisparo%1").arg(i);
        QPixmap pix(ruta);
        if (!pix.isNull()) {
            framesDisparoCmd.push_back(pix);
        }
    }

    // Disparo fusilero
    for (int i = 1; i <= 6; ++i) {
        QString ruta = QString(":/FusileroLateralDisparo/FusileroDisparo%1").arg(i);
        QPixmap pix(ruta);
        if (!pix.isNull()) {
            framesDisparoFus.push_back(pix);
        }
    }

    // Lanzar granada (granadero)
    for (int i = 1; i <= 4; ++i) {
        QString ruta = QString(":/GranaderoLateral/GranaderoLateral%1").arg(i);
        QPixmap pix(ruta);
        if (!pix.isNull()) {
            framesLanzarGranadero.push_back(pix);
        }
    }

    granaderoLanzando      = false;
    tiempoLanzarGranadero  = 0.0f;
}

void Nivel2Window::dispararComandante()
{
    if (!escena || !comandanteModelo || !comandanteSprite) return;
    if (tiempoRecargaCmd > 0.0f) return;
    if (agacharPresionado) return;

    float dir = (dirHorizontalCmd >= 0) ? 1.0f : -1.0f;

    QRectF br = comandanteSprite->boundingRect();
    qreal localX = (dir > 0.0f) ? br.right() - 6.0 : br.left() + 6.0;
    qreal localY = br.top() + br.height() * 0.2; // altura media del arma
    QPointF origen = comandanteSprite->mapToScene(QPointF(localX, localY));

    float spawnX = static_cast<float>(origen.x());
    float spawnY = static_cast<float>(origen.y());

    int dano = comandanteModelo->getDanoBase();
    const float VEL_BALA = 400.0f;

    Proyectil* p = new Proyectil(
        spawnX,
        spawnY,
        dir * VEL_BALA,
        0.0f,
        dano,
        Proyectil::TIPO_BALA,
        true
        );
    p->setTiempoVidaMax(10.0f);
    proyectilesModelo.push_back(p);

    ProyectilGrafico* pg = new ProyectilGrafico(p);
    pg->setPos(spawnX, spawnY);
    pg->setZValue(5);
    pg->setScale(2.0f);
    escena->addItem(pg);
    proyectilesGraficos.push_back(pg);

    tiempoRecargaCmd = intervaloDisparoCmd;
    cmdDisparando = true;
    tiempoDisparoCmd = 0.0f;
}

void Nivel2Window::dispararFusilero()
{
    if (!escena || !fusileroModelo || !fusileroSprite) return;
    if (tiempoRecargaFus > 0.0f) return;
    if (agacharPresionado) return;

    float dir = (dirHorizontalCmd >= 0) ? 1.0f : -1.0f;

    QRectF br = fusileroSprite->boundingRect();
    qreal localX = (dir > 0.0f) ? br.right() - 6.0 : br.left() + 6.0;
    qreal localY = br.top() + br.height() * 0.2;
    QPointF origen = fusileroSprite->mapToScene(QPointF(localX, localY));

    float spawnX = static_cast<float>(origen.x());
    float spawnY = static_cast<float>(origen.y());

    int dano = fusileroModelo->getDanoBase();
    const float VEL_BALA = 320.0f;

    Proyectil* p = new Proyectil(
        spawnX,
        spawnY,
        dir * VEL_BALA,
        0.0f,
        dano,
        Proyectil::TIPO_BALA,
        true
        );
    p->setTiempoVidaMax(10.0f);
    proyectilesModelo.push_back(p);

    ProyectilGrafico* pg = new ProyectilGrafico(p);
    pg->setPos(spawnX, spawnY);
    pg->setZValue(5);
    pg->setScale(2.0f);
    escena->addItem(pg);
    proyectilesGraficos.push_back(pg);

    tiempoRecargaFus = intervaloDisparoFus;
    fusDisparando = true;
    tiempoDisparoFus = 0.0f;
}

void Nivel2Window::actualizarHUD()
{
    if (comandanteModelo && hudVidaLabel) {
        QString textoCmd = QString("CMD: %1 / %2")
        .arg(comandanteModelo->getSalud())
            .arg(comandanteModelo->getSaludMax());
        hudVidaLabel->setText(textoCmd);
    }

    if (nivel && hudTiempoLabel) {
        int t = (int) nivel->getTiempoRestante();
        hudTiempoLabel->setText(QString("Tiempo: %1").arg(t));
    }

}

void Nivel2Window::inicializarHUD()
{
    hudVidaLabel = new QLabel(this);
    hudVidaLabel->setStyleSheet(
        "QLabel { "
        " color: white; "
        " background-color: rgba(0, 0, 0, 160); "
        " padding: 4px; "
        " border-radius: 4px; "
        "}"
        );
    hudVidaLabel->setFont(QFont("Consolas", 10));
    hudVidaLabel->move(10, 10);
    hudVidaLabel->resize(220, 24);
    hudVidaLabel->show();

    hudTiempoLabel = new QLabel(this);
    hudTiempoLabel->setStyleSheet(
        "QLabel { "
        " color: white; "
        " background-color: rgba(0, 0, 0, 160); "
        " padding: 4px; "
        " border-radius: 4px; "
        "}"
        );
    hudTiempoLabel->setGeometry(10, 70, 200, 25);
    hudTiempoLabel->setText("Tiempo: 90");
    hudTiempoLabel->show();
}

void Nivel2Window::actualizarProyectiles(float dt)
{
    if (!escena) return;

    QRectF bounds = escena->sceneRect();

    for (int i = static_cast<int>(proyectilesModelo.size()) - 1; i >= 0; --i) {
        Proyectil*        p  = proyectilesModelo[i];
        ProyectilGrafico* pg = proyectilesGraficos[i];

        if (!p || !pg) {
            if (pg) {
                escena->removeItem(pg);
                delete pg;
            }
            if (p) {
                delete p;
            }
            proyectilesModelo.erase(proyectilesModelo.begin() + i);
            proyectilesGraficos.erase(proyectilesGraficos.begin() + i);
            continue;
        }

        p->actualizar(dt);
        pg->actualizarDesdeModelo();

        if (p->getDeAliado() && nivel) {
            const std::vector<Personaje*>& enemigosVec = nivel->getEnemigos();

            auto probarImpacto = [&](Personaje* modelo,
                                     PersonajeLateralGrafico* sprite) -> bool
            {
                if (!modelo || !sprite) return false;
                if (modelo->getSalud() <= 0) return false;

                if (sprite->collidesWithItem(pg)) {
                    modelo->recibirDano(p->getDano());

                    if (modelo->getSalud() <= 0) {
                        sprite->setVisible(false);
                    } else {
                        sprite->actualizarDesdeModelo();
                    }

                    p->setActivo(false);
                    return true;
                }
                return false;
            };

            bool impacto = false;
            std::size_t n = std::min(enemigosVec.size(), spritesEnemigos.size());
            for (std::size_t k = 0; k < n; ++k) {
                impacto = probarImpacto(enemigosVec[k], spritesEnemigos[k]) || impacto;
                if (impacto) break;
            }
        } else if (!p->getDeAliado()) {
            if (comandanteModelo && comandanteSprite &&
                comandanteModelo->getSalud() > 0 &&
                comandanteSprite->collidesWithItem(pg))
            {
                comandanteModelo->recibirDano(p->getDano());
                comandanteSprite->actualizarDesdeModelo();
                actualizarHUD();
                p->setActivo(false);
            }
        }


        bool fuera = !bounds.adjusted(-50, -50, 50, 50).contains(pg->pos());

        if (!p->estaActivo() || fuera) {
            escena->removeItem(pg);
            delete pg;
            delete p;
            proyectilesModelo.erase(proyectilesModelo.begin() + i);
            proyectilesGraficos.erase(proyectilesGraficos.begin() + i);
        }
    }

    if (tiempoRecargaCmd > 0.0f) {
        tiempoRecargaCmd -= dt;
        if (tiempoRecargaCmd < 0.0f) tiempoRecargaCmd = 0.0f;
    }
    if (tiempoRecargaFus > 0.0f) {
        tiempoRecargaFus -= dt;
        if (tiempoRecargaFus < 0.0f) tiempoRecargaFus = 0.0f;
    }
}

void Nivel2Window::actualizarAnimDisparo(float dt)
{
    const float durFrame = 0.04f;

    if (cmdDisparando && comandanteSprite && !framesDisparoCmd.isEmpty()) {
        tiempoDisparoCmd += dt;
        int idx = static_cast<int>(tiempoDisparoCmd / durFrame);
        if (idx >= framesDisparoCmd.size()) {
            cmdDisparando = false;
            comandanteSprite->actualizarDesdeModelo();
        } else {
            QPixmap pix = framesDisparoCmd[idx];
            if (dirHorizontalCmd > 0) {
                pix = pix.transformed(QTransform().scale(-1.0, 1.0));
            }
            comandanteSprite->setPixmap(pix);
        }
    }

    if (fusDisparando && fusileroSprite && !framesDisparoFus.isEmpty()) {
        tiempoDisparoFus += dt;
        int idx = static_cast<int>(tiempoDisparoFus / durFrame);
        if (idx >= framesDisparoFus.size()) {
            fusDisparando = false;
            fusileroSprite->actualizarDesdeModelo();
        } else {
            QPixmap pix = framesDisparoFus[idx];
            if (dirHorizontalCmd > 0) {
                pix = pix.transformed(QTransform().scale(-1.0, 1.0));
            }
            fusileroSprite->setPixmap(pix);
        }
    }

    if (granaderoLanzando &&
        granaderoSprite &&
        !framesLanzarGranadero.isEmpty())
    {
        tiempoLanzarGranadero += dt;
        int idx = static_cast<int>(tiempoLanzarGranadero / durFrame);

        if (idx >= framesLanzarGranadero.size()) {
            granaderoLanzando     = false;
            tiempoLanzarGranadero = 0.0f;
            granaderoSprite->actualizarDesdeModelo();
        } else {
            QPixmap pix = framesLanzarGranadero[idx];

            if (dirHorizontalGranadero > 0) {
                pix = pix.transformed(QTransform().scale(-1.0, 1.0));
            }

            granaderoSprite->setPixmap(pix);
        }
    }
}

float Nivel2Window::getFactorDificultad() const
{
    if (duracionNivel <= 0.0f) return 1.0f;
    float f = tiempoNivel / duracionNivel;
    if (f < 0.0f) f = 0.0f;
    if (f > 1.0f) f = 1.0f;
    return f;
}

void Nivel2Window::crearEnemigos()
{
    if (!nivel || !escena) return;

    for (PersonajeLateralGrafico* s : spritesEnemigos) {
        if (s) {
            escena->removeItem(s);
            delete s;
        }
    }
    spritesEnemigos.clear();

    enemigoModelo   = nullptr;
    granaderoModelo = nullptr;
    enemigoSprite   = nullptr;
    granaderoSprite = nullptr;

    const std::vector<Personaje*>& enemigosVec = nivel->getEnemigos();

    for (std::size_t i = 0; i < enemigosVec.size(); ++i) {
        Personaje* modelo = enemigosVec[i];
        if (!modelo) {
            spritesEnemigos.push_back(nullptr);
            continue;
        }

        auto* sprite = new PersonajeLateralGrafico(
            modelo,
            ":/EnemigoLateral/Enemigo",
            8,
            ""
            );
        sprite->setZValue(5);
        sprite->setMirandoDerecha(false);
        escena->addItem(sprite);
        sprite->actualizarDesdeModelo();

        spritesEnemigos.push_back(sprite);

        if (!granaderoModelo) {
            if (auto* g = dynamic_cast<SoldadoGranadero*>(modelo)) {
                granaderoModelo = modelo;
                granaderoSprite = sprite;
            }
        }

        if (!enemigoModelo && !dynamic_cast<SoldadoGranadero*>(modelo)) {
            enemigoModelo = modelo;
            enemigoSprite = sprite;
        }
    }
}

void Nivel2Window::sincronizarSpritesDesdeModelo()
{
    if (comandanteSprite) {
        comandanteSprite->actualizarDesdeModelo();
    }
    if (fusileroSprite) {
        fusileroSprite->actualizarDesdeModelo();
    }

    const std::vector<Personaje*>& enemigosVec = nivel ? nivel->getEnemigos()
                                                        : std::vector<Personaje*>();
    std::size_t n = std::min(enemigosVec.size(), spritesEnemigos.size());
    for (std::size_t i = 0; i < n; ++i) {
        if (spritesEnemigos[i]) {
            spritesEnemigos[i]->actualizarDesdeModelo();
        }
    }
}

void Nivel2Window::iniciarAnimLanzarGranadero()
{
    if (!granaderoSprite || framesLanzarGranadero.isEmpty())
        return;

    granaderoLanzando     = true;
    tiempoLanzarGranadero = 0.0f;

    dirHorizontalGranadero = -1;
}

void Nivel2Window::sincronizarProyectilesNivel()
{
    if (!nivel || !escena) return;

    const auto &proys = nivel->getProyectiles();
    std::map<Proyectil*, ProyectilGrafico*> nuevos;

    const float ALTURA_BALA_ENEMIGO = 80.0f;
    const float yBalasEnemigas = limiteAbajo - ALTURA_BALA_ENEMIGO;

    for (Proyectil* p : proys)
    {
        if (!p) continue;

        ProyectilGrafico* g = nullptr;
        auto it = proyectilesNivelGraficos.find(p);

        if (it == proyectilesNivelGraficos.end()) {
            g = new ProyectilGrafico(p);
            g->setScale(2.0f);
            g->setZValue(5);
            escena->addItem(g);
        } else {
            g = it->second;
        }

        float x = p->getX();
        float y = p->getY();

        // ================================
        // BALAS ENEMIGAS → ALTURA FIJA
        // ================================
        if (p->getTipo() == Proyectil::TIPO_BALA && !p->getDeAliado())
        {
            g->setPos(x, yBalasEnemigas);
            g->setRotation(0);
        }
        // ================================
        // GRANADAS → usar física real
        // ================================
        else if (p->getTipo() == Proyectil::TIPO_GRANADA && !p->getDeAliado())
        {
            g->setPos(x, y);
            g->setRotation(0);
        }
        // ================================
        // OTROS PROYECTILES
        // ================================
        else {
            g->actualizarDesdeModelo();
        }

        // ================================
        // COLISIÓN PROYECTILES → COMANDANTE
        // ================================
        if (!p->getDeAliado() && p->estaActivo() &&
            comandanteModelo && comandanteSprite &&
            comandanteModelo->getSalud() > 0)
        {
            if (g->collidesWithItem(comandanteSprite)) {
                comandanteModelo->recibirDano(p->getDano());
                comandanteSprite->actualizarDesdeModelo();
                actualizarHUD();
                p->setActivo(false);
            }
        }

        nuevos[p] = g;
    }

    // Limpieza
    for (auto &par : proyectilesNivelGraficos) {
        if (nuevos.find(par.first) == nuevos.end()) {
            escena->removeItem(par.second);
            delete par.second;
        }
    }

    proyectilesNivelGraficos.swap(nuevos);
}
