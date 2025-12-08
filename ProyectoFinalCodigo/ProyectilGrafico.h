#ifndef PROYECTILGRAFICO_H
#define PROYECTILGRAFICO_H

#include <QObject>
#include <QGraphicsPixmapItem>

class Proyectil;

class ProyectilGrafico : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    explicit ProyectilGrafico(Proyectil* modelo,
                              QGraphicsItem* parent = nullptr);

    void actualizarDesdeModelo();

private:
    Proyectil* modelo;
};

#endif // PROYECTILGRAFICO_H
