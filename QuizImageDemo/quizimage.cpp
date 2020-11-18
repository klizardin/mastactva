#include "quizimage.h"
#include <QtGui/QOpenGLFramebufferObject>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLTexture>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLShader>
#include <QtCore/QDebug>


QuizImageFB::QuizImageFB()
{
}

#if QT_CONFIG(opengl)

class QuizImageRendererFB : public QQuickFramebufferObject::Renderer
{
public:
    QuizImageRendererFB(QObject * parent_);
    virtual ~QuizImageRendererFB() override;
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
    bool m_objectCreated = false;
    QVector<GLfloat> m_vertData;
    QOpenGLTexture *m_fromTexture = nullptr;
    QOpenGLTexture *m_toTexture = nullptr;
    QOpenGLShaderProgram *m_program = nullptr;
    QOpenGLShader *m_vshader = nullptr;
    QOpenGLShader *m_fshader = nullptr;
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

QuizImageRendererFB::QuizImageRendererFB(QObject * parent_)
    : m_parent(parent_)
{
}

QuizImageRendererFB::~QuizImageRendererFB()
{
    delete m_vshader;
    m_vshader = nullptr;
    delete m_fshader;
    m_fshader = nullptr;
    delete m_fromTexture;
    m_fromTexture = nullptr;
    delete m_toTexture;
    m_toTexture = nullptr;
    clearData();
}

void QuizImageRendererFB::clearData()
{
    delete m_program;
    m_program = nullptr;
}

void QuizImageRendererFB::initializeGL(QOpenGLFunctions *f_)
{
    if(nullptr != m_program) { return; }

    makeObject();

    f_->glEnable(GL_DEPTH_TEST);
    f_->glEnable(GL_CULL_FACE);

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
}

void QuizImageRendererFB::makeObject()
{
    if(!m_objectCreated)
    {
        static const int coords[4][3] = {
            { +1, -1, -1 }, { -1, -1, -1 }, { -1, +1, -1 }, { +1, +1, -1 }
        };

        m_fromTexture = new QOpenGLTexture(QImage(QString(":/images/side%1.png").arg(1)).mirrored());
        m_toTexture = new QOpenGLTexture(QImage(QString(":/images/side%1.png").arg(2)).mirrored());

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

        m_vbo.create();
    }
    m_vbo.bind();
    m_vbo.allocate(m_vertData.constData(), m_vertData.count() * sizeof(GLfloat));
    m_objectCreated = true;
}

void QuizImageRendererFB::paintGL(QOpenGLFunctions *f_)
{
    m_program->bind();
    m_program->setUniformValue(m_textureId, 0);

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

    m_fromTexture->bind();
    f_->glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void QuizImageRendererFB::render()
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->initializeOpenGLFunctions();
    if(nullptr == m_program)
    {
        initializeGL(f);
    }
    paintGL(f);

    clearData();
    update();
}

QOpenGLFramebufferObject *QuizImageRendererFB::createFramebufferObject(const QSize &size_)
{
    clearData();
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    return new QOpenGLFramebufferObject(size_, format);
}

void QuizImageRendererFB::synchronize(QQuickFramebufferObject *item_)
{
    clearData();
    QuizImageFB *item = static_cast<QuizImageFB *>(item_);
    Q_UNUSED(item);
    m_xRot += 10.0;
}

#endif


QQuickFramebufferObject::Renderer *QuizImageFB::createRenderer() const
{
#if QT_CONFIG(opengl)
    return new QuizImageRendererFB(const_cast<QuizImageFB *>(this));
#else
    return nullptr;
#endif
}

void QuizImageFB::updateState()
{
    update();
}
