#ifndef PARTICLE_H
#define PARTICLE_H

#include <QVector3D>
#include <QColor>

class Particle {
public:
    Particle(double x = 0.0, double y = 0.0, double z = 0.0,
             double vx = 0.0, double vy = 0.0, double vz = 0.0);
    ~Particle();
    QVector3D p; // position
    QVector3D v; // velocity
    QColor color;
    bool impact;

    static double radius;
    static double cor;

    static bool collisionParticle(Particle *p1, Particle *p2);
};

#endif // PARTICLE_H
