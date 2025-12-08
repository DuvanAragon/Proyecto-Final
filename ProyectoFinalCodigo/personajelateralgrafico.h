#ifndef PERSONAJELATERALGRAFICO_H
#define PERSONAJELATERALGRAFICO_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QVector>
#include <QPixmap>

class QTimer;
class Personaje;

class PersonajeLateralGrafico : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    explicit PersonajeLateralGrafico(Personaje *modelo,
                                     const QString &prefijoFrames,
                                     int numFrames,
                                     const QString &spriteCrouchPath,
                                     QGraphicsItem *parent = nullptr);
    ~PersonajeLateralGrafico();

    void actualizarDesdeModelo();

    void setEnMovimiento(bool mov);
    void setAgachado(bool crouch);
    void setMirandoDerecha(bool der);


private slots:
    void avanzarFrame();

private:
    void cargarFramesMovimiento(const QString &prefijoFrames, int numFrames);
    void cargarSpriteAgachado(const QString &spriteCrouchPath);

    Personaje *modelo;

    QVector<QPixmap> framesMovimiento;
    QPixmap frameQuieto;
    QPixmap frameAgachado;

    int   frameActual;
    QTimer *timerAnim;

    bool enMovimiento;
    bool agachado;
    bool tieneAgachado;

    bool mirandoDerecha;
};

#endif // PERSONAJELATERALGRAFICO_H
