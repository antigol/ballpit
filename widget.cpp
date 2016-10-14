#include "widget.h"
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QTime>
#include <QDebug>
#include <QInputDialog>
#include <QMap>
#include <cmath>
using namespace std;

double randd()
{
    return double(rand()) / (double(RAND_MAX)+1.0);
}

Widget::Widget(QWidget *parent)
    : QGLWidget(parent)
{
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAutoFillBackground(false);

    _sphere = new GLSphere(this);
    _cube = new GLCube(this);
    _skybox = new SkyBox(this);
    Particle::radius = 0.03125;
    Particle::cor = 0.8;
    _gValue = -1.0;
    _fps = 0;

    _psi = _tetha = 0.0;
    _x = _y = 0.0;
    _z = 5.0;

    _dt.start();
    startTimer(0);
}

Widget::~Widget()
{
    qDeleteAll(_particles);
    deleteTexture(_texture0);
}

void Widget::initializeGL()
{
    QVector3D light(1.0, 0.5, -1.0);

    _sphere->initializeGL(20, 20);
    _sphere->setLight(light);
    _cube->initializeGL();
    _cube->setLight(light);
    QMatrix4x4 m;
    m.scale(1.0 + Particle::radius);
    _cube->setModel(m);
    _cube->setColor(QColor::fromHsvF(0.5, 0.2, 0.6, 0.2), QColor::fromHsvF(0.5, 0.4, 0.8, 0.2), QColor::fromHsvF(0.5, 0.0, 1.0, 0.6));


    _skybox->initialiseGL();
    _texture0 = bindTexture(QImage(":/images/skybox.jpg"));
}

void Widget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    QMatrix4x4 p;
    p.perspective(50.0, qreal(w)/qreal(h?h:1), 0.1, 1000.0);
    _sphere->setProjection(p);
    _cube->setProjection(p);
    _skybox->setProjection(p);
}

void Widget::paintGL()
{
    glEnable(GL_TEXTURE_2D); // texture
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QMatrix4x4 m;
    m.translate(_x, -_y, -_z);
    m.rotate(_tetha, 1, 0, 0);
    m.rotate(_psi, 0, 1, 0);

    _sphere->setView(m);
    _cube->setView(m);
    _skybox->setView(m);

    _skybox->drawGL(_texture0);

    glDisable(GL_CULL_FACE);
    glDepthMask(GL_FALSE);
    //    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    _cube->drawGL();
    //    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);

    for (int i = 0; i < _particles.size(); ++i) {
        const Particle *p = _particles[i];
        m.setToIdentity();
        m.translate(p->p);
        m.scale(Particle::radius);
        _sphere->setColor(p->color.darker(), p->color, p->color.lighter(230));
        _sphere->setModel(m);
        _sphere->drawGL();
    }
}

void Widget::paintEvent(QPaintEvent *)
{
    paintGL();

    QPainter painter(this);

    painter.setPen(Qt::yellow);
    painter.setFont(QFont("Helvetica", 8));
    painter.drawText(QRectF(10, 10, 210, 100), QString(
                         "R, C, S, P to initialize shapes\n"
                         "G modify g\n"
                         "C mod. coeff. of restitution\n"
                         "g = %1\n"
                         "cor = %2\n"
                         "%3 particles\n"
                         "fps : %4").arg(_gValue).arg(Particle::cor).arg(_particles.size()).arg(_cfps));

    painter.end();
}

void Widget::timerEvent(QTimerEvent *)
{
    _fps++;
    if (_tfps != QTime::currentTime().second()) {
        _tfps = QTime::currentTime().second();
        _cfps = _fps;
        _fps = 0;
    }

    simulation(_dt.restart() * 2e-4);
    update();
}

void Widget::mousePressEvent(QMouseEvent *e)
{
    _oldmouseposition = e->pos();
}

void Widget::mouseMoveEvent(QMouseEvent *e)
{
    QPointF d = e->pos() - _oldmouseposition;
    _oldmouseposition = e->pos();

    if (e->buttons() & Qt::LeftButton) {
        _psi += d.x() * 0.5;
        _tetha += d.y() * 0.5;
        _tetha = _tetha < -90.0 ? -90.0 : _tetha > 90.0 ? 90.0 : _tetha;
    } else if (e->buttons() & Qt::RightButton) {
        _x += d.x() * 0.005;
        _y += d.y() * 0.005;
    }
}

void Widget::wheelEvent(QWheelEvent *e)
{
    _z *= pow(0.999, e->delta());
}

void Widget::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_T) {
        qDeleteAll(_particles);
        _particles.clear();
        for (int x = -4; x <= 4; ++x) {
            for (int y = -6; y <= 6; ++y) {
                for (int z = -4; z <= 4; ++z) {
                    Particle *p = new Particle(x*2.01*Particle::radius+randd()*1e-7,
                                               y*2.01*Particle::radius+randd()*1e-7,
                                               z*2.01*Particle::radius+randd()*1e-7);
                    p->color = QColor::fromHslF(0.1*(abs(x+y+z)%11), 0.9, 0.5);
                    _particles.append(p);
                }
            }
        }
    }
    if (e->key() == Qt::Key_S) {
        qDeleteAll(_particles);
        _particles.clear();
        for (int x = -10; x < 11; ++x) {
            for (int y = -10; y < 11; ++y) {
                for (int z = -10; z < 11; ++z) {
                    if (x*x + y*y + z*z < 70.0) {
                        QVector3D v(x*2.1*Particle::radius+randd()*1e-5,
                                    y*2.1*Particle::radius+randd()*1e-5,
                                    z*2.1*Particle::radius+randd()*1e-5);
                        QMatrix4x4 m;
                        m.rotate(45, 1, 1, 1);
                        v = m * v;
                        Particle *p = new Particle(v.x(), v.y(), v.z(), 0, 0, 0);
                        p->color = QColor::fromHslF(0.1*(abs(x+y+z)%11), 0.9, 0.5);
                        _particles.append(p);
                    }
                }
            }
        }
    }

    if (e->key() == Qt::Key_P) {
        qDeleteAll(_particles);
        _particles.clear();
        for (int x = -9; x < 10; ++x) {
            for (int y = -9; y < 10; ++y) {
                for (int z = -9; z < 10; ++z) {
                    if (abs(z) + abs(x) + abs(y) < 9) {
                        Particle *p = new Particle(x*2.02*Particle::radius+randd()*1e-6,
                                                   y*2.02*Particle::radius+randd()*1e-6,
                                                   z*2.02*Particle::radius+randd()*1e-6,
                                                   0,0,0);
                        p->color = QColor::fromHslF(0.1*(abs(x+y+z)%11), 0.9, 0.5);
                        QMatrix4x4 m;
                        //                        m.rotate(45, 1, 1, 1);
                        p->p = m * p->p;
                        _particles.append(p);
                    }
                }
            }
        }
    }
    if (e->key() == Qt::Key_R) {
        qDeleteAll(_particles);
        _particles.clear();
        for (int i = 0; i < 1000; i++) {
            Particle *p = new Particle(2.0 * randd() - 1.0,
                                       2.0 * randd() - 1.0,
                                       2.0 * randd() - 1.0);
            p->color = QColor::fromHslF(randd(), 0.9, 0.5);
            _particles.append(p);
        }
    }
    if (e->key() == Qt::Key_G) {
        _gValue = QInputDialog::getDouble(this, "G", "G value [-10,10] default value is -1", _gValue, -10.0, 10.0, 2);
    }
    if (e->key() == Qt::Key_C) {
        Particle::cor = QInputDialog::getDouble(this, "COR", "Coefficient of restitusion [0,1]", Particle::cor, 0.0, 1.0, 2);
    }
}

bool Widget::collisionWall(Particle *p1, double d, QVector3D u)
{
    double pos = QVector3D::dotProduct(p1->p, u);
    if (pos <= d) {
        double dot = QVector3D::dotProduct(u, p1->v);
        if (dot < 0.0) {
            double depth = (d - pos) / Particle::radius;
            double cor = Particle::cor + (1.0 - exp(-depth)) * (1.0 - Particle::cor);
            p1->v -= (1.0 + cor) * u * dot;
            return true;
        }
    }
    return false;
}

class CList {
public:
    QVector<Particle *> v;
    bool b;
    CList() {
        b = false;
        v.reserve(10);
    }
};

void Widget::simulation(double dt)
{
    QTime t;
    t.start();

    const int div = 32;
    QMap<int, CList> m;
    for (int i = 0; i < _particles.size(); ++i) {
        QVector3D p = _particles[i]->p + QVector3D(1, 1, 1); // -1 <-> -0.75 <-> -0.5 [...] 0.5 <-> 0.75 <-> 1.0
        p *= div / 2;
        int x = max(min(int(floor(p.x())), div-1), 0);
        int y = max(min(int(floor(p.y())), div-1), 0);
        int z = max(min(int(floor(p.z())), div-1), 0);

        m[x + div*y + div*div*z].v.append(_particles[i]);
    }

    qDebug() << t.elapsed();

    for (int i = 0; i < _particles.size(); ++i) {
        _particles[i]->impact = false;
    }

    QMap<int, CList>::iterator k = m.begin();
    while (k != m.end()) {
        const QVector<Particle *> &u = k.value().v;

        int kk = k.key();
        int z = (kk / (div*div)) % div;
        int y = (kk / div) % div;
        int x = kk % div;

        for (int dx = -1; dx <= 1; ++dx) {
            int xx = x + dx;
            if (xx < 0 || xx > div-1) continue;
            for (int dy = -1; dy <= 1; ++dy) {
                int yy = y + dy;
                if (yy < 0 || yy > div-1) continue;
                for (int dz = -1; dz <= 1; ++dz) {
                    int zz = z + dz;
                    if (zz < 0 || zz > div-1) continue;
                    int l = xx + div*yy + div*div*zz;

                    QMap<int, CList>::const_iterator ll;
                    if ((ll = m.find(l)) != m.end() && ll.value().b == false) {
                        const QVector<Particle *> &v = ll.value().v;
                        for (int i = 0; i < u.size(); ++i) {
                            u[i]->impact |= collisionWall(u[i], -1.0, QVector3D(1,0,0));
                            u[i]->impact |= collisionWall(u[i], -1.0, QVector3D(-1,0,0));
                            u[i]->impact |= collisionWall(u[i], -1.0, QVector3D(0,1,0));
                            u[i]->impact |= collisionWall(u[i], -1.0, QVector3D(0,-1,0));
                            u[i]->impact |= collisionWall(u[i], -1.0, QVector3D(0,0,1));
                            u[i]->impact |= collisionWall(u[i], -1.0, QVector3D(0,0,-1));
                            for (int j = 0; j < v.size(); ++j) {
                                if (u[i] != v[j])
                                    if (Particle::collisionParticle(u[i], v[j]))
                                        u[i]->impact = v[j]->impact = true;
                            }
                        }
                    }
                }
            }
        }


        k.value().b = true;
        ++k;
    }

    for (int i = 0; i < _particles.size(); ++i) {
        if (!_particles[i]->impact) {
            _particles[i]->p += _particles[i]->v * dt;
            _particles[i]->v += QVector3D(0, _gValue, 0) * dt;
        } else {
            _particles[i]->p += _particles[i]->v * dt * 0.05;
        }
    }

    qDebug() << t.elapsed();
}
