#include "quizimage.h"
#include <QtGui/QOpenGLFramebufferObject>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLTexture>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLShader>
#include <QtCore/QDebug>


QuizImage::QuizImage()
{
}

#if QT_CONFIG(opengl)

class QuizImageRenderer : public QQuickFramebufferObject::Renderer
{
public:
    QuizImageRenderer(QObject * parent_);
    virtual ~QuizImageRenderer() override;
    virtual void render() override;
    virtual QOpenGLFramebufferObject *createFramebufferObject(const QSize &size_) override;
    virtual void synchronize(QQuickFramebufferObject *item_) override;

protected:
    void clearData();
    void makeObject();
    void initializeGL(QOpenGLFunctions *f_);
    void paintGL(QOpenGLFunctions *f_);

private:
    QObject *m_parent = nullptr;
    //QOpenGLTexture *m_fromImageTexture = nullptr;
    //QOpenGLTexture *m_toImageTexture = nullptr;
    QOpenGLTexture *m_textures[6] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
    QOpenGLShaderProgram *m_program = nullptr;
    QOpenGLBuffer m_vbo;
    QColor m_clearColor = Qt::black;
    QPoint m_lastPos;
    int m_xRot = 0;
    int m_yRot = 0;
    int m_zRot = 0;
    int m_vertexAttrId = -1;
    int m_texCoordAttrId = -1;
    int m_textureId = -1;
    int m_matrixId = -1;
};

QuizImageRenderer::QuizImageRenderer(QObject * parent_)
    : m_parent(parent_)
{
}

QuizImageRenderer::~QuizImageRenderer()
{
    qDebug("QuizImageRenderer destroyed");
    //delete m_fromImageTexture;
    //m_fromImageTexture = nullptr;
    //delete m_toImageTexture;
    //m_toImageTexture = nullptr;
    clearData();
}

void QuizImageRenderer::clearData()
{
    for(auto it = std::begin(m_textures); it != std::end(m_textures); ++it)
    {
        delete *it;
        *it = nullptr;
    }
    delete m_program;
    m_program = nullptr;
}

void QuizImageRenderer::initializeGL(QOpenGLFunctions *f_)
{
    if(nullptr != m_program) { return; }

    makeObject();

    f_->glEnable(GL_DEPTH_TEST);
    f_->glEnable(GL_CULL_FACE);

    QOpenGLShader *vshader = new QOpenGLShader(QOpenGLShader::Vertex, m_parent);
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
    vshader->compileSourceCode(vsrc);

    QOpenGLShader *fshader = new QOpenGLShader(QOpenGLShader::Fragment, m_parent);
    const char *fsrc =
        "uniform sampler2D texture;\n"
        "varying mediump vec4 texc;\n"
        "void main(void)\n"
        "{\n"
        "    gl_FragColor = texture2D(texture, texc.st);\n"
        "}\n";
    fshader->compileSourceCode(fsrc);

    m_program = new QOpenGLShaderProgram();
    m_program->addShader(vshader);
    m_program->addShader(fshader);
    m_program->link();
    m_program->bind();

    m_vertexAttrId = m_program->attributeLocation("vertex");
    m_texCoordAttrId = m_program->attributeLocation("texCoord");
    m_textureId = m_program->uniformLocation("texture");
    m_matrixId = m_program->uniformLocation("matrix");

    m_program->bindAttributeLocation("vertex", m_vertexAttrId);
    m_program->bindAttributeLocation("texCoord", m_texCoordAttrId);

    m_program->setUniformValue(m_textureId, 0);
}

void QuizImageRenderer::makeObject()
{
    static const int coords[6][4][3] = {
        { { +1, -1, -1 }, { -1, -1, -1 }, { -1, +1, -1 }, { +1, +1, -1 } },
        { { +1, +1, -1 }, { -1, +1, -1 }, { -1, +1, +1 }, { +1, +1, +1 } },
        { { +1, -1, +1 }, { +1, -1, -1 }, { +1, +1, -1 }, { +1, +1, +1 } },
        { { -1, -1, -1 }, { -1, -1, +1 }, { -1, +1, +1 }, { -1, +1, -1 } },
        { { +1, -1, +1 }, { -1, -1, +1 }, { -1, -1, -1 }, { +1, -1, -1 } },
        { { -1, -1, +1 }, { +1, -1, +1 }, { +1, +1, +1 }, { -1, +1, +1 } }
    };

    for (int j = 0; j < 6; ++j)
        m_textures[j] = new QOpenGLTexture(QImage(QString(":/images/side%1.png").arg(j + 1)).mirrored());

    QVector<GLfloat> vertData;
    for (int i = 0; i < 6; ++i) {
        for (int j = 0; j < 4; ++j) {
            // vertex position
            vertData.append(0.2 * coords[i][j][0]);
            vertData.append(0.2 * coords[i][j][1]);
            vertData.append(0.2 * coords[i][j][2]);
            // texture coordinate
            vertData.append(j == 0 || j == 3);
            vertData.append(j == 0 || j == 1);
        }
    }

    m_vbo.create();
    m_vbo.bind();
    m_vbo.allocate(vertData.constData(), vertData.count() * sizeof(GLfloat));
}

void QuizImageRenderer::paintGL(QOpenGLFunctions *f_)
{
    f_->glClearColor(m_clearColor.redF(), m_clearColor.greenF(), m_clearColor.blueF(), m_clearColor.alphaF());
    f_->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QMatrix4x4 m;
    m.ortho(-0.5f, +0.5f, +0.5f, -0.5f, 4.0f, 15.0f);
    m.translate(0.0f, 0.0f, -10.0f);
    m.rotate(m_xRot / 16.0f, 1.0f, 0.0f, 0.0f);
    m.rotate(m_yRot / 16.0f, 0.0f, 1.0f, 0.0f);
    m.rotate(m_zRot / 16.0f, 0.0f, 0.0f, 1.0f);

    m_program->setUniformValue(m_matrixId, m);
    m_program->enableAttributeArray(m_vertexAttrId);
    m_program->enableAttributeArray(m_texCoordAttrId);
    m_program->setAttributeBuffer(m_vertexAttrId, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
    m_program->setAttributeBuffer(m_texCoordAttrId, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));

    for (int i = 0; i < 6; ++i) {
        m_textures[i]->bind();
        f_->glDrawArrays(GL_TRIANGLE_FAN, i * 4, 4);
    }
}

void QuizImageRenderer::render()
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->initializeOpenGLFunctions();
    if(nullptr == m_program)
    {
        initializeGL(f);
    }
    paintGL(f);
}

QOpenGLFramebufferObject *QuizImageRenderer::createFramebufferObject(const QSize &size_)
{
    qDebug() << "Creating FBO" << size_;
    clearData();
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    return new QOpenGLFramebufferObject(size_, format);
}

void QuizImageRenderer::synchronize(QQuickFramebufferObject *item_)
{
    clearData();
    QuizImage *item = static_cast<QuizImage *>(item_);
    Q_UNUSED(item);
    qDebug() << "Sync";
    m_xRot += 10.0;
}

#endif


QQuickFramebufferObject::Renderer *QuizImage::createRenderer() const
{
#if QT_CONFIG(opengl)
    return new QuizImageRenderer(const_cast<QuizImage *>(this));
#else
    return nullptr;
#endif
}

void QuizImage::updateState()
{
    update();
}
