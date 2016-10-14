#include "glsphere.h"
#include "shaders.h"
#include <cmath>

GLSphere::GLSphere(QObject *parent)
    : QObject(parent), _vbo(QGLBuffer::VertexBuffer), _ibo(QGLBuffer::IndexBuffer)
{
}

void GLSphere::initializeGL(const QGLContext *context)
{
    initializeGL(10, 10, context);
}

void GLSphere::initializeGL(GLint slices, GLint stacks, const QGLContext *context)
{
    _program = new QGLShaderProgram(context, this);
    _program->addShaderFromSourceCode(QGLShader::Vertex, vertsrc);
    _program->addShaderFromSourceCode(QGLShader::Fragment, fragsrc);
    _program->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
    _program->bindAttributeLocation("normal", PROGRAM_NORMAL_ATTRIBUTE);
    if (!_program->link())
        qDebug() << _program->log();

    _modelLocation = _program->uniformLocation("matrixm");
    _viewLocation = _program->uniformLocation("matrixv");
    _normalmodelLocation = _program->uniformLocation("matrixmn");
    _normalviewLocation = _program->uniformLocation("matrixvn");
    _projectionLocation = _program->uniformLocation("matrixp");
    _ambiantLocation = _program->uniformLocation("ambiant");
    _diffuseLocation = _program->uniformLocation("diffuse");
    _specularLocation = _program->uniformLocation("specular");
    _hardnessLocation = _program->uniformLocation("hardness");
    _lightLocation = _program->uniformLocation("light");

    setProjection(QMatrix4x4());
    setView(QMatrix4x4());
    setModel(QMatrix4x4());

    QVector<GLfloat> positions;

    QVector<GLuint> indices0;
    QVector<GLuint> indices1;
    QVector<GLuint> indices2;



    unsigned int size = 2 + slices * (stacks - 1);

    positions.reserve(3 * size);

    double alpha = M_PI / double(stacks);
    double beta = 2.0*M_PI / double(slices);

    positions << 0.0 << 0.0 << 1.0;

    for (int i = 1; i < stacks; ++i) {
        for (int j = 0; j < slices; ++j) {
            float r = sin(i*alpha);
            float z = cos(i*alpha);
            float y = sin(j*beta) * r;
            float x = cos(j*beta) * r;

            positions << x << y << z;
        }
    }

    positions << 0.0 << 0.0 << -1.0;





    indices0.reserve(slices+2);
    for (int i = 0; i <= slices; ++i)
        indices0 << i;
    indices0 << 1;

    indices1.reserve((stacks-2) * 2 * (slices+1));
    for (int i = 0; i < stacks-2; ++i) {
        for (int j = 0; j < slices; ++j) {
            indices1 << 1+i*slices+j;
            indices1 << 1+(i+1)*slices+j;
        }
        indices1 << 1+i*slices;
        indices1 << 1+(i+1)*slices;
    }

    indices2.reserve(slices+2);
    for (int i = 1; i <= slices+1; ++i)
        indices2 << size-i;
    indices2 << size-2;

    vbo_sz = 3 * size * sizeof (GLfloat);
    _vbo.create();
    _vbo.bind();
    _vbo.allocate(positions.constData(), vbo_sz);
    _vbo.release();

    ibo_sz[0] = indices0.size() * sizeof (GLuint);
    ibo_sz[1] = indices1.size() * sizeof (GLuint);
    ibo_sz[2] = indices2.size() * sizeof (GLuint);
    _ibo.create();
    _ibo.bind();
    _ibo.allocate(ibo_sz[0] + ibo_sz[1] + ibo_sz[2]);
    _ibo.write(0, indices0.constData(), ibo_sz[0]);
    _ibo.write(ibo_sz[0], indices1.constData(), ibo_sz[1]);
    _ibo.write(ibo_sz[0] + ibo_sz[1], indices2.constData(), ibo_sz[2]);
    _ibo.release();
}

#define BUFFER_OFFSET(a) ((char*)NULL + (a))

void GLSphere::drawGL()
{
    _program->bind();

    _vbo.bind();
    _ibo.bind();

    _program->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
    _program->enableAttributeArray(PROGRAM_NORMAL_ATTRIBUTE);

    _program->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 3);
    _program->setAttributeBuffer(PROGRAM_NORMAL_ATTRIBUTE, GL_FLOAT, 0, 3);

    glDrawElements(GL_TRIANGLE_FAN, ibo_sz[0] / sizeof (GLuint), GL_UNSIGNED_INT, BUFFER_OFFSET(0));
    glDrawElements(GL_QUAD_STRIP, ibo_sz[1] / sizeof (GLuint), GL_UNSIGNED_INT, BUFFER_OFFSET(ibo_sz[0]));
    glDrawElements(GL_TRIANGLE_FAN, ibo_sz[2] / sizeof (GLuint), GL_UNSIGNED_INT, BUFFER_OFFSET(ibo_sz[0] + ibo_sz[1]));

    _program->disableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
    _program->disableAttributeArray(PROGRAM_NORMAL_ATTRIBUTE);

    _ibo.release();
    _vbo.release();

    _program->release();
}

void GLSphere::setModel(const QMatrix4x4 &m)
{
    _program->bind();
    _program->setUniformValue(_modelLocation, m);
    _program->setUniformValue(_normalmodelLocation, m.normalMatrix());
    _program->release();
}

void GLSphere::setView(const QMatrix4x4 &v)
{
    _program->bind();
    _program->setUniformValue(_viewLocation, v);
    _program->setUniformValue(_normalviewLocation, v.normalMatrix());
    _program->release();
}

void GLSphere::setProjection(const QMatrix4x4 &p)
{
    _program->bind();
    _program->setUniformValue(_projectionLocation, p);
    _program->release();
}

void GLSphere::setColor(const QColor &ambiant, const QColor &diffuse, const QColor &specular, GLfloat hardness)
{
    _program->bind();
    _program->setUniformValue(_ambiantLocation, ambiant);
    _program->setUniformValue(_diffuseLocation, diffuse);
    _program->setUniformValue(_specularLocation, specular);
    _program->setUniformValue(_hardnessLocation, hardness);
    _program->release();
}

void GLSphere::setLight(const QVector3D &lightPosition)
{
    _program->bind();
    _program->setUniformValue(_lightLocation, lightPosition.normalized());
    _program->release();
}
