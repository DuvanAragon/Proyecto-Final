#ifndef PERSONAJE_H
#define PERSONAJE_H

class Personaje {
protected:
    float x;
    float y;
    float vx;
    float vy;
    int   salud;
    int   saludMax;
    bool  esAliado;
    int   danoBase;

public:
    // Constructores
    Personaje();
    Personaje(float x,
              float y,
              int   saludMax,
              bool  esAliado,
              int   danoBase);

    virtual ~Personaje() = default;

    // Getters
    float getX() const;
    float getY() const;
    float getVx() const;
    float getVy() const;
    int   getSalud() const;
    int   getSaludMax() const;
    bool  getEsAliado() const;
    int   getDanoBase() const;

    // Setters
    void setPosicion(float _x, float _y);
    void setVelocidad(float _vx, float _vy);
    void setSalud(int _salud);
    void setSaludMax(int _saludMax);
    void setEsAliado(bool _esAliado);
    void setDanoBase(int _danoBase);

    // Metodos
    virtual void mover(float dt);
    virtual void recibirDano(int cantidad);
    virtual void atacar(Personaje* objetivo);
};

#endif // PERSONAJE_H
