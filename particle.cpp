#include "particle.h"
#include <cmath>

double Particle::radius;
double Particle::cor;

Particle::Particle(double x, double y, double z, double vx, double vy, double vz)
    : p(x, y, z), v(vx, vy, vz)
{
}

Particle::~Particle()
{
}

bool Particle::collisionParticle(Particle* p1, Particle* p2)
{
  QVector3D u = p2->p - p1->p;
  double lengthSquared = u.lengthSquared();
  const double r4 = 4.0 * radius * radius;
  if (lengthSquared <= r4) {
      QVector3D w = (p1->v + p2->v) / 2.0; // velocity or mass center
      QVector3D r1 = p1->v - w; // relative velocity of p1
      double dot = QVector3D::dotProduct(r1, u);
      if (dot > 1e-20) {
          double depth = 10.0 * (2.0 * radius - sqrt(lengthSquared)) / radius;
          double __cor = cor + (1.0 - exp(-depth)) * (1.0 - cor);
          Q_ASSERT(__cor <= 1.0);
          r1 -= (1.0 + __cor) * u * dot / lengthSquared; // new relative velocity

          p1->v = w + r1;
          p2->v = w - r1;
          return true;
      }/* else if (dot > -1e-5 && lengthSquared < 0.75*r4) {
          QVector3D r = 0.0001 * u /lengthSquared;
          p1->v -= r;
          p2->v += r;
          qDebug() << "reaction";
      }*/
  }
  return false;
}
