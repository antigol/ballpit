#ifndef SKYBOX_H
#define SKYBOX_H

#include <QObject>
#include <QtOpenGL/QGLShaderProgram>

class SkyBox : public QObject
{
public:
    explicit SkyBox(QObject *parent = 0);

    void initialiseGL(const QGLContext *context = 0);
    void drawGL(GLuint textureId);

    void setProjection(const QMatrix4x4 &p);
    void setView(const QMatrix4x4 &v);
private:
    QGLShaderProgram *_program;

    int _projectionLocation;
    int _viewLocation;
    GLuint _texture;
};

#endif // SKYBOX_H
