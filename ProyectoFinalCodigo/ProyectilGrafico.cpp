#include "ProyectilGrafico.h"
#include "Proyectil.h"

#include <QPixmap>
#include <QPainter>
#include <cmath>

ProyectilGrafico::ProyectilGrafico(Proyectil* modelo,
                                   QGraphicsItem* parent)
    : QObject(),
    QGraphicsPixmapItem(parent),
    modelo(modelo)
{
    QPixmap pix;

    if (modelo && modelo->getTipo() == Proyectil::TIPO_GRANADA) {
        //  Estilo granada: círculo verde
        const int lado = 8;
        pix = QPixmap(lado, lado);
        pix.fill(Qt::transparent);

        QPainter p(&pix);
        p.setRenderHint(QPainter::Antialiasing, true);

        p.setPen(Qt::NoPen);
        p.setBrush(QColor(80, 120, 80));
        p.drawEllipse(0, 0, lado - 1, lado - 1);

        p.setBrush(QColor(180, 220, 180));
        p.drawEllipse(2, 1, lado - 4, lado - 4);

        p.end();
    } else {
        const int ancho = 10;
        const int alto  = 2;

        pix = QPixmap(ancho, alto);
        pix.fill(Qt::transparent);

        QPainter p(&pix);
        p.setRenderHint(QPainter::Antialiasing, true);

        p.setPen(Qt::NoPen);
        p.setBrush(QColor(255, 220, 0));
        p.drawRoundedRect(0, 0, ancho - 1, alto - 1, 3, 3);

        p.setBrush(QColor(255, 255, 200));
        p.drawEllipse(ancho - 5, (alto / 2) - 2, 4, 4);

        p.end();
    }

    setPixmap(pix);

    setOffset(-pix.width() / 2.0, -pix.height() / 2.0);

    if (modelo) {
        setPos(modelo->getX(), modelo->getY());
    }

    setTransformOriginPoint(boundingRect().center());
}

void ProyectilGrafico::actualizarDesdeModelo()
{
    if (!modelo) return;

    setPos(modelo->getX(), modelo->getY());

    if (modelo->getTipo() == Proyectil::TIPO_BALA) {
        float vx = modelo->getVx();
        float vy = modelo->getVy();

        if (std::fabs(vx) > 0.01f || std::fabs(vy) > 0.01f) {
            double angulo = std::atan2(vy, vx)
            * 180.0
                / 3.14159265358979323846;
            setRotation(angulo);
        }
    }
}
