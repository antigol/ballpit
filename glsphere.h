#ifndef GLSPHERE_H
#define GLSPHERE_H

#include <QtOpenGL/QGLShaderProgram>
#include <QtOpenGL/QGLBuffer>
#include <QObject>

class GLSphere : public QObject
{
    Q_OBJECT

public:
    explicit GLSphere(QObject *parent = 0);

    void initializeGL(const QGLContext *context = 0);
    void initializeGL(GLint slices , GLint stacks, const QGLContext *context = 0);
    void drawGL();

    void setModel(const QMatrix4x4 &m);
    void setView(const QMatrix4x4 &v);
    void setProjection(const QMatrix4x4 &p);

    void setColor(const QColor &ambiant, const QColor &diffuse, const QColor &specular, GLfloat hardness = 4.0);
    void setLight(const QVector3D &lightPosition);

private:
    QGLShaderProgram *_program;
    QGLBuffer _vbo, _ibo;
    GLuint vbo_sz;
    GLuint ibo_sz[3];

    int _projectionLocation;
    int _viewLocation;
    int _modelLocation;
    int _normalviewLocation;
    int _normalmodelLocation;

    int _ambiantLocation;
    int _diffuseLocation;
    int _specularLocation;
    int _hardnessLocation;
    int _lightLocation;
};

#endif // GLSPHERE_H
