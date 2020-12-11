#include "openglquizimage.h"
#include <QByteArray>
#include <QTextCodec>
#include "quizimage.h"
#include "../MastactvaModels/effectshader.h"
#include "../MastactvaModels/shader.h"
#include "../MastactvaModels/shadertype.h"
#include "../MastactvaModels/effectarg.h"
#include "../MastactvaModels/effectargvalue.h"
#include "../MastactvaBase/serverfiles.h"
#include "../MastactvaBase/utils.h"
#include "../MastactvaBase/qmlobjects.h"


static const QString g_noImage = ":/resources/no-image.png";


void ArgumentInfo::setArgId(int argId_)
{
    m_argId = argId_;
}

int ArgumentInfo::getArgId() const
{
    return m_argId;
}

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
    m_id = program_->attributeLocation(m_name);
}

void ArgumentInfo::setValue(QOpenGLShaderProgram *program_) const
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

    m_fromImageUrlNew = g_noImage;
    m_toImageUrlNew = g_noImage;
    extractArguments();
}

OpenGlQuizImage::~OpenGlQuizImage()
{
    releaseResources();
}

void OpenGlQuizImage::releaseResources()
{
    delete m_fromTexture;
    m_fromTexture = nullptr;
    delete m_toTexture;
    m_toTexture = nullptr;
    delete m_fromImage;
    m_fromImage = nullptr;
    delete m_toImage;
    m_toImage = nullptr;
    resetProgram();
}

void OpenGlQuizImage::render(const RenderState *state_)
{
    //QOpenGLContext *context = QOpenGLContext::currentContext();
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->initializeOpenGLFunctions();
    if (nullptr == m_program) { init(f); }
    createTextures();
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
    if(item_->width() != m_width || item_->height() != m_height)
    {
        m_updateSize = true;
    }
    m_left = item_->x();
    m_top = item_->y();
    m_width = item_->width();
    m_height = item_->height();

    // element data
    QuizImage *quizImage = static_cast<QuizImage *>(item_);
    // base data
    m_t = quizImage->t();
    if(!quizImage->areAllDataAvailable()) { return; }
    // all data
    m_fromImageUrlNew = quizImage->fromImageLocalUrl();
    if(m_fromImageUrlNew.isEmpty()) { m_fromImageUrlNew = g_noImage; }
    m_toImageUrlNew = quizImage->toImageLocalUrl();
    if(m_toImageUrlNew.isEmpty()) { m_toImageUrlNew = g_noImage; }
    m_effect = quizImage->getEffect();
    m_argumentSet = quizImage->getArgumentSet();
    int effectId = nullptr != m_effect ? m_effect->id() : -1;
    int argumentSetId = nullptr != m_argumentSet ? m_argumentSet->id() : -1;
    if(effectId != m_oldEffectId || argumentSetId != m_oldArgumentSetId)
    {
        m_oldEffectId = effectId;
        m_oldArgumentSetId = argumentSetId;
        extractArguments();
    }
}

void OpenGlQuizImage::makeObject()
{
    static const int w = 4, h = 4;
    static const int coords[h][3] =
    {
        { 1, 0, 0 }, { 0, 0, 0 }, { 0, 1, 0 }, { 1, 1, 0 }
    };

    m_vertData.resize(w * h);
    for (int j = 0; j < h; ++j) {
        // vertex position
        m_vertData[j*w + 0] = coords[j][0] * (m_width - 1);
        m_vertData[j*w + 1] = coords[j][1] * (m_height - 1);
        // texture coordinate
        m_vertData[j*w + 2] = j == 0 || j == 3;
        m_vertData[j*w + 3] = j == 0 || j == 1;
    }
}

void OpenGlQuizImage::createTextures()
{
    if(m_fromImageUrlNew == m_toImageUrl &&
            m_toImageUrlNew == m_fromImageUrl &&
            m_fromImageUrlNew != m_fromImageUrl
            )
    {
        // swap textures
        std::swap(m_fromImageUrl, m_toImageUrl);
        Q_ASSERT(m_fromImageUrl == m_fromImageUrlNew && m_toImageUrl == m_toImageUrlNew);
        std::swap(m_fromImage, m_toImage);
        std::swap(m_fromTexture, m_toTexture);
        m_t = 1.0 - m_t;
        m_updateSize = true;
        return;
    }

    if(m_fromImageUrlNew != m_fromImageUrl)
    {
        m_fromImageUrl = m_fromImageUrlNew;
        delete m_fromImage;
        if(m_fromImageUrl != g_noImage)
        {
            QUrl url(m_fromImageUrl);
            m_fromImage = new QImage(url.path());
        }
        else
        {
            m_fromImage = new QImage(m_fromImageUrl);
        }
        Q_ASSERT(!m_fromImage->isNull());
        m_fromTexture = new QOpenGLTexture(m_fromImage->mirrored(), QOpenGLTexture::GenerateMipMaps);
        m_fromTexture->setMagnificationFilter(QOpenGLTexture::Filter::LinearMipMapLinear);
        m_fromTexture->setWrapMode(QOpenGLTexture::WrapMode::ClampToBorder);
        m_fromTexture->setBorderColor(1, 1, 1, 0);
        m_updateSize = true;
    }
    if(m_toImageUrlNew != m_toImageUrl)
    {
        m_toImageUrl = m_toImageUrlNew;
        delete m_toImage;
        if(m_toImageUrl != g_noImage)
        {
            QUrl url(m_toImageUrl);
            m_toImage = new QImage(url.path());
        }
        else
        {
            m_toImage = new QImage(m_toImageUrl);
        }
        Q_ASSERT(!m_toImage->isNull());
        m_toTexture = new QOpenGLTexture(m_toImage->mirrored(), QOpenGLTexture::GenerateMipMaps);
        m_toTexture->setMagnificationFilter(QOpenGLTexture::Filter::LinearMipMapLinear);
        m_toTexture->setWrapMode(QOpenGLTexture::WrapMode::ClampToBorder);
        m_toTexture->setBorderColor(1, 1, 1, 0);
        m_updateSize = true;
    }
}

void OpenGlQuizImage::makeTextureMatrixes()
{
    m_texMatrix1 = QMatrix4x4();
    m_texMatrix2 = QMatrix4x4();
    QSize rectSize(m_width, m_height);
    if(nullptr != m_fromImage)
    {
        const QSize imageSize = m_fromImage->size();
        calculatePreserveAspectFitTextureMatrix(m_texMatrix1, imageSize, rectSize);
    }
    if(nullptr != m_toImage)
    {
        const QSize imageSize = m_toImage->size();
        calculatePreserveAspectFitTextureMatrix(m_texMatrix2, imageSize, rectSize);
    }
}

void OpenGlQuizImage::calculatePreserveAspectFitTextureMatrix(QMatrix4x4 & textureMatrix_, const QSize &imageSize_, const QSize &rectSize_)
{
    const qreal imageRate = (qreal)std::max(1, imageSize_.width())/(qreal)std::max(1, imageSize_.height());
    const qreal rectRate = (qreal)std::max(1, rectSize_.width())/(qreal)std::max(1, rectSize_.height());
    if(rectRate >= imageRate)
    {
        textureMatrix_.scale(rectRate/imageRate, 1.0);
        textureMatrix_.translate(-(rectRate - imageRate)/rectRate*0.5, 0.0);
    }
    else
    {
        textureMatrix_.scale(1.0, imageRate/rectRate);
        textureMatrix_.translate(0.0, -(imageRate - rectRate)/imageRate*0.5);
    }
}

void OpenGlQuizImage::init(QOpenGLFunctions *f_)
{
    f_->glEnable(GL_DEPTH_TEST);
    f_->glEnable(GL_CULL_FACE);

    const bool isProgramRecreated = nullptr == m_vshader || nullptr == m_fshader;
    if(nullptr == m_vshader)
    {
        m_vshader = new QOpenGLShader(QOpenGLShader::Vertex, m_parent);
        m_vshaderBA = m_vertexShader.toUtf8();
        m_vshader->compileSourceCode(m_vshaderBA.constData());
    }

    if(nullptr == m_fshader)
    {
        m_fshader = new QOpenGLShader(QOpenGLShader::Fragment, m_parent);
        m_fshaderBA = m_fragmentShader.toUtf8();
        m_fshader->compileSourceCode(m_fshaderBA.constData());
    }

    m_program = new QOpenGLShaderProgram();
    m_program->addShader(m_vshader);
    m_program->addShader(m_fshader);
    m_program->link();

    if(isProgramRecreated)
    {
        m_vertexAttrId = m_program->attributeLocation("vertexArg");
        m_texCoordAttrId = m_program->attributeLocation("texCoordArg");
        m_fromTextureId = m_program->uniformLocation("texture1Arg");
        m_toTextureId = m_program->uniformLocation("texture2Arg");
        m_opacitiId = m_program->uniformLocation("opacityArg");
        m_matrixId = m_program->uniformLocation("matrixArg");
        m_texMatrix1Id = m_program->uniformLocation("texMatrix1Arg");
        m_texMatrix2Id = m_program->uniformLocation("texMatrix2Arg");

        m_tId = m_program->uniformLocation("t");

        for(ArgumentInfo &ai: m_arguments)
        {
            ai.initId(m_program);
        }
    }

    m_program->bindAttributeLocation("vertexArg", m_vertexAttrId);
    m_program->bindAttributeLocation("texCoordArg", m_texCoordAttrId);

    makeObject();

    if(nullptr == m_vbo)
    {
        m_vbo = new QOpenGLBuffer();
        m_vbo->create();
        m_vbo->bind();
        m_vbo->allocate(m_vertData.count() * sizeof(GLfloat));
        m_vbo->write(m_vertData.count(), m_vertData.constData(), sizeof(GLfloat));
        m_vbo->release();
    }
}

void OpenGlQuizImage::paintGL(QOpenGLFunctions *f_, const RenderState *state_)
{
    m_program->bind();

    m_program->setUniformValue(m_fromTextureId, 0); // GL_TEXTURE0
    m_program->setUniformValue(m_toTextureId, 1);  // GL_TEXTURE1

    if(m_updateSize)
    {
        makeObject();
        makeTextureMatrixes();
        m_updateSize = false;
    }

    m_program->setUniformValue(m_tId, (GLfloat)m_t);
    m_program->setUniformValue(m_matrixId, *state_->projectionMatrix() * *matrix());
    m_program->setUniformValue(m_texMatrix1Id, m_texMatrix1);
    m_program->setUniformValue(m_texMatrix2Id, m_texMatrix2);
    m_program->setUniformValue(m_opacitiId, float(inheritedOpacity()));

    for(const ArgumentInfo &ai: m_arguments)
    {
        ai.setValue(m_program);
    }

    m_vbo->bind();
    m_vbo->write(0, m_vertData.constData(), sizeof(GLfloat)*m_vertData.count());

    m_program->setAttributeBuffer(m_vertexAttrId, GL_FLOAT, 0, 2, 4 * sizeof(GLfloat));
    m_program->setAttributeBuffer(m_texCoordAttrId, GL_FLOAT, 2 * sizeof(GLfloat), 2, 4 * sizeof(GLfloat));
    m_program->enableAttributeArray(m_vertexAttrId);
    m_program->enableAttributeArray(m_texCoordAttrId);

    // We are prepared both for the legacy (direct OpenGL) and the modern
    // (abstracted by RHI) OpenGL scenegraph. So set all the states that are
    // important to us.

    f_->glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    f_->glEnable(GL_BLEND);
    f_->glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    // Clip support.
    if (state_->scissorEnabled())
    {
        f_->glEnable(GL_SCISSOR_TEST);
        const QRect r = state_->scissorRect(); // already bottom-up
        f_->glScissor(r.x(), r.y(), r.width(), r.height());
    }
    if (state_->stencilEnabled())
    {
        f_->glEnable(GL_STENCIL_TEST);
        f_->glStencilFunc(GL_EQUAL, state_->stencilValue(), 0xFF);
        f_->glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    }

    f_->glActiveTexture(GL_TEXTURE1);
    m_toTexture->bind();
    f_->glActiveTexture(GL_TEXTURE0);
    m_fromTexture->bind();

    f_->glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    m_program->disableAttributeArray(m_vertexAttrId);
    m_program->disableAttributeArray(m_texCoordAttrId);
    m_vbo->release();
    m_program->release();
}

void OpenGlQuizImage::extractArguments()
{
    if(nullptr == m_effect)
    {
        initDefaultShaders();
        resetProgram();
        return;
    }

    // read shaders files
    ShaderTypeModel *shaderTypeModel = static_cast<ShaderTypeModel *>(
                QMLObjectsBase::getInstance().getListModel(g_shaderTypeModel)
                );
    Q_ASSERT(nullptr != shaderTypeModel);
    ServerFiles *sf = QMLObjectsBase::getInstance().getServerFiles();
    Q_ASSERT(nullptr != sf);

    m_vertexShader.clear();
    m_fragmentShader.clear();

    EffectShaderModel *shaders = m_effect->getEffectShaders();
    Q_ASSERT(nullptr != shaders && shaders->isListLoaded());
    for(int i = 0; i < shaders->sizeImpl(); ++i)
    {
        EffectShader *effect_shader = shaders->dataItemAtImpl(i);
        Q_ASSERT(nullptr != effect_shader);
        ShaderModel *shaderModel = effect_shader->getShader();
        Q_ASSERT(nullptr != shaderModel && shaderModel->isListLoaded() && shaderModel->sizeImpl() > 0);
        Shader *shader = shaderModel->dataItemAtImpl(0);

        Q_ASSERT(sf->isUrlDownloaded(shader->filename()));
        QString shaderText = loadFileByUrl(shader->filename());

        ShaderType *shaderType = shaderTypeModel->findDataItemByIdImpl(shader->type());
        Q_ASSERT(nullptr != shaderType &&
                    (
                        g_shaderTypeVertex == shaderType->type() ||
                        g_shaderTypeFragmet == shaderType->type()
                    )
                );
        if(g_shaderTypeVertex == shaderType->type())
        {
            m_vertexShader = shaderText;
        }
        else if(g_shaderTypeFragmet == shaderType->type())
        {
            m_fragmentShader = shaderText;
        }
    }
    initDefaultShaders();

    ShaderArgTypeModel *shaderArgTypeModel = static_cast<ShaderArgTypeModel *>(
                QMLObjectsBase::getInstance().getListModel(g_shaderArgTypeModel)
                );
    Q_ASSERT(nullptr != shaderArgTypeModel && shaderArgTypeModel->isListLoaded());

    m_arguments.clear();
    // read default arguments values
    EffectArgModel *effectArguments = m_effect->getEffectArguments();
    Q_ASSERT(nullptr != effectArguments && effectArguments->isListLoaded());
    for(int i = 0; i < effectArguments->sizeImpl(); ++i)
    {
        ArgumentInfo ai;
        EffectArg *effectArgument = effectArguments->dataItemAtImpl(i);
        Q_ASSERT(nullptr != effectArgument);
        ai.setName(effectArgument->name());
        ShaderArgType *argType = shaderArgTypeModel->findDataItemByIdImpl(effectArgument->argTypeId());
        Q_ASSERT(nullptr != argType);
        ai.setType(argType->type());
        ai.setValue(effectArgument->defaultValue());
        m_arguments.push_back(ai);
    }

    if(nullptr != m_argumentSet)
    {
        EffectArgValueModel *argumentValuesModel = m_argumentSet->getArgumentValues();
        Q_ASSERT(nullptr != argumentValuesModel && argumentValuesModel->isListLoaded());
        for(int i = 0; i < argumentValuesModel->sizeImpl(); ++i)
        {
            EffectArgValue *effectArgumentValue = argumentValuesModel->dataItemAtImpl(i);
            Q_ASSERT(nullptr != effectArgumentValue);
            EffectArgModel *effectArgumentModel = effectArgumentValue->getArg();
            Q_ASSERT(nullptr != effectArgumentModel && effectArgumentModel->isListLoaded());
            EffectArg *effectArgument = effectArgumentModel->dataItemAtImpl(0);
            Q_ASSERT(nullptr != effectArgument);
            const int argId = effectArgument->id();
            const auto fita = std::find_if(std::begin(m_arguments), std::end(m_arguments), [argId](const ArgumentInfo &ai)->bool
            {
                return ai.getArgId() == argId;
            });
            Q_ASSERT(std::end(m_arguments) != fita);
            fita->setValue(effectArgumentValue->value());
        }
    }

    resetProgram();
}

void OpenGlQuizImage::initDefaultShaders()
{
    if(m_vertexShader.isEmpty())
    {
        m_vertexShader = loadFile(":/default.vert");
    }
    if(m_fragmentShader.isEmpty())
    {
        m_fragmentShader = loadFile(":/default.frag");
    }
}

void OpenGlQuizImage::resetProgram()
{
    delete m_vshader;
    m_vshader = nullptr;
    delete m_fshader;
    m_fshader = nullptr;
    delete m_program;
    m_program = nullptr;
    delete m_vbo;
    m_vbo = nullptr;
}

QString OpenGlQuizImage::loadFile(const QString &filename_)
{
    QFile file(filename_);
    if(!file.open(QIODevice::ReadOnly)) { return QString(); }
    QByteArray fd = file.readAll();
    QTextCodec *codec = QTextCodec::codecForUtfText(fd);
    return codec->toUnicode(fd);
}

QString OpenGlQuizImage::loadFileByUrl(const QString &filenameUrl_, bool useServerFiles_ /*= true*/)
{
    if(useServerFiles_)
    {
        ServerFiles *sf = QMLObjectsBase::getInstance().getServerFiles();
        Q_ASSERT(nullptr != sf);
        QUrl url(sf->get(filenameUrl_));
        return loadFile(url.toLocalFile());
    }
    else
    {
        QUrl url(filenameUrl_);
        return loadFile(url.toLocalFile());
    }
}
