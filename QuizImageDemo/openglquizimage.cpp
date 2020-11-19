#include "openglquizimage.h"
#include <QQuickItem>

#if QT_CONFIG(opengl)

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include "quizimage.h"


OpenGLQuizImageV1::OpenGLQuizImageV1(QObject * parent_)
    : m_parent(parent_)
{
}

OpenGLQuizImageV1::~OpenGLQuizImageV1()
{
    releaseResources();
}

void OpenGLQuizImageV1::releaseResources()
{
    delete m_vshader;
    m_vshader = nullptr;
    delete m_fshader;
    m_fshader = nullptr;
    delete m_fromTexture;
    m_fromTexture = nullptr;
    delete m_toTexture;
    m_toTexture = nullptr;
    delete m_program;
    m_program = nullptr;
}

void OpenGLQuizImageV1::makeObject()
{
    static const int coords[4][3] = {
        { +1, -1, -1 }, { -1, -1, -1 }, { -1, +1, -1 }, { +1, +1, -1 }
    };

    m_vertData.clear();
    for (int j = 0; j < 4; ++j) {
        // vertex position
        m_vertData.append(0.2 * coords[j][0]);
        m_vertData.append(0.2 * coords[j][1]);
        m_vertData.append(0.2 * coords[j][2]);
        // texture coordinate
        m_vertData.append(j == 0 || j == 3);
        m_vertData.append(j == 0 || j == 1);
    }
}

void OpenGLQuizImageV1::init(QOpenGLFunctions *f_)
{
    f_->glEnable(GL_DEPTH_TEST);
    f_->glEnable(GL_CULL_FACE);

    m_fromTexture = new QOpenGLTexture(QImage(QString(":/images/side%1.png").arg(1)).mirrored());
    m_toTexture = new QOpenGLTexture(QImage(QString(":/images/side%1.png").arg(2)).mirrored());

    if(nullptr == m_vshader)
    {
        m_vshader = new QOpenGLShader(QOpenGLShader::Vertex, m_parent);
        const char *vsrc =
            "attribute highp vec4 vertex;\n"
            "attribute mediump vec4 texCoord;\n"
            "varying mediump vec4 texc;\n"
            "uniform mediump mat4 matrix;\n"
            "void main(void)\n"
            "{\n"
            "    gl_Position = matrix * vertex;\n"
            "    texc = texCoord;\n"
            "}\n";
        m_vshader->compileSourceCode(vsrc);
    }

    if(nullptr == m_fshader)
    {
        m_fshader = new QOpenGLShader(QOpenGLShader::Fragment, m_parent);
        const char *fsrc =
            "uniform sampler2D texture;\n"
            "varying mediump vec4 texc;\n"
            "void main(void)\n"
            "{\n"
            "    gl_FragColor = texture2D(texture, texc.st);\n"
            "}\n";
        m_fshader->compileSourceCode(fsrc);
    }

    m_program = new QOpenGLShaderProgram();
    m_program->addShader(m_vshader);
    m_program->addShader(m_fshader);
    m_program->link();

    if(0 > m_vertexAttrId) { m_vertexAttrId = m_program->attributeLocation("vertex"); }
    if(0 > m_texCoordAttrId) { m_texCoordAttrId = m_program->attributeLocation("texCoord"); }

    if(0 > m_textureId) { m_textureId = m_program->uniformLocation("texture"); }
    if(0 > m_matrixId) { m_matrixId = m_program->uniformLocation("matrix"); }

    m_program->bindAttributeLocation("vertex", m_vertexAttrId);
    m_program->bindAttributeLocation("texCoord", m_texCoordAttrId);

    makeObject();

    m_vbo = new QOpenGLBuffer();
    m_vbo->create();
    m_vbo->bind();
    m_vbo->allocate(m_vertData.count() * sizeof(GLfloat));
    m_vbo->write(m_vertData.count(), m_vertData.constData(), sizeof(GLfloat));
    m_vbo->release();
}

void OpenGLQuizImageV1::paintGL(QOpenGLFunctions *f_, const RenderState *state_)
{
    m_program->bind();
    m_program->setUniformValue(m_textureId, 0);

    QMatrix4x4 m;
    m.ortho(-0.5f, +0.5f, +0.5f, -0.5f, 4.0f, 15.0f);
    m.translate(0.0f, 0.0f, -10.0f);
    m.rotate(m_xRot / 16.0f, 1.0f, 0.0f, 0.0f);
    m.rotate(m_yRot / 16.0f, 0.0f, 1.0f, 0.0f);
    m.rotate(m_zRot / 16.0f, 0.0f, 0.0f, 1.0f);

    m_program->setUniformValue(m_matrixId, m);

    m_vbo->bind();
    m_vbo->write(0, m_vertData.constData(), sizeof(GLfloat)*m_vertData.count());

    m_program->enableAttributeArray(m_vertexAttrId);
    m_program->enableAttributeArray(m_texCoordAttrId);
    m_program->setAttributeBuffer(m_vertexAttrId, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
    m_program->setAttributeBuffer(m_texCoordAttrId, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));

    f_->glClearColor(m_clearColor.redF(), m_clearColor.greenF(), m_clearColor.blueF(), m_clearColor.alphaF());
    f_->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    // We are prepared both for the legacy (direct OpenGL) and the modern
    // (abstracted by RHI) OpenGL scenegraph. So set all the states that are
    // important to us.

    //! [3]
    f_->glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    f_->glEnable(GL_BLEND);
    f_->glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    // Clip support.
    if (state_->scissorEnabled()) {
        f_->glEnable(GL_SCISSOR_TEST);
        const QRect r = state_->scissorRect(); // already bottom-up
        f_->glScissor(r.x(), r.y(), r.width(), r.height());
    }
    if (state_->stencilEnabled()) {
        f_->glEnable(GL_STENCIL_TEST);
        f_->glStencilFunc(GL_EQUAL, state_->stencilValue(), 0xFF);
        f_->glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    }

    m_fromTexture->bind();
    f_->glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}


void OpenGLQuizImageV1::render(const RenderState *state_)
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->initializeOpenGLFunctions();
    if (nullptr == m_program) { init(f); }
    paintGL(f, state_);
}

QSGRenderNode::StateFlags OpenGLQuizImageV1::changedStates() const
{
    return BlendState | ScissorState | StencilState;
}

QSGRenderNode::RenderingFlags OpenGLQuizImageV1::flags() const
{
    return BoundedRectRendering | DepthAwareRendering;
}

QRectF OpenGLQuizImageV1::rect() const
{
    return QRect(0, 0, m_width, m_height);
}

void OpenGLQuizImageV1::sync(QQuickItem *item_)
{
    //qDebug() << "sync()";
    m_width = item_->width();
    m_height = item_->height();
    m_xRot += 10.0;
}


OpenGLQuizImage::OpenGLQuizImage(QObject *parent_)
    : m_parent(parent_)
{
}

OpenGLQuizImage::~OpenGLQuizImage()
{
    releaseResources();
}

void OpenGLQuizImage::releaseResources()
{
}

void OpenGLQuizImage::render(const RenderState *state_)
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->initializeOpenGLFunctions();
    if (nullptr == m_program) { init(f); }
    paintGL(f, state_);
}

QSGRenderNode::StateFlags OpenGLQuizImage::changedStates() const
{
    return BlendState | ScissorState | StencilState;
}

QSGRenderNode::RenderingFlags OpenGLQuizImage::flags() const
{
    return BoundedRectRendering | DepthAwareRendering;
}

QRectF OpenGLQuizImage::rect() const
{
    return QRect(0, 0, m_width, m_height);
}

void OpenGLQuizImage::sync(QQuickItem *item_)
{
    m_width = item_->width();
    m_height = item_->height();
    QuizImage *quizImage = static_cast<QuizImage *>(item_);
    Q_UNUSED(quizImage);
}

void OpenGLQuizImage::makeObject()
{
    static const int coords[4][3] = {
        { +1, -1, -1 }, { -1, -1, -1 }, { -1, +1, -1 }, { +1, +1, -1 }
    };

    m_vertData.clear();
    for (int j = 0; j < 4; ++j) {
        // vertex position
        m_vertData.append(0.5 * coords[j][0]);
        m_vertData.append(0.5 * coords[j][1]);
        m_vertData.append(0.5 * coords[j][2]);
        // texture coordinate
        m_vertData.append(j == 0 || j == 3);
        m_vertData.append(j == 0 || j == 1);
    }
}

void OpenGLQuizImage::init(QOpenGLFunctions *f_)
{
    f_->glEnable(GL_DEPTH_TEST);
    f_->glEnable(GL_CULL_FACE);

    m_fromTexture = new QOpenGLTexture(QImage(QString(":/images/side%1.png").arg(1)).mirrored());
    m_toTexture = new QOpenGLTexture(QImage(QString(":/images/side%1.png").arg(2)).mirrored());

    if(nullptr == m_vshader)
    {
        m_vshader = new QOpenGLShader(QOpenGLShader::Vertex, m_parent);
        const char *vsrc =
            "attribute highp vec4 vertexArg;\n"
            "attribute mediump vec4 texCoordArg;\n"
            "varying mediump vec4 texCoord;\n"
            "uniform mediump mat4 matrixArg;\n"
            "void main(void)\n"
            "{\n"
            "    gl_Position = matrixArg * vertexArg;\n"
            "    texCoord = texCoordArg;\n"
            "}\n";
        m_vshader->compileSourceCode(vsrc);
    }

    if(nullptr == m_fshader)
    {
        m_fshader = new QOpenGLShader(QOpenGLShader::Fragment, m_parent);
        const char *fsrc =
            "uniform sampler2D texture1Arg;\n"
            "uniform sampler2D texture2Arg;\n"
            "varying mediump vec4 texCoord;\n"
            "void main(void)\n"
            "{\n"
            "    vec4 s1 = texture2D( texture1Arg, texCoord.st );\n"
            "    vec4 s2 = texture2D( texture2Arg, texCoord.st );\n"
            "    gl_FragColor = mix( vec4( s1.r, s1.g, s1.b, 1.0 ), \n"
            "                                  vec4( s2.r * 0.6, s2.g * 0.6, s2.b * 0.6, 0.4 ),\n"
            "                                  0.35 );\n"
            "}\n";
        m_fshader->compileSourceCode(fsrc);
    }

    m_program = new QOpenGLShaderProgram();
    m_program->addShader(m_vshader);
    m_program->addShader(m_fshader);
    m_program->link();

    if(0 > m_vertexAttrId) { m_vertexAttrId = m_program->attributeLocation("vertexArg"); }
    if(0 > m_texCoordAttrId) { m_texCoordAttrId = m_program->attributeLocation("texCoordArg"); }

    if(0 > m_fromTextureId) { m_fromTextureId = m_program->uniformLocation("texture1Arg"); }
    if(0 > m_toTextureId) { m_toTextureId = m_program->uniformLocation("texture2Arg"); }
    if(0 > m_matrixId) { m_matrixId = m_program->uniformLocation("matrixArg"); }

    m_program->bindAttributeLocation("vertexArg", m_vertexAttrId);
    m_program->bindAttributeLocation("texCoordArg", m_texCoordAttrId);

    makeObject();

    m_vbo = new QOpenGLBuffer();
    m_vbo->create();
    m_vbo->bind();
    m_vbo->allocate(m_vertData.count() * sizeof(GLfloat));
    m_vbo->write(m_vertData.count(), m_vertData.constData(), sizeof(GLfloat));
    m_vbo->release();
}

void OpenGLQuizImage::paintGL(QOpenGLFunctions *f_, const RenderState *state_)
{
    m_program->bind();

    m_program->setUniformValue("texture1Arg", 0); // GL_TEXTURE0
    m_program->setUniformValue("texture2Arg", 1);  // GL_TEXTURE1

    QMatrix4x4 m;
    m.ortho(-0.5f, +0.5f, +0.5f, -0.5f, 4.0f, 15.0f);
    m.translate(0.0f, 0.0f, -10.0f);
    m.rotate(m_xRot / 16.0f, 1.0f, 0.0f, 0.0f);
    m.rotate(m_yRot / 16.0f, 0.0f, 1.0f, 0.0f);
    m.rotate(m_zRot / 16.0f, 0.0f, 0.0f, 1.0f);

    m_program->setUniformValue(m_matrixId, m);

    m_vbo->bind();
    m_vbo->write(0, m_vertData.constData(), sizeof(GLfloat)*m_vertData.count());

    m_program->enableAttributeArray(m_vertexAttrId);
    m_program->enableAttributeArray(m_texCoordAttrId);
    m_program->setAttributeBuffer(m_vertexAttrId, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
    m_program->setAttributeBuffer(m_texCoordAttrId, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));

    f_->glClearColor(m_clearColor.redF(), m_clearColor.greenF(), m_clearColor.blueF(), m_clearColor.alphaF());
    f_->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    // We are prepared both for the legacy (direct OpenGL) and the modern
    // (abstracted by RHI) OpenGL scenegraph. So set all the states that are
    // important to us.

    //! [3]
    f_->glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    f_->glEnable(GL_BLEND);
    f_->glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    // Clip support.
    if (state_->scissorEnabled()) {
        f_->glEnable(GL_SCISSOR_TEST);
        const QRect r = state_->scissorRect(); // already bottom-up
        f_->glScissor(r.x(), r.y(), r.width(), r.height());
    }
    if (state_->stencilEnabled()) {
        f_->glEnable(GL_STENCIL_TEST);
        f_->glStencilFunc(GL_EQUAL, state_->stencilValue(), 0xFF);
        f_->glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    }

    f_->glActiveTexture(GL_TEXTURE1);
    m_toTexture->bind();
    f_->glActiveTexture(GL_TEXTURE0);
    m_fromTexture->bind();

    f_->glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}




#endif // opengl

