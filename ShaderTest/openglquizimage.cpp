#include "openglquizimage.h"
#include <QByteArray>
#include <QTextCodec>
#include "quizimage.h"
#include <random>
#include "quizimage.h"
#include "../MastactvaBase/utils.h"


static const QString g_rand = "rand";
static const QString g_renderRectSize = "renderRectSize";


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

const QString &ArgumentInfo::getName() const
{
    return m_name;
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
    if(m_floatType) { m_valueFloat.resize(m_size); }
    else { m_valueInt.resize(m_size); }
}

static bool set_value(const QString &valStr_, GLint& val_)
{
    bool ok = false;
    val_ = QVariant::fromValue(valStr_).toInt(&ok);
    return ok;
}

static bool set_value(const QString &valStr_, GLfloat& val_)
{
    bool ok = false;
    val_ = QVariant::fromValue(valStr_).toDouble(&ok);
    return ok;
}

template<typename Type_>
static void extractValues(const QVariantList &values_, QVector<Type_> &valuesArray_)
{
    int pos = 0;
    for(const QVariant &val_ : values_)
    {
        QString val = val_.toString().trimmed();
        if(val.isEmpty()) { continue; }
        if(pos < valuesArray_.size() && set_value(val, valuesArray_[pos]))
        {
            ++pos;
        }
    }
}

template<typename Type_>
static void extractValues(const QString &valuesStr_, QVector<Type_> &valuesArray_)
{
    QString value = valuesStr_;
    value.replace(QString("("), QString(", "));
    value.replace(QString(")"), QString(", "));
    value.replace(QString("{"), QString(", "));
    value.replace(QString("}"), QString(", "));
    value.replace(QString("\n"), QString(", "));
    QStringList values = value.split(QString(","));
    QVariantList valuesVar;
    valuesVar.reserve(values.size());
    for(const QString &s : values)
    {
        valuesVar.push_back(s);
    }
    extractValues(valuesVar, valuesArray_);
}

static void generateUniformRealRands(const QVector<GLfloat> &args_, QVector<GLfloat> &valuesArray_)
{
    if(args_.size() < 2) { return; }
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> dis(args_[0], args_[1]);
    for(int i = 0; i < valuesArray_.size(); i++)
    {
        valuesArray_[i] = dis(gen);
    }
}

static void generateUniformIntRands(const QVector<GLint> &args_, QVector<GLint> &valuesArray_)
{
    if(args_.size() < 2) { return; }
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> dis(args_[0], args_[1]);
    for(int i = 0; i < valuesArray_.size(); i++)
    {
        valuesArray_[i] = dis(gen);
    }
}

void ArgumentInfo::setValue(const QString &value_)
{
    m_renderStateInitializeFunc = &OpenGlQuizImage::renderStateInitializeNone;
    if(value_.contains(g_rand))
    {
        if(m_floatType)
        {
            QVector<GLfloat> args;
            args.resize(2);
            extractValues(value_, args);
            generateUniformRealRands(args, m_valueFloat);
        }
        else
        {
            QVector<GLint> args;
            args.resize(2);
            extractValues(value_, args);
            generateUniformIntRands(args, m_valueInt);
        }
    }
    else if(value_.contains(g_renderRectSize))
    {
        m_renderStateInitializeFunc = &OpenGlQuizImage::getRenderRectSize;
    }
    else
    {
        if(m_floatType)
        {
            extractValues(value_, m_valueFloat);
        }
        else
        {
            extractValues(value_, m_valueInt);
        }
    }
}

void ArgumentInfo::setValue(const QVariantList &values_)
{
    if(m_floatType)
    {
        extractValues(values_, m_valueFloat);
    }
    else
    {
        extractValues(values_, m_valueInt);
    }
}

void ArgumentInfo::initValueFromRenderState(OpenGlQuizImage *render_)
{
    QVariantList values;
    if(!(render_->*m_renderStateInitializeFunc)(values)) { return; }
    setValue(values);
}

void ArgumentInfo::initId(QOpenGLShaderProgram *program_)
{
    m_id = program_->uniformLocation(m_name);
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
        program_->setUniformValue(m_id, m_valueFloat[0], m_valueFloat[1], m_valueFloat[2], m_valueFloat[3]);
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


static const int g_trianglesCount = 2;
static const int g_triangleConers = 3;
static const int g_geometryVertexCoords = 3;
static const int g_geometryTextureCoords = 2;
static const int g_colorCoords = 3;


OpenGlQuizImage::OpenGlQuizImage(QObject *parent_)
{
    m_parent = parent_;

    m_geometryItemSize = g_geometryVertexCoords + g_geometryTextureCoords;

    m_fromImageUrlNew = g_noImage;
    m_toImageUrlNew = g_noImage;
    initDefaultShaders();
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
    return BlendState | ScissorState | StencilState | CullState;
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
    if(int(item_->width()) != m_width || int(item_->height()) != m_height)
    {
        qDebug() << "if(item_->width() != m_width || item_->height() != m_height)";
        m_updateSize = true;
    }
    m_left = int(item_->x());
    m_top = int(item_->y());
    m_width = int(item_->width());
    m_height = int(item_->height());

    // element data
    QuizImage *quizImage = static_cast<QuizImage *>(item_);
    // base data
    //qreal oldT = m_t;
    m_t = quizImage->t();
    //if(oldT != m_t)
    //{
    //    qDebug() << "m_t = " << m_t;
    //}
    m_fromImageUrlNew = quizImage->fromImage();
    if(m_fromImageUrlNew.isEmpty()) { m_fromImageUrlNew = g_noImage; }
    m_toImageUrlNew = quizImage->toImage();
    if(m_toImageUrlNew.isEmpty()) { m_toImageUrlNew = g_noImage; }
    //if(m_fromImageUrlNew != m_fromImageUrl || m_toImageUrlNew != m_toImageUrl)
    //{
    //    qDebug() << "m_fromImageUrl = " << m_fromImageUrl << "m_toImageUrl = " << m_toImageUrl;
    //}
    bool shadersUpdated = quizImage->shadersUpdated();
    if(shadersUpdated)
    {
        m_vertexShader = quizImage->vertexShader();
        m_fragmentShader = quizImage->fragmentShader();
        initDefaultShaders();
        quizImage->clearShadersUpdated();
    }
    if(quizImage->argumentsUpdated() || shadersUpdated)
    {
        extractArguments();
        quizImage->clearArgumentsUpdated();
    }
    quizImage->renderBuildError(m_programBuildLog);
}

int OpenGlQuizImage::getGeometryItemSize() const
{
    return m_geometryItemSize + (m_attributeColors ? 3 : 0);
}

void OpenGlQuizImage::makeObject()
{
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> dis(0.0, 1.0);

    static const int coords[g_trianglesCount][g_triangleConers][2] =
    {
        {{ 1, 0 }, { 0, 0 }, { 0, 1 }},
        {{ 1, 0 }, { 0, 1 }, { 1, 1 }}
    };

    m_vertData.resize(m_geomertyPointsWidth * m_geometryPointsHeight * g_trianglesCount * g_triangleConers * getGeometryItemSize());
    for(int y = 0; y < m_geometryPointsHeight; y++)
    {
        for(int x = 0; x < m_geomertyPointsWidth; x++)
        {
            const int offs = (y * m_geomertyPointsWidth + x) * g_trianglesCount * g_triangleConers * getGeometryItemSize();
            for (int j = 0; j < g_trianglesCount; ++j)
            {
                // one color for whole triangle
                const qreal r = dis(gen);
                const qreal g = dis(gen);
                const qreal b = dis(gen);

                for(int k = 0; k < g_triangleConers; k++)
                {
                    // vertex position
                    const int offs1 = offs + (j * g_triangleConers + k) * getGeometryItemSize();
                    if(m_geometrySolid)
                    {
                        m_vertData[offs1 + 0] = (x + coords[j][k][0]) * m_width / (GLfloat)m_geomertyPointsWidth;
                        m_vertData[offs1 + 1] = (y + coords[j][k][1]) * m_height / (GLfloat)m_geometryPointsHeight;
                    }
                    else
                    {
                        m_vertData[offs1 + 0] = (x + coords[j][k][0]) * m_width / (GLfloat)m_geomertyPointsWidth
                                - (coords[j][k][0] * 2 - 1) * m_facedGeometryXCoef
                                ;
                        m_vertData[offs1 + 1] = (y + coords[j][k][1]) * m_height / (GLfloat)m_geometryPointsHeight
                                - (coords[j][k][1] * 2 - 1) * m_facedGeometryYCoef
                                ;
                    }
                    m_vertData[offs1 + 2] = 0.1;

                    // texture coordinate
                    m_vertData[offs1 + 3] = (GLfloat)(x + coords[j][k][0])/(GLfloat)m_geomertyPointsWidth;
                    m_vertData[offs1 + 4] = 1.0 - (GLfloat)(y + coords[j][k][1])/(GLfloat)m_geometryPointsHeight;

                    // color
                    if(m_attributeColors)
                    {
                        m_vertData[offs1 + 5] = r;
                        m_vertData[offs1 + 6] = g;
                        m_vertData[offs1 + 7] = b;
                    }
                }
            }
        }
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
        //qDebug() << "m_fromImageUrl = " << m_fromImageUrl << "m_toImageUrl = " << m_toImageUrl;
        Q_ASSERT(m_fromImageUrl == m_fromImageUrlNew && m_toImageUrl == m_toImageUrlNew);
        std::swap(m_fromImage, m_toImage);
        std::swap(m_fromTexture, m_toTexture);
        qDebug() << "std::swap(m_fromImageUrl, m_toImageUrl);";
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
        //qDebug() << "m_fromImageUrl = " << m_fromImageUrl;
        Q_ASSERT(!m_fromImage->isNull());
        m_fromTexture = new QOpenGLTexture(m_fromImage->mirrored(), QOpenGLTexture::GenerateMipMaps);
        m_fromTexture->setMagnificationFilter(QOpenGLTexture::Filter::LinearMipMapLinear);
        m_fromTexture->setWrapMode(QOpenGLTexture::WrapMode::ClampToBorder);
        m_fromTexture->setBorderColor(1, 1, 1, 0);
        qDebug() << "if(m_fromImageUrlNew != m_fromImageUrl)";
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
        //qDebug() << "m_toImageUrl = " << m_toImageUrl;
        Q_ASSERT(!m_toImage->isNull());
        m_toTexture = new QOpenGLTexture(m_toImage->mirrored(), QOpenGLTexture::GenerateMipMaps);
        m_toTexture->setMagnificationFilter(QOpenGLTexture::Filter::LinearMipMapLinear);
        m_toTexture->setWrapMode(QOpenGLTexture::WrapMode::ClampToBorder);
        m_toTexture->setBorderColor(1, 1, 1, 0);
        qDebug() << "if(m_toImageUrlNew != m_toImageUrl)";
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
        //QVector4D tl(0,0,0,1), br(1, 1, 0, 1);
        //qDebug() << "tl = " <<  m_texMatrix1 * tl << "br = " << m_texMatrix1 * br;
    }
    if(nullptr != m_toImage)
    {
        const QSize imageSize = m_toImage->size();
        calculatePreserveAspectFitTextureMatrix(m_texMatrix2, imageSize, rectSize);
        //QVector4D tl(0,0,0,1), br(1, 1, 0, 1);
        //qDebug() << "tl = " <<  m_texMatrix2 * tl << "br = " << m_texMatrix2 * br;
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
    if(nullptr != m_program) { return; }

    m_vertexAttrId = -1;
    m_texCoordAttrId = -1;
    m_colorAttrId = -1;
    m_fromTextureId = -1;
    m_toTextureId = -1;
    m_opacitiId = -1;
    m_matrixId = -1;
    m_texMatrix1Id = -1;
    m_texMatrix2Id = -1;
    m_tId = -1;

    f_->glEnable(GL_DEPTH_TEST);
    f_->glEnable(GL_CULL_FACE);

    m_programBuildLog.clear();
    const bool isProgramRecreated = nullptr == m_vshader || nullptr == m_fshader;
    if(nullptr == m_vshader)
    {
        m_vshader = new QOpenGLShader(QOpenGLShader::Vertex, m_parent);
        m_vshaderBA = m_vertexShader.toUtf8();
        m_vshader->compileSourceCode(m_vshaderBA.constData());
        if(!m_vshader->isCompiled())
        {
            m_programBuildLog += m_vshader->log();
        }
    }

    if(nullptr == m_fshader)
    {
        m_fshader = new QOpenGLShader(QOpenGLShader::Fragment, m_parent);
        m_fshaderBA = m_fragmentShader.toUtf8();
        m_fshader->compileSourceCode(m_fshaderBA.constData());
        if(!m_fshader->isCompiled())
        {
            m_programBuildLog += m_fshader->log();
        }
    }

    m_program = new QOpenGLShaderProgram();
    m_program->addShader(m_vshader);
    m_program->addShader(m_fshader);
    m_program->link();
    if(!m_program->isLinked())
    {
        m_programBuildLog += m_program->log();
        return;
    }

    if(isProgramRecreated)
    {
        m_vertexAttrId = m_program->attributeLocation("vertexArg");
        m_texCoordAttrId = m_program->attributeLocation("texCoordArg");
        if(m_attributeColors)
        {
            m_colorAttrId = m_program->attributeLocation("colorArg");
        }
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
    if(m_attributeColors)
    {
        m_program->bindAttributeLocation("colorArg", m_colorAttrId);
    }
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
    if(nullptr == m_program ||
            nullptr == m_vshader ||
            nullptr == m_fshader ||
            nullptr == m_vbo
            ) { return; }

    m_program->bind();

    m_program->setUniformValue(m_fromTextureId, 0); // GL_TEXTURE0
    m_program->setUniformValue(m_toTextureId, 1);  // GL_TEXTURE1

    if(m_updateSize)
    {
        qDebug() << "m_updateSize = " << m_updateSize;
        makeObject();
        makeTextureMatrixes();
        m_updateSize = false;
    }

    for(ArgumentInfo &ai: m_arguments)
    {
        ai.initValueFromRenderState(this);
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

    m_program->setAttributeBuffer(m_vertexAttrId, GL_FLOAT,
                                  0,
                                  g_geometryVertexCoords,
                                  getGeometryItemSize() * sizeof(GLfloat)
                                  );
    m_program->setAttributeBuffer(m_texCoordAttrId, GL_FLOAT,
                                  g_geometryVertexCoords * sizeof(GLfloat),
                                  g_geometryTextureCoords,
                                  getGeometryItemSize() * sizeof(GLfloat));
    if(m_attributeColors)
    {
        m_program->setAttributeBuffer(m_colorAttrId, GL_FLOAT,
                                      (g_geometryVertexCoords + g_geometryTextureCoords) * sizeof(GLfloat),
                                      g_colorCoords,
                                      getGeometryItemSize() * sizeof(GLfloat));
    }

    m_program->enableAttributeArray(m_vertexAttrId);
    m_program->enableAttributeArray(m_texCoordAttrId);
    if(m_attributeColors)
    {
        m_program->enableAttributeArray(m_colorAttrId);
    }

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

    //f_->glEnable(GL_CULL_FACE);
    //f_->glCullFace(GL_FRONT_AND_BACK);
    f_->glFrontFace(GL_CCW);

    f_->glActiveTexture(GL_TEXTURE1);
    m_toTexture->bind();
    f_->glActiveTexture(GL_TEXTURE0);
    m_fromTexture->bind();

    if(m_geometrySolid)
    {
        f_->glDrawArrays(GL_TRIANGLES, 0, m_vertData.count()/getGeometryItemSize());
    }
    else
    {
        for(int y = 0; y < m_geometryPointsHeight; y++)
        {
            for(int x = 0; x < m_geomertyPointsWidth; x++)
            {
                const int offs = (y * m_geomertyPointsWidth + x) * g_trianglesCount * g_triangleConers;
                f_->glDrawArrays(GL_TRIANGLES, offs, g_trianglesCount * g_triangleConers);
            }
        }
    }

    m_program->disableAttributeArray(m_vertexAttrId);
    m_program->disableAttributeArray(m_texCoordAttrId);
    if(m_attributeColors)
    {
        m_program->disableAttributeArray(m_colorAttrId);
    }
    m_vbo->release();
    m_program->release();
}

void OpenGlQuizImage::extractArguments()
{
    initGeometry();

    QVector<Comment> vertexComments;
    getShaderComments(m_vertexShader, vertexComments);
    QVector<Comment> fragmentComments;
    getShaderComments(m_fragmentShader, fragmentComments);
    m_arguments.clear();
    int argId = 0;
    for(const Comment &comment: vertexComments)
    {
        if(!comment.values().contains(g_argumentName))
        {
            continue;
        }
        if(!comment.values().contains(g_typeName) ||
                !comment.values().contains(g_nameName) ||
                !comment.values().contains(g_defaultValueName))
        {
            continue;
        }

        ArgumentInfo ai;
        ai.setName(comment.values().value(g_nameName).trimmed());
        ai.setType(comment.values().value(g_typeName).trimmed());
        ai.setValue(comment.values().value(g_defaultValueName).trimmed());
        ai.setArgId(argId++);
        m_arguments.push_back(ai);
    }
    for(const Comment &comment: fragmentComments)
    {
        if(!comment.values().contains(g_argumentName))
        {
            continue;
        }
        if(!comment.values().contains(g_typeName) ||
                !comment.values().contains(g_nameName) ||
                !comment.values().contains(g_defaultValueName))
        {
            continue;
        }

        ArgumentInfo ai;
        ai.setName(comment.values().value(g_nameName).trimmed());
        auto fitc = std::find_if(std::begin(m_arguments), std::end(m_arguments),
                                       [&ai](const ArgumentInfo &ai_)->bool
        {
            return ai_.getName() == ai.getName();
        });
        if(std::end(m_arguments) == fitc)
        {
            ai.setType(comment.values().value(g_typeName).trimmed());
            ai.setValue(comment.values().value(g_defaultValueName).trimmed());
            ai.setArgId(argId++);
            m_arguments.push_back(ai);
        }
        else
        {
            //fitc->setValue(comment.values().value(g_defaultValueName).trimmed());
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

void OpenGlQuizImage::initGeometry()
{
    QVector<Comment> comments;
    getShaderComments(m_vertexShader, comments);
    const auto fitShader = std::find_if(std::begin(comments), std::end(comments),
                                  [](const Comment &comment)->bool
    {
        return comment.values().contains(g_shaderName);
    });
    if(std::end(comments) != fitShader)
    {
        const Comment &shaderComment = *fitShader;
        m_geometrySolid = !(!shaderComment.values().contains(g_geometrySolidName)
                && shaderComment.values().contains(g_geometryFacedName))
                ;
        m_attributeColors = shaderComment.values().contains(g_colorsAttributeName);

        if(shaderComment.values().contains(g_geometrySizeName))
        {
            const QString geomSizeStr = shaderComment.values().value(g_geometrySizeName);
            QVector<GLint> args;
            args.resize(2);
            extractValues(geomSizeStr, args);
            m_geomertyPointsWidth = std::max(1, args[0]);
            m_geometryPointsHeight = std::max(1, args[1]);
        }
        else
        {
            m_geomertyPointsWidth = 1;
            m_geometryPointsHeight = 1;
        }
        if(shaderComment.values().contains(g_facedGeometryCoefName))
        {
            const QString geomCoefsStr = shaderComment.values().value(g_facedGeometryCoefName);
            QVector<GLfloat> args;
            args.resize(2);
            extractValues(geomCoefsStr, args);
            m_facedGeometryXCoef = args[0];
            m_facedGeometryYCoef = args[1];
        }
        else
        {
            m_facedGeometryXCoef = 1e-3;
            m_facedGeometryYCoef = 1e-3;
        }
    }
    else
    {
        m_geometrySolid = true;
        m_geomertyPointsWidth = 1;
        m_geometryPointsHeight = 1;
        m_attributeColors = false;
        m_facedGeometryXCoef = 1e-3;
        m_facedGeometryYCoef = 1e-3;
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

bool OpenGlQuizImage::getRenderRectSize(QVariantList &values_)
{
    values_.clear();
    values_.reserve(2);
    values_.push_back(m_width);
    values_.push_back(m_height);
    return true;
}

bool OpenGlQuizImage::renderStateInitializeNone(QVariantList &values_)
{
    Q_UNUSED(values_);
    return false;
}
