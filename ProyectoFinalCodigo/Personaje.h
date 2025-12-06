#ifndef PERSONAJE_H
#define PERSONAJE_H

class Personaje
{
public:
    // Constructores
    Personaje();
    Personaje(float xParam,
              float yParam,
              int   saludMaxParam,
              bool  esAliadoParam,
              int   danoBaseParam);

    // Destructor
    virtual ~Personaje() {}

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

    // Métodos básicos
    virtual void mover(float dt);
    virtual void recibirDano(int cantidad);
    virtual void atacar(Personaje* objetivo);

    // Método de actualización genérico (IA, etc.)
    virtual void actualizar(float dt);



protected:
    float x;
    float y;
    float vx;
    float vy;
    int   salud;
    int   saludMax;
    bool  esAliado;
    int   danoBase;
};

#endif // PERSONAJE_H
