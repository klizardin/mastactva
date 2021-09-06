/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "openglquizimage.h"
#include <QByteArray>
#include <QTextCodec>
#include <random>
#include "../MastactvaBase/quizimage.h"
#include "../MastactvaBase/serverfiles.h"
#include "../MastactvaBase/utils.h"
#include "../MastactvaBase/qmlobjects.h"
#include "../MastactvaModels/objectartefact.h"
#include "../MastactvaModels/artefact.h"
#include "../MastactvaModels/artefacttype.h"
#include "../MastactvaModels/effectarg.h"
#include "../MastactvaModels/effectargvalue.h"
#include "../MastactvaBase/defines.h"
#include "../MastactvaBase/utils.h"


/*OpenGlQuizImage::OpenGlQuizImage()
{
#if defined(TRACE_THREADS)
    qDebug() << "OpenGlQuizImage::OpenGlQuizImage()" << QThread::currentThread() << QThread::currentThreadId();
#endif

    //m_data.extractArguments(nullptr, nullptr);
    initGeometry();
    resetProgram();
}

OpenGlQuizImage::~OpenGlQuizImage()
{
    OpenGlQuizImage::releaseResources();
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
    freeDrawingData();
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
        m_updateSize = true;
    }
#if defined(TRACE_THREADS_QUIZIMAGE)
    //qDebug() << "OpenGlQuizImage::sync()" << QThread::currentThread() << QThread::currentThreadId();
#endif
    m_left = int(item_->x());
    m_top = int(item_->y());
    m_width = int(item_->width());
    m_height = int(item_->height());

    // element data
    QuizImage *quizImage = static_cast<QuizImage *>(item_);

    // base data
    m_t = quizImage->t();
    if(!quizImage->isImageDataUpdated()) { return; }

    // all data
    const bool effectUpdated = quizImage->isEffectUpdated();
    freeDrawingData();
    m_drawingData = quizImage->getData();
    if(effectUpdated)
    {
        initGeometry();
        resetProgram();
    }
    quizImage->retryData();
    quizImage->renderBuildError(m_programBuildLog);
}

static const int g_trianglesCount = 2;
static const int g_triangleConers = 3;
static const int g_geometryVertexCoords = 3;
static const int g_geometryTextureCoords = 2;
static const int g_geometryItemSize = g_geometryVertexCoords + g_geometryTextureCoords;

void OpenGlQuizImage::makeObject()
{
    static const int coords[g_trianglesCount][g_triangleConers][2] =
    {
        {{ 1, 0 }, { 0, 0 }, { 0, 1 }},
        {{ 1, 0 }, { 0, 1 }, { 1, 1 }}
    };

    m_vertData.resize(m_geomertyPointsWidth * m_gg_triangleConerseometryPointsHeight *
                      g_trianglesCount * g_triangleConers * g_geometryItemSize);
    for(int y = 0; y < m_geometryPointsHeight; y++)
    {
        for(int x = 0; x < m_geomertyPointsWidth; x++)
        {
            const int offs = (y * m_geomertyPointsWidth + x) *
                    g_trianglesCount * g_triangleConers * g_geometryItemSize;
            for (int j = 0; j < g_trianglesCount; ++j)
            {
                for(int k = 0; k < g_triangleConers; k++)
                {
                    // vertex position
                    const int offs1 = offs + (j * g_triangleConers + k) * g_geometryItemSize;
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
                }
            }
        }
    }
}

void OpenGlQuizImage::createTextures()
{
    if(m_data.isSwapImages())
    {
        // swap textures
        m_data.swapImages();
        std::swap(m_fromImage, m_toImage);
        std::swap(m_fromTexture, m_toTexture);
        m_updateSize = true;
#if defined(TRACE_THREADS_QUIZIMAGE)
        qDebug() << "OpenGlQuizImage::createTextures()" << QThread::currentThread() << QThread::currentThreadId();
#endif
        return;
    }

    if(m_data.fromImageUrlChanged())
    {
        m_data.useNewFromImageUrl();
        delete m_fromImage;
        if(m_data.isFromImageIsUrl())
        {
            QUrl url(m_data.getFromImageUrl());
            m_fromImage = new QImage(url.path());
        }
        else
        {
            m_fromImage = new QImage(m_data.getFromImageUrl());
        }
        Q_ASSERT(!m_fromImage->isNull());
        m_fromTexture = new QOpenGLTexture(m_fromImage->mirrored(), QOpenGLTexture::GenerateMipMaps);
        m_fromTexture->setMagnificationFilter(QOpenGLTexture::Filter::LinearMipMapLinear);
        m_fromTexture->setWrapMode(QOpenGLTexture::WrapMode::ClampToBorder);
        m_fromTexture->setBorderColor(1, 1, 1, 0);
        m_updateSize = true;
#if defined(TRACE_THREADS_QUIZIMAGE)
        qDebug() << "OpenGlQuizImage::createTextures()" << QThread::currentThread() << QThread::currentThreadId();
#endif
    }

    if(m_data.toImageUrlChanged())
    {
        m_data.useNewToImageUrl();
        delete m_toImage;
        if(m_data.isFromImageIsUrl())
        {
            QUrl url(m_data.getToImageUrl());
            m_toImage = new QImage(url.path());
        }
        else
        {
            m_toImage = new QImage(m_data.getToImageUrl());
        }
        Q_ASSERT(!m_toImage->isNull());
        m_toTexture = new QOpenGLTexture(m_toImage->mirrored(), QOpenGLTexture::GenerateMipMaps);
        m_toTexture->setMagnificationFilter(QOpenGLTexture::Filter::LinearMipMapLinear);
        m_toTexture->setWrapMode(QOpenGLTexture::WrapMode::ClampToBorder);
        m_toTexture->setBorderColor(1, 1, 1, 0);
        m_updateSize = true;
#if defined(TRACE_THREADS_QUIZIMAGE)
        qDebug() << "OpenGlQuizImage::createTextures()" << QThread::currentThread() << QThread::currentThreadId();
#endif
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
#if defined(TRACE_THREADS_QUIZIMAGE)
        qDebug() << "OpenGlQuizImage::makeTextureMatrixes()" << QThread::currentThread() << QThread::currentThreadId();
#endif
    }
    if(nullptr != m_toImage)
    {
        const QSize imageSize = m_toImage->size();
        calculatePreserveAspectFitTextureMatrix(m_texMatrix2, imageSize, rectSize);
        //QVector4D tl(0,0,0,1), br(1, 1, 0, 1);
        //qDebug() << "tl = " <<  m_texMatrix2 * tl << "br = " << m_texMatrix2 * br;
#if defined(TRACE_THREADS_QUIZIMAGE)
        qDebug() << "OpenGlQuizImage::makeTextureMatrixes()" << QThread::currentThread() << QThread::currentThreadId();
#endif
    }
}

void OpenGlQuizImage::calculatePreserveAspectFitTextureMatrix(QMatrix4x4 & textureMatrix_,
                                                              const QSize &imageSize_,
                                                              const QSize &rectSize_)
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

#if defined(TRACE_THREADS_QUIZIMAGE)
    qDebug() << "OpenGlQuizImage::init()" << QThread::currentThread() << QThread::currentThreadId();
#endif

    m_vertexAttrId = -1;
    m_texCoordAttrId = -1;
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
        m_vshader = new QOpenGLShader(QOpenGLShader::Vertex, nullptr);
        m_vshaderBA = m_data.getVertexShader().toUtf8();
        m_vshader->compileSourceCode(m_vshaderBA.constData());
        if(!m_vshader->isCompiled())
        {
            m_programBuildLog += m_vshader->log();
        }
    }

    if(nullptr == m_fshader)
    {
        m_fshader = new QOpenGLShader(QOpenGLShader::Fragment, nullptr);
        m_fshaderBA = m_data.getFragmentShader().toUtf8();
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
        m_fromTextureId = m_program->uniformLocation("texture1Arg");
        m_toTextureId = m_program->uniformLocation("texture2Arg");
        m_opacitiId = m_program->uniformLocation("opacityArg");
        m_matrixId = m_program->uniformLocation("matrixArg");
        m_texMatrix1Id = m_program->uniformLocation("texMatrix1Arg");
        m_texMatrix2Id = m_program->uniformLocation("texMatrix2Arg");

        m_tId = m_program->uniformLocation("t");

        for(ArgumentInfo &ai: m_data.getArgumentsNC())
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
        makeObject();
        makeTextureMatrixes();
        m_updateSize = false;
    }

    for(ArgumentInfo &ai: m_data.getArgumentsNC())
    {
        ai.initValueFromRenderState(this);
    }

    m_program->setUniformValue(m_tId, (GLfloat)m_t);
    m_program->setUniformValue(m_matrixId, *state_->projectionMatrix() * *matrix());
    m_program->setUniformValue(m_texMatrix1Id, m_texMatrix1);
    m_program->setUniformValue(m_texMatrix2Id, m_texMatrix2);
    m_program->setUniformValue(m_opacitiId, float(inheritedOpacity()));

    for(const ArgumentInfo &ai: qAsConst(m_data.getArgumentsNC()))
    {
        ai.setValue(m_program);
    }

    m_vbo->bind();
    m_vbo->write(0, m_vertData.constData(), sizeof(GLfloat)*m_vertData.count());

    m_program->setAttributeBuffer(m_vertexAttrId, GL_FLOAT,
                                  0,
                                  g_geometryVertexCoords,
                                  g_geometryItemSize * sizeof(GLfloat)
                                  );
    m_program->setAttributeBuffer(m_texCoordAttrId, GL_FLOAT,
                                  g_geometryVertexCoords * sizeof(GLfloat),
                                  g_geometryTextureCoords,
                                  g_geometryItemSize * sizeof(GLfloat));
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

    //f_->glEnable(GL_CULL_FACE);
    //f_->glCullFace(GL_FRONT_AND_BACK);
    f_->glFrontFace(GL_CCW);

    f_->glActiveTexture(GL_TEXTURE1);
    m_toTexture->bind();
    f_->glActiveTexture(GL_TEXTURE0);
    m_fromTexture->bind();

    if(m_geometrySolid)
    {
        f_->glDrawArrays(GL_TRIANGLES, 0, m_vertData.count()/g_geometryItemSize);
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
    m_vbo->release();
    m_program->release();
}

void OpenGlQuizImage::initDefaultShaders()
{
    m_data.initDefaultShaders();
}

void OpenGlQuizImage::initGeometry()
{
    QVector<Comment> comments;
    getShaderComments(m_data.getVertexShader(), comments);
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

void OpenGlQuizImage::freeDrawingData()
{
    delete m_drawingData;
    m_drawingData = nullptr;
}
*/


OpenGlQuizImage::OpenGlQuizImage()
{
#if defined(TRACE_THREADS)
    qDebug() << "OpenGlQuizImage::OpenGlQuizImage()" << QThread::currentThread() << QThread::currentThreadId();
#endif
}

OpenGlQuizImage::~OpenGlQuizImage()
{
    OpenGlQuizImage::releaseResources();
}

void OpenGlQuizImage::releaseResources()
{
}

void OpenGlQuizImage::render(const RenderState *state_)
{
    //QOpenGLContext *context = QOpenGLContext::currentContext();
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->initializeOpenGLFunctions();
    if (nullptr != m_drawingData)
    {
        if(!m_drawingData->isInitialized())
        {
            init(f);
        }
        paintGL(f, state_);
    }
    else
    {
        noPaintGL(f, state_);
    }
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
#if defined(TRACE_THREADS_QUIZIMAGE)
    //qDebug() << "OpenGlQuizImage::sync()" << QThread::currentThread() << QThread::currentThreadId();
#endif
    QuizImage *quizImage = static_cast<QuizImage *>(item_);

    // render arguments is be updated
    m_left = int(item_->x());
    m_top = int(item_->y());
    m_width = int(item_->width());
    m_height = int(item_->height());
    m_t = quizImage->t();

    if(!quizImage->isImageDataUpdated())
    {
        updateRenderArguments();
        return;
    }

    // all drawing data need to be updated
    m_drawingData = quizImage->getData();
    updateRenderArguments();
    quizImage->retryData();
    quizImage->renderBuildError(m_programBuildLog);
}

void OpenGlQuizImage::init(QOpenGLFunctions *f_)
{
    if(nullptr == m_drawingData ||
            m_drawingData->isInitialized()
            ) { return; }

#if defined(TRACE_THREADS_QUIZIMAGE)
    qDebug() << "OpenGlQuizImage::init()" << QThread::currentThread() << QThread::currentThreadId();
#endif

    /*m_vertexAttrId = -1;
    m_texCoordAttrId = -1;
    m_fromTextureId = -1;
    m_toTextureId = -1;
    m_opacitiId = -1;
    m_matrixId = -1;
    m_texMatrix1Id = -1;
    m_texMatrix2Id = -1;
    m_tId = -1;*/

    f_->glEnable(GL_DEPTH_TEST);
    f_->glEnable(GL_CULL_FACE);

    m_programBuildLog.clear();

    for(int i = 0; i < m_drawingData->stepCount(); i++)
    {
        //m_drawingData->clearStepArgumentIds(i);
        const bool isProgramCreated = m_drawingData->buildStepProgram(i, m_programBuildLog);

        /*const bool isProgramRecreated = nullptr == m_vshader || nullptr == m_fshader;
        if(nullptr == m_vshader)
        {
            m_vshader = new QOpenGLShader(QOpenGLShader::Vertex, nullptr);
            m_vshaderBA = m_data.getVertexShader().toUtf8();
            m_vshader->compileSourceCode(m_vshaderBA.constData());
            if(!m_vshader->isCompiled())
            {
                m_programBuildLog += m_vshader->log();
            }
        }

        if(nullptr == m_fshader)
        {
            m_fshader = new QOpenGLShader(QOpenGLShader::Fragment, nullptr);
            m_fshaderBA = m_data.getFragmentShader().toUtf8();
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
        }*/

        if(isProgramCreated)
        {
            /*m_vertexAttrId = m_program->attributeLocation("vertexArg");
            m_texCoordAttrId = m_program->attributeLocation("texCoordArg");
            m_fromTextureId = m_program->uniformLocation("texture1Arg");
            m_toTextureId = m_program->uniformLocation("texture2Arg");
            m_opacitiId = m_program->uniformLocation("opacityArg");
            m_matrixId = m_program->uniformLocation("matrixArg");
            m_texMatrix1Id = m_program->uniformLocation("texMatrix1Arg");
            m_texMatrix2Id = m_program->uniformLocation("texMatrix2Arg");

            m_tId = m_program->uniformLocation("t");
            */

            /*for(ArgumentInfo &ai: m_data.getArgumentsNC())
            {
                ai.initId(m_program);
            }*/

            m_drawingData->createStepArgument(i);
        }

        /*m_program->bindAttributeLocation("vertexArg", m_vertexAttrId);
        m_program->bindAttributeLocation("texCoordArg", m_texCoordAttrId);
        */
        m_drawingData->bindStep(i);

        /*makeObject();*/

        /*if(nullptr == m_vbo)
        {
            m_vbo = new QOpenGLBuffer();
            m_vbo->create();
            m_vbo->bind();
            m_vbo->allocate(m_vertData.count() * sizeof(GLfloat));
            m_vbo->write(m_vertData.count(), m_vertData.constData(), sizeof(GLfloat));
            m_vbo->release();
        }*/
        m_drawingData->buildStepVBO(i);
    }
}

void OpenGlQuizImage::paintGL(QOpenGLFunctions *f_, const RenderState *state_)
{
    noPaintGL(f_, state_);

    if(nullptr == m_drawingData ||
            !m_drawingData->isInitialized()
            ) { return; }

    m_drawingData->setRenderArgumentValue(
                g_renderOpacityName,
                QVector<GLfloat>({GLfloat(inheritedOpacity()), })
                );

    for(int i = 0; i < m_drawingData->stepCount(); i++)
    {
        if(!m_drawingData->isStepProgramBuilded(i)) { continue; }
        m_drawingData->bindStepProgram(i);

        /*m_program->bind();*/

        m_drawingData->useStepArguments(i);

        /*m_program->setUniformValue(m_fromTextureId, 0); // GL_TEXTURE0
        m_program->setUniformValue(m_toTextureId, 1);  // GL_TEXTURE1

        if(m_updateSize)
        {
            makeObject();
            makeTextureMatrixes();
            m_updateSize = false;
        }

        for(ArgumentInfo &ai: m_data.getArgumentsNC())
        {
            ai.initValueFromRenderState(this);
        }

        m_program->setUniformValue(m_tId, (GLfloat)m_t);
        m_program->setUniformValue(m_matrixId, *state_->projectionMatrix() * *matrix());
        m_program->setUniformValue(m_texMatrix1Id, m_texMatrix1);
        m_program->setUniformValue(m_texMatrix2Id, m_texMatrix2);
        m_program->setUniformValue(m_opacitiId, float(inheritedOpacity()));

        for(const ArgumentInfo &ai: qAsConst(m_data.getArgumentsNC()))
        {
            ai.setValue(m_program);
        }

        m_vbo->bind();
        m_vbo->write(0, m_vertData.constData(), sizeof(GLfloat)*m_vertData.count());

        m_program->setAttributeBuffer(m_vertexAttrId, GL_FLOAT,
                                      0,
                                      g_geometryVertexCoords,
                                      g_geometryItemSize * sizeof(GLfloat)
                                      );
        m_program->setAttributeBuffer(m_texCoordAttrId, GL_FLOAT,
                                      g_geometryVertexCoords * sizeof(GLfloat),
                                      g_geometryTextureCoords,
                                      g_geometryItemSize * sizeof(GLfloat));
        m_program->enableAttributeArray(m_vertexAttrId);
        m_program->enableAttributeArray(m_texCoordAttrId);
        */

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

        /*f_->glActiveTexture(GL_TEXTURE1);
        m_toTexture->bind();
        f_->glActiveTexture(GL_TEXTURE0);
        m_fromTexture->bind();
        */

        m_drawingData->bindStepTexture(i, f_);

        /*if(m_geometrySolid)
        {
            f_->glDrawArrays(GL_TRIANGLES, 0, m_vertData.count()/g_geometryItemSize);
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
        }*/

        m_drawingData->drawStep(i, f_);

        /*m_program->disableAttributeArray(m_vertexAttrId);
        m_program->disableAttributeArray(m_texCoordAttrId);
        m_vbo->release();
        m_program->release();*/

        m_drawingData->releaseStep(i);
    }
}

void OpenGlQuizImage::noPaintGL(QOpenGLFunctions *f_, const RenderState *state_)
{
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
    f_->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ACCUM_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void OpenGlQuizImage::updateRenderArguments()
{
    if(nullptr == m_drawingData) { return; }
    m_drawingData->setRenderArgumentValue(
                g_renderScreenRectName,
                QVector<GLfloat>({GLfloat(m_left), GLfloat(m_top), GLfloat(m_width), GLfloat(m_height)})
                );
    m_drawingData->setRenderArgumentValue(
                g_renderTName,
                QVector<GLfloat>({GLfloat(m_t),})
                );
}
