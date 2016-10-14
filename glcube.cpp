#include "glcube.h"
#include "shaders.h"

GLCube::GLCube(QObject *parent) :
    QObject(parent), _vbo(QGLBuffer::VertexBuffer)
{
}

void GLCube::initializeGL(const QGLContext *context)
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

    static const GLfloat data[6 * 4 * 2 * 3] = {
        // -Z
        -1.0, +1.0, -1.0,
            +0.0, +0.0, -1.0,
        +1.0, +1.0, -1.0,
            +0.0, +0.0, -1.0,
        +1.0, -1.0, -1.0,
            +0.0, +0.0, -1.0,
        -1.0, -1.0, -1.0,
            +0.0, +0.0, -1.0,

        // -X
        -1.0, +1.0, +1.0,
            -1.0, +0.0, +0.0,
        -1.0, +1.0, -1.0,
            -1.0, +0.0, +0.0,
        -1.0, -1.0, -1.0,
            -1.0, +0.0, +0.0,
        -1.0, -1.0, +1.0,
            -1.0, +0.0, +0.0,

        // +Z
        +1.0, +1.0, +1.0,
            +0.0, +0.0, +1.0,
        -1.0, +1.0, +1.0,
            +0.0, +0.0, +1.0,
        -1.0, -1.0, +1.0,
            +0.0, +0.0, +1.0,
        +1.0, -1.0, +1.0,
            +0.0, +0.0, +1.0,

        // +X
        +1.0, +1.0, -1.0,
            +1.0, +0.0, +0.0,
        +1.0, +1.0, +1.0,
            +1.0, +0.0, +0.0,
        +1.0, -1.0, +1.0,
            +1.0, +0.0, +0.0,
        +1.0, -1.0, -1.0,
            +1.0, +0.0, +0.0,

        // -Y
        -1.0, -1.0, -1.0,
            +0.0, -1.0, +0.0,
        +1.0, -1.0, -1.0,
            +0.0, -1.0, +0.0,
        +1.0, -1.0, +1.0,
            +0.0, -1.0, +0.0,
        -1.0, -1.0, +1.0,
            +0.0, -1.0, +0.0,

        // +Y
        +1.0, +1.0, -1.0,
            +0.0, +1.0, +0.0,
        -1.0, +1.0, -1.0,
            +0.0, +1.0, +0.0,
        -1.0, +1.0, +1.0,
            +0.0, +1.0, +0.0,
        +1.0, +1.0, +1.0,
            +0.0, +1.0, +0.0
    };

    _vbo.create();
    _vbo.bind();
    _vbo.allocate(data, 144 * sizeof (GLfloat));
    _vbo.release();
}

void GLCube::drawGL()
{
    _program->bind();

    _vbo.bind();

    _program->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
    _program->enableAttributeArray(PROGRAM_NORMAL_ATTRIBUTE);

    _program->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0,                    3, 6 * sizeof (GLfloat));
    _program->setAttributeBuffer(PROGRAM_NORMAL_ATTRIBUTE, GL_FLOAT, 3 * sizeof (GLfloat), 3, 6 * sizeof (GLfloat));

    glDrawArrays(GL_QUADS, 0, 6 * 4);

    _program->disableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
    _program->disableAttributeArray(PROGRAM_NORMAL_ATTRIBUTE);

    _vbo.release();

    _program->release();
}

void GLCube::setModel(const QMatrix4x4 &m)
{
    _program->bind();
    _program->setUniformValue(_modelLocation, m);
    _program->setUniformValue(_normalmodelLocation, m.normalMatrix());
    _program->release();
}

void GLCube::setView(const QMatrix4x4 &v)
{
    _program->bind();
    _program->setUniformValue(_viewLocation, v);
    _program->setUniformValue(_normalviewLocation, v.normalMatrix());
    _program->release();
}

void GLCube::setProjection(const QMatrix4x4 &p)
{
    _program->bind();
    _program->setUniformValue(_projectionLocation, p);
    _program->release();
}

void GLCube::setColor(const QColor &ambiant, const QColor &diffuse, const QColor &specular, GLfloat hardness)
{
    _program->bind();
    _program->setUniformValue(_ambiantLocation, ambiant);
    _program->setUniformValue(_diffuseLocation, diffuse);
    _program->setUniformValue(_specularLocation, specular);
    _program->setUniformValue(_hardnessLocation, hardness);
    _program->release();
}

void GLCube::setLight(const QVector3D &lightPosition)
{
    _program->bind();
    _program->setUniformValue(_lightLocation, lightPosition.normalized());
    _program->release();
}
