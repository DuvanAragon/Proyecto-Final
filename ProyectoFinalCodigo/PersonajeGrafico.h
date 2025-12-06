#ifndef PERSONAJEGRAFICO_H
#define PERSONAJEGRAFICO_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QVector>
#include <QPointF>

class Personaje;

class PersonajeGrafico : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    explicit PersonajeGrafico(Personaje* modelo,
                              QGraphicsItem* parent = nullptr);

    void actualizarDesdeModelo();

    void actualizarDesdeModelo(float dt);

    void notificarDisparo(float duracion = 0.15f);

    Personaje* getModelo() const { return modelo; }

private:
    Personaje* modelo;

    QVector<QPixmap> framesMovimiento;
    QPixmap          frameDisparo;
    QPixmap          frameQuieto;

    float   tiempoAnim;
    bool    estaDisparando;
    float   tiempoRestanteDisparo;
    QPointF ultimaPosicion;

    void cargarSprites();
    void cargarSpritesComandante();
    void cargarSpritesEnemigoFusilero();
    void cargarSpritesGranadero();
    void cargarSpritesFrancotirador();
    void cargarSpritesFusileroAliado();
    void cargarSpritesMedico();

    void animar(float dt, const QPointF& posAnterior);
};

#endif // PERSONAJEGRAFICO_H
