#ifndef PROYECTIL_H
#define PROYECTIL_H

class Proyectil {
public:
    static const int TIPO_BALA    = 0;
    static const int TIPO_GRANADA = 1;
    static const int TIPO_CANON   = 2;

private:
    float x;
    float y;
    float vx;
    float vy;
    int   dano;
    bool  activo;
    int   tipo;
    bool  deAliado;

    float tiempoVida;
    float tiempoVidaMax;

    bool  explosivo;
    float radioExplosion;
    float xImpactoPrevisto;
    float yImpactoPrevisto;
    float tiempoDetonacion;
    float tiempoRestanteDetonacion;
    bool  exploto;

public:
    // Constructores
    Proyectil();
    Proyectil(float x,
              float y,
              float vx,
              float vy,
              int   dano,
              int   tipo,
              bool  deAliado);

    // Getters
    float getX() const;
    float getY() const;
    float getVx() const;
    float getVy() const;
    int   getDano() const;
    bool  estaActivo() const;
    int   getTipo() const;
    bool  getDeAliado() const;
    float getTiempoVida() const;
    float getTiempoVidaMax() const;
    bool  esExplosivo() const;
    float getRadioExplosion() const;
    float getXImpactoPrevisto() const;
    float getYImpactoPrevisto() const;
    bool  yaExploto() const;
    float getTiempoRestanteDetonacion() const;

    // Setters
    void setPosicion(float _x, float _y);
    void setVelocidad(float _vx, float _vy);
    void setDano(int _dano);
    void setActivo(bool _activo);
    void setTipo(int _tipo);
    void setDeAliado(bool _deAliado);
    void setTiempoVidaMax(float _tiempoVidaMax);

    void configurarExplosivo(float _radioExplosion,
                             float _tiempoDetonacion,
                             float _xImpactoPrevisto,
                             float _yImpactoPrevisto);

    // Metodos
    void actualizar(float dt);
    void aplicarFisica(float dt);
    bool fueDestruido() const;
};

#endif // PROYECTIL_H
