#include "openglquizimage.h"
#include <QByteArray>
#include <QTextCodec>
#include "quizimage.h"
#include "../MastactvaModels/effectshader.h"
#include "../MastactvaModels/shader.h"
#include "../MastactvaModels/shadertype.h"
#include "../MastactvaBase/serverfiles.h"
#include "../MastactvaBase/utils.h"
#include "qmlmainobjects.h"


void ArgumentInfo::setName(const QString &name_)
{
    m_name = name_;
}

void ArgumentInfo::setType(const QString &type_)
{
    using TypeInfo = QPair<QString, QPair<int, QPair<bool, bool>>>;
    static const TypeInfo typeInfos[] = {
        {"int", {1, {false, false}}},
        {"float", {1, {true, false}}},
        {"vec2", {2, {true, false}}},
        {"vec3", {3, {true, false}}},
        {"vec4", {4, {true, false}}},
        {"mat2", {4, {true, true}}},
        {"mat3", {9, {true, true}}},
        {"mat4", {16, {true, true}}},
    };

    const auto fit = std::find_if(std::begin(typeInfos), std::end(typeInfos),
                                  [&type_](const TypeInfo &ti_)->bool
    {
        return ti_.first == type_;
    });

    Q_ASSERT(std::end(typeInfos) != fit);   // unknown type
    if(std::end(typeInfos) == fit) { return; }

    m_floatType = fit->second.second.first;
    m_matrixType = fit->second.second.second;
    m_size = fit->second.first;
    if(m_floatType) { m_valueFloat.reserve(m_size); }
    else { m_valueInt.reserve(m_size); }
}

void ArgumentInfo::setValue(const QString &value_)
{
    QString value = value_;
    value.replace(QString("("), QString(", "));
    value.replace(QString(")"), QString(", "));
    value.replace(QString("{"), QString(", "));
    value.replace(QString("}"), QString(", "));
    value.replace(QString("\n"), QString(", "));
    QStringList values = value.split(QString(","));
    int pos = 0;
    for(const QString &s : values)
    {
        QString val = s.trimmed();
        if(val.isEmpty()) { continue; }
        bool ok = false;
        if(m_floatType)
        {
            qreal nv = QVariant::fromValue(val).toDouble(&ok);
            if(!ok) { continue; }
            if(m_valueFloat.size() > pos) { m_valueFloat[pos] = nv; }
        }
        else
        {
            int nv = QVariant::fromValue(val).toInt(&ok);
            if(!ok) { continue; }
            if(m_valueInt.size() > pos) { m_valueInt[pos] = nv; }
        }
        ++pos;
    }
}

void ArgumentInfo::initId(QOpenGLShaderProgram *program_)
{
}

void ArgumentInfo::setValue(QOpenGLShaderProgram *program_)
{
    if(1 == m_size && !m_floatType)
    {
        program_->setUniformValue(m_id, m_valueInt[0]);
    }
    else if(1 == m_size && m_floatType)
    {
        program_->setUniformValue(m_id, m_valueFloat[0]);
    }
    else if(2 == m_size && m_floatType)
    {
        program_->setUniformValue(m_id, m_valueFloat[0], m_valueFloat[1]);
    }
    else if(3 == m_size && m_floatType)
    {
        program_->setUniformValue(m_id, m_valueFloat[0], m_valueFloat[1], m_valueFloat[2]);
    }
    else if(4 == m_size && m_floatType && !m_matrixType)
    {
        program_->setUniformValue(m_id, m_valueFloat[0], m_valueFloat[1], m_valueFloat[2], m_valueFloat[4]);
    }
    else if(4 == m_size && m_floatType && m_matrixType)
    {
        QMatrix2x2 m(&m_valueFloat[0]);
        program_->setUniformValue(m_id, m);
    }
    else if(9 == m_size && m_floatType && m_matrixType)
    {
        QMatrix3x3 m(&m_valueFloat[0]);
        program_->setUniformValue(m_id, m);
    }
    else if(16 == m_size && m_floatType && m_matrixType)
    {
        QMatrix4x4 m(&m_valueFloat[0]);
        program_->setUniformValue(m_id, m);
    }
    else
    {
        Q_ASSERT(false); // unknown type
    }
}


OpenGlQuizImage::OpenGlQuizImage(QObject * parent_)
{
    m_parent = parent_;
}

OpenGlQuizImage::~OpenGlQuizImage()
{
    releaseResources();
}

void OpenGlQuizImage::releaseResources()
{
}

void OpenGlQuizImage::render(const RenderState *state_)
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->initializeOpenGLFunctions();
    createTextures();
    if (nullptr == m_program) { init(f); }
    paintGL(f, state_);
}

QSGRenderNode::StateFlags OpenGlQuizImage::changedStates() const
{
    return BlendState | ScissorState | StencilState;
}

QSGRenderNode::RenderingFlags OpenGlQuizImage::flags() const
{
    return BoundedRectRendering | DepthAwareRendering;
}

QRectF OpenGlQuizImage::rect() const
{
    return QRect(0, 0, m_width, m_height);
}

void OpenGlQuizImage::sync(QQuickItem *item_)
{
    m_width = item_->width();
    m_height = item_->height();

    // element data
    QuizImage *quizImage = static_cast<QuizImage *>(item_);
    // base data
    m_t = quizImage->t();
    if(!quizImage->areAllDataAvailable()) { return; }
    // all data
    m_fromImageUrlNew = quizImage->getFromImage();
    m_toImageUrlNew = quizImage->getToImage();
    m_effect = quizImage->getEffect();
    int effectId = nullptr != m_effect ? m_effect->id() : -1;
    if(effectId != m_oldEffectId)
    {
        m_oldEffectId = effectId;
        extractArguments();
    }
}

void OpenGlQuizImage::makeObject()
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

void OpenGlQuizImage::createTextures()
{
    if(m_fromImageUrlNew != m_fromImageUrl)
    {
        m_fromImageUrl = m_fromImageUrlNew;
        m_fromTexture = new QOpenGLTexture(QImage(m_fromImageUrl).mirrored());
    }
    if(m_toImageUrlNew != m_toImageUrl)
    {
        m_toImageUrl = m_toImageUrlNew;
        m_toTexture = new QOpenGLTexture(QImage(m_toImageUrl).mirrored());
    }
}

void OpenGlQuizImage::init(QOpenGLFunctions *f_)
{
    f_->glEnable(GL_DEPTH_TEST);
    f_->glEnable(GL_CULL_FACE);

    if(nullptr == m_vshader)
    {
        m_vshader = new QOpenGLShader(QOpenGLShader::Vertex, m_parent);
        const char *vsrc =
            "attribute highp vec4 vertexArg;\n"
            "attribute mediump vec4 texCoordArg;\n"
            "uniform mediump mat4 matrixArg;\n"
            "uniform mediump mat4 texMatrix1Arg;\n"
            "uniform mediump mat4 texMatrix2Arg;\n"
            "varying mediump vec4 texCoord1Var;\n"
            "varying mediump vec4 texCoord2Var;\n"
            "void main(void)\n"
            "{\n"
            "    gl_Position = matrixArg * vertexArg;\n"
            "    texCoord1Var = texMatrix1Arg * texCoordArg;\n"
            "    texCoord2Var = texMatrix2Arg * texCoordArg;\n"
            "}\n";
        m_vshader->compileSourceCode(vsrc);
    }

    if(nullptr == m_fshader)
    {
        m_fshader = new QOpenGLShader(QOpenGLShader::Fragment, m_parent);
        const char *fsrc =
            "uniform sampler2D texture1Arg;\n"
            "uniform sampler2D texture2Arg;\n"
            "uniform mediump float t;\n"
            "varying mediump vec4 texCoord1Var;\n"
            "varying mediump vec4 texCoord2Var;\n"
            "void main(void)\n"
            "{\n"
            "    vec4 s1 = texture2D( texture1Arg, texCoord1Var.st );\n"
            "    vec4 s2 = texture2D( texture2Arg, texCoord2Var.st );\n"
            "    gl_FragColor = mix( vec4( s1.r, s1.g, s1.b, 1.0 ),\n"
            "                        vec4( s2.r, s2.g, s2.b, 1.0 ),\n"
            "                        t );\n"
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
    if(0 > m_texMatrix1Id) { m_texMatrix1Id = m_program->uniformLocation("texMatrix1Arg"); }
    if(0 > m_texMatrix2Id) { m_texMatrix2Id = m_program->uniformLocation("texMatrix2Arg"); }

    if(0 > m_tId) { m_tId = m_program->uniformLocation("t"); }
    //if(0 > m_colorId) { m_colorId = m_program->uniformLocation("color"); }

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

void OpenGlQuizImage::paintGL(QOpenGLFunctions *f_, const RenderState *state_)
{
    m_program->bind();

    m_program->setUniformValue("texture1Arg", 0); // GL_TEXTURE0
    m_program->setUniformValue("texture2Arg", 1);  // GL_TEXTURE1

    QMatrix4x4 m;
    m.ortho(-0.5f, +0.5f, +0.5f, -0.5f, 4.0f, 15.0f);
    m.translate(0.0f, 0.0f, -10.0f);
    QMatrix4x4 texm1;
    QMatrix4x4 texm2;

    m_program->setUniformValue(m_tId, (GLfloat)m_t);
    m_program->setUniformValue(m_matrixId, m);
    m_program->setUniformValue(m_texMatrix1Id, texm1);
    m_program->setUniformValue(m_texMatrix2Id, texm2);

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

void OpenGlQuizImage::extractArguments()
{
    if(nullptr == m_effect)
    {
        // reinit after
        delete m_program;
        m_program = nullptr;
    }

    ShaderTypeModel *shaderTypeModel = static_cast<ShaderTypeModel *>(QMLObjectsBase::getInstance().getListModel(g_shaderTypeModel));
    Q_ASSERT(nullptr != shaderTypeModel);
    ServerFiles *sf = QMLObjectsBase::getInstance().getServerFiles();
    Q_ASSERT(nullptr != sf);

    m_vertexShader.clear();
    m_fragmentShader.clear();

    EffectShaderModel *shaders = m_effect->getEffectShaders();
    Q_ASSERT(nullptr != shaders && shaders->isListLoaded());
    for(int i = 0; i < shaders->sizeImpl(); i++)
    {
        EffectShader *effect_shader = shaders->dataItemAtImpl(i);
        Q_ASSERT(nullptr != effect_shader);
        ShaderModel *shaderModel = effect_shader->getShader();
        Q_ASSERT(nullptr != shaderModel && shaderModel->isListLoaded() && shaderModel->sizeImpl() > 0);
        Shader *shader = shaderModel->dataItemAtImpl(0);

        Q_ASSERT(sf->isUrlDownloaded(shader->filename()));
        QUrl url(sf->get(shader->filename()));
        QFile file(url.toLocalFile());
        if(!file.open(QIODevice::ReadOnly)) { continue; }
        QByteArray fd = file.readAll();
        QTextCodec *codec = QTextCodec::codecForUtfText(fd);
        QString shaderText = codec->toUnicode(fd);

        ShaderType *shaderType = shaderTypeModel->findDataItemByIdImpl(shader->type());
        Q_ASSERT(nullptr != shaderType && (g_shaderTypeVertex == shaderType->type() || g_shaderTypeFragmet == shaderType->type()));
        if(g_shaderTypeVertex == shaderType->type())
        {
            m_vertexShader = shaderText;
        }
        else if(g_shaderTypeFragmet == shaderType->type())
        {
            m_fragmentShader = shaderText;
        }
    }

    // reinit after
    delete m_program;
    m_program = nullptr;
}
