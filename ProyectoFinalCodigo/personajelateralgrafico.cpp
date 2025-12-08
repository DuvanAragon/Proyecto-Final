#include "personajelateralgrafico.h"

#include <QTimer>
#include <QPixmap>

#include "Personaje.h"

PersonajeLateralGrafico::PersonajeLateralGrafico(Personaje *modelo,
                                                 const QString &prefijoFrames,
                                                 int numFrames,
                                                 const QString &spriteCrouchPath,
                                                 QGraphicsItem *parent)
    : QObject(),
    QGraphicsPixmapItem(parent),
    modelo(modelo),
    frameActual(0),
    timerAnim(nullptr),
    enMovimiento(false),
    agachado(false),
    tieneAgachado(false),
    mirandoDerecha(true)
{
    cargarFramesMovimiento(prefijoFrames, numFrames);
    cargarSpriteAgachado(spriteCrouchPath);

    if (!frameQuieto.isNull()) {
        setPixmap(frameQuieto);
    }

    timerAnim = new QTimer(this);
    connect(timerAnim, &QTimer::timeout,
            this, &PersonajeLateralGrafico::avanzarFrame);
    timerAnim->start(120); // 120 ms por frame
}

PersonajeLateralGrafico::~PersonajeLateralGrafico(){}

void PersonajeLateralGrafico::cargarFramesMovimiento(const QString &prefijoFrames,
                                                     int numFrames)
{
    framesMovimiento.clear();
    frameQuieto = QPixmap();

    if (prefijoFrames.isEmpty() || numFrames <= 0)
        return;


    for (int i = 1; i < numFrames; ++i) {
        QString ruta = QString("%1%2").arg(prefijoFrames).arg(i);
        QPixmap frame;
        if (frame.load(ruta)) {
            framesMovimiento.push_back(frame);
        }
    }

    if (!framesMovimiento.isEmpty()) {
        frameQuieto = framesMovimiento[0];
    }
}

void PersonajeLateralGrafico::cargarSpriteAgachado(const QString &spriteCrouchPath)
{
    frameAgachado = QPixmap();
    tieneAgachado = false;

    if (spriteCrouchPath.isEmpty())
        return;

    QPixmap crouch(spriteCrouchPath);
    if (!crouch.isNull()) {
        frameAgachado = crouch;
        tieneAgachado = true;
    }
}

void PersonajeLateralGrafico::avanzarFrame()
{
    if (framesMovimiento.isEmpty())
        return;

    if (!enMovimiento || agachado) {
        return;
    }

    frameActual = (frameActual + 1) % framesMovimiento.size();
}

void PersonajeLateralGrafico::actualizarDesdeModelo()
{
    if (!modelo)
        return;

    float escala = modelo->getEsAliado() ? 5.0f : 3.5f;

    if (!modelo->getEsAliado()) {
        float vx = modelo->getVx();
        float vy = modelo->getVy();
        bool seMueve = (vx != 0.0f || vy != 0.0f);
        setEnMovimiento(seMueve);
    }

    const QPixmap *pix = nullptr;

    if (agachado && tieneAgachado) {
        pix = &frameAgachado;
    } else if (!enMovimiento && !frameQuieto.isNull()) {
        pix = &frameQuieto;
    } else if (!framesMovimiento.isEmpty()) {
        pix = &framesMovimiento[frameActual];
    }

    if (!pix || pix->isNull())
        return;

    QPixmap display = *pix;

    if (mirandoDerecha) {
        display = display.transformed(QTransform().scale(-1.0, 1.0));
    }

    setPixmap(display);
    setScale(escala);

    qreal xPies = static_cast<qreal>(modelo->getX());
    qreal yPies = static_cast<qreal>(modelo->getY());

    qreal w = display.width()  * escala;
    qreal h = display.height() * escala;

    setPos(xPies - w / 2.0, yPies - h);
}


void PersonajeLateralGrafico::setEnMovimiento(bool mov)
{
    enMovimiento = mov;
    if (!enMovimiento) {
        frameActual = 0;
    }
}

void PersonajeLateralGrafico::setAgachado(bool crouch)
{
    agachado = crouch;
}

void PersonajeLateralGrafico::setMirandoDerecha(bool der)
{
    mirandoDerecha = der;
}
