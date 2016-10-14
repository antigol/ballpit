#ifndef WIDGET_H
#define WIDGET_H

#include <QGLWidget>
#include <QVector3D>
#include <QGLShaderProgram>
#include <QTime>
#include "glsphere.h"
#include "glcube.h"
#include "skybox.h"
#include "particle.h"

class Widget : public QGLWidget
{
    Q_OBJECT
    
public:
    Widget(QWidget *parent = 0);
    ~Widget();

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

private:
    void paintEvent(QPaintEvent *);
    void timerEvent(QTimerEvent *);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void wheelEvent(QWheelEvent *e);
    void keyPressEvent(QKeyEvent *e);

    bool collisionWall(Particle *p1, double d, QVector3D u);
    void simulation(double dt);

    double _gValue;
    QVector<Particle *> _particles;
    QTime _dt;

    int _cfps, _tfps;
    int _fps;

    QPointF _oldmouseposition;
    double _psi, _tetha, _z, _x, _y;

    GLSphere *_sphere;
    GLCube *_cube;
    SkyBox *_skybox;
    GLuint _texture0;
};

#endif // WIDGET_H
