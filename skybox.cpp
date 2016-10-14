#include "skybox.h"
#define GLSL(version, shader)  "#version " #version "\n" #shader
#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1

static const char *vertsrc = GLSL(130,
    in vec3 vertex;
    in vec2 texCoord;

    uniform mat4 matrixp; // projection
    uniform mat4 matrixv; // view (camera)

    out vec2 t;

    void main(void)
    {
        gl_Position = matrixp * vec4(mat3(matrixv) * vertex, 1.0);
        t = texCoord;
    }
);

static const char *fragsrc = GLSL(130,
    in vec2 t;

    uniform sampler2D texture;

    out vec4 color;

    void main(void)
    {
        color = texture2D(texture, t);
    }
);

SkyBox::SkyBox(QObject *parent) :
    QObject(parent)
{
}

void SkyBox::initialiseGL(const QGLContext *context)
{
    _program = new QGLShaderProgram(context, this);
    _program->addShaderFromSourceCode(QGLShader::Vertex, vertsrc);
    _program->addShaderFromSourceCode(QGLShader::Fragment, fragsrc);
    _program->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
    _program->bindAttributeLocation("texCoord", PROGRAM_TEXCOORD_ATTRIBUTE);
    if (!_program->link())
        qDebug() << _program->log();

    _projectionLocation = _program->uniformLocation("matrixp");
    _viewLocation = _program->uniformLocation("matrixv");

    _program->bind();
    _program->setUniformValue("texture", 0);
    _program->setUniformValue(_projectionLocation, QMatrix4x4());
    _program->setUniformValue(_viewLocation, QMatrix4x4());
    _program->release();
}

void SkyBox::drawGL(GLuint textureId)
{
    static const GLfloat vertices[72] = {
        // -Z
        -1.0, +1.0, -1.0,
        -1.0, -1.0, -1.0,
        +1.0, -1.0, -1.0,
        +1.0, +1.0, -1.0,

        // -X
        -1.0, +1.0, +1.0,
        -1.0, -1.0, +1.0,
        -1.0, -1.0, -1.0,
        -1.0, +1.0, -1.0,

        // +Z
        +1.0, +1.0, +1.0,
        +1.0, -1.0, +1.0,
        -1.0, -1.0, +1.0,
        -1.0, +1.0, +1.0,

        // +X
        +1.0, +1.0, -1.0,
        +1.0, -1.0, -1.0,
        +1.0, -1.0, +1.0,
        +1.0, +1.0, +1.0,

        // -Y
        -1.0, -1.0, -1.0,
        -1.0, -1.0, +1.0,
        +1.0, -1.0, +1.0,
        +1.0, -1.0, -1.0,

        // +Y
        +1.0, +1.0, -1.0,
        +1.0, +1.0, +1.0,
        -1.0, +1.0, +1.0,
        -1.0, +1.0, -1.0
    };
    static const GLfloat texCoords[48] = {
        // -Z
        0.25, 2.0/3.0,
        0.25, 1.0/3.0,
        0.50, 1.0/3.0,
        0.50, 2.0/3.0,

        // -X
        0.0, 2.0/3.0,
        0.0, 1.0/3.0,
        0.25, 1.0/3.0,
        0.25, 2.0/3.0,

        // +Z
        0.75, 2.0/3.0,
        0.75, 1.0/3.0,
        1.0, 1.0/3.0,
        1.0, 2.0/3.0,

        // +X
        0.5, 2.0/3.0,
        0.5, 1.0/3.0,
        0.75, 1.0/3.0,
        0.75, 2.0/3.0,

        // -Y
        0.25, 1.0/3.0,
        0.25, 0.0,
        0.5, 0.0,
        0.5, 1.0/3.0,

        // +Y
        0.5, 2.0/3.0,
        0.5, 1.0,
        0.25, 1.0,
        0.25, 2.0/3.0
    };

    _program->bind();
    _program->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
    _program->enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);
    _program->setAttributeArray(PROGRAM_VERTEX_ATTRIBUTE, vertices, 3);
    _program->setAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE, texCoords, 2);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glDepthMask(GL_FALSE);
    glDrawArrays(GL_QUADS, 0, 24);
    glDepthMask(GL_TRUE);

    _program->release();
}

void SkyBox::setProjection(const QMatrix4x4 &p)
{
    _program->bind();
    _program->setUniformValue(_projectionLocation, p);
    _program->release();
}

void SkyBox::setView(const QMatrix4x4 &v)
{
    _program->bind();
    _program->setUniformValue(_viewLocation, v);
    _program->release();
}
