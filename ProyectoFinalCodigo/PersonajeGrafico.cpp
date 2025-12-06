#include "PersonajeGrafico.h"
#include "Personaje.h"
#include "Comandante.h"
#include "SoldadoEnemigo.h"
#include "SoldadoGranadero.h"
#include "Francotirador.h"
#include "Fusilero.h"
#include "Medico.h"

#include <QPainter>
#include <QLineF>
#include <QtMath>

PersonajeGrafico::PersonajeGrafico(Personaje* modelo,
                                   QGraphicsItem* parent)
    : QObject(),
    QGraphicsPixmapItem(parent),
    modelo(modelo),
    tiempoAnim(0.0f),
    estaDisparando(false),
    tiempoRestanteDisparo(0.0f),
    ultimaPosicion(0.0, 0.0)
{
    cargarSprites();

    if (!frameQuieto.isNull()) {
        setPixmap(frameQuieto);
    } else if (!framesMovimiento.isEmpty()) {
        setPixmap(framesMovimiento[0]);
    }

    if (modelo) {
        setPos(modelo->getX(), modelo->getY());
        ultimaPosicion = pos();
    }

    setTransformOriginPoint(boundingRect().center());
}

void PersonajeGrafico::cargarSprites()
{
    framesMovimiento.clear();
    frameDisparo = QPixmap();
    frameQuieto  = QPixmap();

    // Comandante
    if (modelo && dynamic_cast<Comandante*>(modelo)) {
        cargarSpritesComandante();
    }
    // Enemigo fusilero
    else if (modelo && dynamic_cast<SoldadoEnemigo*>(modelo)) {
        cargarSpritesEnemigoFusilero();
    }
    // Granadero
    else if (modelo && dynamic_cast<SoldadoGranadero*>(modelo)) {
        cargarSpritesGranadero();
    }
    // Francotirador (solo idle + shoot)
    else if (modelo && dynamic_cast<Francotirador*>(modelo)) {
        cargarSpritesFrancotirador();
    }
    // Fusilero aliado
    else if (modelo && dynamic_cast<Fusilero*>(modelo)) {
        cargarSpritesFusileroAliado();
    }
    // Medico aliado
    else if (modelo && dynamic_cast<Medico*>(modelo)) {
        cargarSpritesMedico();
    }
    else {
        const int ancho = 40;
        const int alto  = 40;
        QPixmap pix(ancho, alto);
        pix.fill(Qt::transparent);

        QPainter p(&pix);
        if (!modelo) {
            p.setBrush(Qt::gray);
        } else if (modelo->getEsAliado()) {
            p.setBrush(Qt::green);
        } else {
            p.setBrush(Qt::red);
        }
        p.setPen(Qt::black);
        p.drawRect(0, 0, ancho, alto);
        p.end();

        frameQuieto = pix;
    }

    if (frameQuieto.isNull() && !framesMovimiento.isEmpty()) {
        frameQuieto = framesMovimiento[0];
    }
}

void PersonajeGrafico::cargarSpritesComandante()
{
    framesMovimiento.clear();

    for (int i = 0; i < 8; ++i) {
        QString ruta = QString(":/SpriteComandante/Comandante%1").arg(i);
        QPixmap f;
        if (f.load(ruta)) {
            framesMovimiento.push_back(f);
        }
    }

    QPixmap shoot;
    if (shoot.load(":/SpriteComandante/Comandante9")) {
        frameDisparo = shoot;
    }

    if (!framesMovimiento.isEmpty()) {
        frameQuieto = framesMovimiento[0];
    }
}

void PersonajeGrafico::cargarSpritesEnemigoFusilero()
{
    framesMovimiento.clear();

    for (int i = 0; i < 8; ++i) {
        QString ruta = QString(":/SpriteEnemigo1/EnemigoFusilero%1").arg(i);
        QPixmap f;
        if (f.load(ruta)) {
            framesMovimiento.push_back(f);
        }
    }

    QPixmap shoot;
    if (shoot.load(":/SpriteEnemigo1/EnemigoFusilero9")) {
        frameDisparo = shoot;
    }

    if (!framesMovimiento.isEmpty()) {
        frameQuieto = framesMovimiento[0];
    }
}

void PersonajeGrafico::cargarSpritesGranadero()
{
    framesMovimiento.clear();

    for (int i = 0; i < 8; ++i) {
        QString ruta = QString(":/SpriteGranadero/Granadero%1").arg(i);
        QPixmap f;
        if (f.load(ruta)) {
            framesMovimiento.push_back(f);
        }
    }

    QPixmap shoot;
    if (shoot.load(":/SpriteGranadero/Granadero8")) {
        frameDisparo = shoot;
    }

    if (!framesMovimiento.isEmpty()) {
        frameQuieto = framesMovimiento[0];
    }
}

void PersonajeGrafico::cargarSpritesFrancotirador()
{
    framesMovimiento.clear();

    QPixmap idle;
    if (idle.load(":/SpriteFrancotirador/Francotirador1")) {
        frameQuieto = idle;
    }

    QPixmap shoot;
    if (shoot.load(":/SpriteFrancotirador/Francotirador2")) {
        frameDisparo = shoot;
    }

    if (!frameQuieto.isNull()) {
        setPixmap(frameQuieto);
    }
}

void PersonajeGrafico::cargarSpritesFusileroAliado()
{
    framesMovimiento.clear();

    for (int i = 0; i < 8; ++i) {
        QString ruta = QString(":/SpriteSoldado/SoldadoRegular%1").arg(i);
        QPixmap f;
        if (f.load(ruta)) {
            framesMovimiento.push_back(f);
        }
    }

    QPixmap shoot;
    if (shoot.load(":/SpriteSoldado/SoldadoRegular9")) {
        frameDisparo = shoot;
    }

    if (!framesMovimiento.isEmpty()) {
        frameQuieto = framesMovimiento[0];
    }
}

void PersonajeGrafico::cargarSpritesMedico()
{
    framesMovimiento.clear();

    for (int i = 0; i < 8; ++i) {
        QString ruta = QString(":/SpriteMedico/Medico%1").arg(i);
        QPixmap f;
        if (f.load(ruta)) {
            framesMovimiento.push_back(f);
        }
    }

    if (!framesMovimiento.isEmpty()) {
        frameQuieto = framesMovimiento[0];
    }
}

void PersonajeGrafico::actualizarDesdeModelo()
{
    actualizarDesdeModelo(0.0f);
}

void PersonajeGrafico::actualizarDesdeModelo(float dt)
{
    if (!modelo) return;

    QPointF posAnterior = ultimaPosicion;

    setPos(modelo->getX(), modelo->getY());

    if (modelo->getSalud() <= 0) {
        setOpacity(0.3);
    } else {
        setOpacity(1.0);
    }

    animar(dt, posAnterior);

    ultimaPosicion = pos();
}

void PersonajeGrafico::animar(float dt, const QPointF& posAnterior)
{
    if (modelo && dynamic_cast<Francotirador*>(modelo)) {

        if (estaDisparando) {
            if (!frameDisparo.isNull()) {
                setPixmap(frameDisparo);
            } else if (!frameQuieto.isNull()) {
                setPixmap(frameQuieto);
            }

            if (dt > 0.0f) {
                tiempoRestanteDisparo -= dt;
                if (tiempoRestanteDisparo <= 0.0f) {
                    estaDisparando = false;
                }
            }
        } else {
            if (!frameQuieto.isNull()) {
                setPixmap(frameQuieto);
            }
        }
        return;
    }

    if (framesMovimiento.isEmpty() && frameQuieto.isNull())
        return;

    QLineF seg(posAnterior, pos());
    bool seMueve = (seg.length() > 0.5f);

    if (seMueve) {
        double angulo = qAtan2(seg.dy(), seg.dx()) * 180.0 / 3.141592653;
        setRotation(angulo);
    }

    if (estaDisparando) {
        if (!frameDisparo.isNull()) {
            setPixmap(frameDisparo);
        } else if (!frameQuieto.isNull()) {
            setPixmap(frameQuieto);
        }

        if (dt > 0.0f) {
            tiempoRestanteDisparo -= dt;
            if (tiempoRestanteDisparo <= 0.0f) {
                estaDisparando = false;
            }
        }
        return;
    }

    if (seMueve && !framesMovimiento.isEmpty() && dt > 0.0f) {
        const float DUR_FRAME = 0.09f;
        tiempoAnim += dt;

        int numFrames = framesMovimiento.size();
        int index = static_cast<int>(tiempoAnim / DUR_FRAME) % numFrames;
        setPixmap(framesMovimiento[index]);
    } else {
        if (!frameQuieto.isNull()) {
            setPixmap(frameQuieto);
        } else if (!framesMovimiento.isEmpty()) {
            setPixmap(framesMovimiento[0]);
        }
    }
}

void PersonajeGrafico::notificarDisparo(float duracion)
{
    if (frameDisparo.isNull()) {
        return;
    }

    estaDisparando        = true;
    tiempoRestanteDisparo = duracion;
    tiempoAnim            = 0.0f;

    setPixmap(frameDisparo);
}

