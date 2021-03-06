#include "openglquizimagedemo.h"
#include "quizimagedemo.h"


OpenGlQuizImageDemo::OpenGlQuizImageDemo()
{
#if defined(TRACE_THREADS)
    qDebug() << "OpenGlQuizImageDemo::OpenGlQuizImageDemo()" << QThread::currentThread() << QThread::currentThreadId();
#endif
}

OpenGlQuizImageDemo::~OpenGlQuizImageDemo()
{
    OpenGlQuizImageDemo::releaseResources();
}

void OpenGlQuizImageDemo::releaseResources()
{
}

void OpenGlQuizImageDemo::render(const RenderState *state_)
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

QSGRenderNode::StateFlags OpenGlQuizImageDemo::changedStates() const
{
    return BlendState | ScissorState | StencilState | CullState;
}

QSGRenderNode::RenderingFlags OpenGlQuizImageDemo::flags() const
{
    return BoundedRectRendering | DepthAwareRendering;
}

QRectF OpenGlQuizImageDemo::rect() const
{
    return QRect(0, 0, m_width, m_height);
}

void OpenGlQuizImageDemo::sync(QQuickItem *item_)
{
#if defined(TRACE_THREADS_QUIZIMAGE)
    //qDebug() << "OpenGlQuizImageDemo::sync()" << QThread::currentThread() << QThread::currentThreadId();
#endif
    QuizImageDemo *quizImage = static_cast<QuizImage *>(item_);

    // render arguments is be updated
    m_left = int(item_->x());
    m_top = int(item_->y());
    m_width = int(item_->width());
    m_height = int(item_->height());
    m_t = quizImage->t();

    quizImage->renderBuildError(m_programBuildLog);
    if(!quizImage->isImageDataUpdated())
    {
        updateRenderArguments(true);
        return;
    }

    // all drawing data need to be updated
    OpenGLDrawingImageData *old = m_drawingData;
    m_drawingData = quizImage->getData();
    quizImage->retryData(old);
    updateRenderArguments(false);
}

void OpenGlQuizImageDemo::init(QOpenGLFunctions *f_)
{
    if(nullptr == m_drawingData ||
            m_drawingData->isInitialized()
            ) { return; }

#if defined(TRACE_THREADS_QUIZIMAGE)
    qDebug() << "OpenGlQuizImageDemo::init()" << QThread::currentThread() << QThread::currentThreadId();
#endif

    f_->glEnable(GL_DEPTH_TEST);
    f_->glEnable(GL_CULL_FACE);

    qDebug() << "glEnable(GL_DEPTH_TEST)";
    qDebug() << "glEnable(GL_CULL_FACE)";

    m_programBuildLog.clear();

    for(int i = 0; i < m_drawingData->stepCount(); i++)
    {
        const bool isProgramCreated = m_drawingData->buildStepProgram(i, m_programBuildLog);
        if(!isProgramCreated) { continue; }

        m_drawingData->createStepArgument(i, f_);
        m_drawingData->createStepTextures(i);
        m_drawingData->bindStep(i);
        m_drawingData->buildStepVBO(i);
    }
}

QVector<GLfloat> matrixToVector(const QMatrix4x4 &mat_)
{
    static const int sz = 16;
    QVector<GLfloat> res(sz);
    std::copy(mat_.constData(), mat_.constData() + sz, std::begin(res));
    return res;
}

void OpenGlQuizImageDemo::paintGL(QOpenGLFunctions *f_, const RenderState *state_)
{
    noPaintGL(f_, state_);

    if(nullptr == m_drawingData ||
            !m_drawingData->isInitialized()
            ) { return; }

    m_drawingData->setRenderArgumentValue(
                g_renderOpacityName,
                QVector<GLfloat>({GLfloat(inheritedOpacity()), }),
                -1
                );
    QVector<GLfloat> matIdentityData = matrixToVector(*state_->projectionMatrix() * *matrix());
    m_drawingData->setRenderArgumentValue(
                g_renderMatrixName,
                matIdentityData,
                matIdentityData.size()
                );

    for(int i = 0; i < m_drawingData->stepCount(); i++)
    {
        if(!m_drawingData->isStepProgramBuilded(i)) { continue; }
        m_drawingData->bindStepProgramAndVBO(i);

        m_drawingData->writeStepVBO(i);
        m_drawingData->useStepArguments(i);

        // We are prepared both for the legacy (direct OpenGL) and the modern
        // (abstracted by RHI) OpenGL scenegraph. So set all the states that are
        // important to us.

        f_->glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        qDebug() << "glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE)";

        f_->glEnable(GL_BLEND);
        f_->glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        qDebug() << "glEnable(GL_BLEND)";
        qDebug() << "glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA)";

        // Clip support.
        if (state_->scissorEnabled())
        {
            f_->glEnable(GL_SCISSOR_TEST);
            qDebug() << "glEnable(GL_SCISSOR_TEST)";
            const QRect r = state_->scissorRect(); // already bottom-up
            f_->glScissor(r.x(), r.y(), r.width(), r.height());
            qDebug() << "glScissor(" << r.x() << r.y() << r.width() << r.height() << ")";
        }
        if (state_->stencilEnabled())
        {
            f_->glEnable(GL_STENCIL_TEST);
            qDebug() << "glEnable(GL_STENCIL_TEST)";
            f_->glStencilFunc(GL_EQUAL, state_->stencilValue(), 0xFF);
            qDebug() << "glStencilFunc(GL_EQUAL " << state_->stencilValue() << " 0xFF)";
            f_->glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
            qDebug() << "glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP)";
        }

        //f_->glEnable(GL_CULL_FACE);
        //f_->glCullFace(GL_FRONT_AND_BACK);
        f_->glFrontFace(GL_CCW);
        qDebug() << "glFrontFace(GL_CCW)";

        m_drawingData->bindStepTexture(i, f_);
        m_drawingData->drawStep(i, f_);

        m_drawingData->releaseStep(i);
    }
}

void OpenGlQuizImageDemo::noPaintGL(QOpenGLFunctions *f_, const RenderState *state_)
{
    Q_UNUSED(f_);
    Q_UNUSED(state_);

/*    f_->glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

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
    //f_->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ACCUM_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
*/
}

static const int g_trianglesCount = 2;
static const int g_triangleConers = 3;

void makeGeometry(
        int width_, int height_,
        int geomertyPointsWidth_, int geometryPointsHeight_,
        float facedGeometryXCoef_, float facedGeometryYCoef_,
        int geometryVertexCoords_, int geometryTextureCoords_,
        bool hasTextureCoords_,
        bool isGeometrySolid_,
        QVector<GLfloat> &vertexData_,
        QVector<GLfloat> &textureData_,
        QVector<GLint> &indexesData_
        )
{
    static const int coords[g_trianglesCount][g_triangleConers][2] =
    {
        {{ 1, 0 }, { 0, 0 }, { 0, 1 }},
        {{ 1, 0 }, { 0, 1 }, { 1, 1 }}
    };

    vertexData_.resize(geomertyPointsWidth_ * geometryPointsHeight_ *
                      g_trianglesCount * g_triangleConers * geometryVertexCoords_);
    textureData_.resize(geomertyPointsWidth_ * geometryPointsHeight_ *
                        g_trianglesCount * g_triangleConers * geometryTextureCoords_);
    for(int y = 0; y < geometryPointsHeight_; y++)
    {
        for(int x = 0; x < geomertyPointsWidth_; x++)
        {
            const int offsBase0 = (y * geomertyPointsWidth_ + x) *
                    g_trianglesCount * g_triangleConers * geometryVertexCoords_;
            const int offsBase1 = (y * geomertyPointsWidth_ + x) *
                    g_trianglesCount * g_triangleConers * geometryTextureCoords_;
            for (int j = 0; j < g_trianglesCount; ++j)
            {
                for(int k = 0; k < g_triangleConers; k++)
                {
                    // vertex position
                    const int offs0 = offsBase0 + (j * g_triangleConers + k) * geometryVertexCoords_;
                    const int offs1 = offsBase1 + (j * g_triangleConers + k) * geometryTextureCoords_;
                    if(geometryVertexCoords_ >= 2)
                    {
                        if(isGeometrySolid_)
                        {
                                vertexData_[offs0 + 0] = (x + coords[j][k][0]) * width_ / (GLfloat)geomertyPointsWidth_;
                                vertexData_[offs0 + 1] = (y + coords[j][k][1]) * height_ / (GLfloat)geometryPointsHeight_;
                        }
                        else
                        {
                            vertexData_[offs0 + 0] = (x + coords[j][k][0]) * width_ / (GLfloat)geomertyPointsWidth_
                                    - (coords[j][k][0] * 2 - 1) * facedGeometryXCoef_
                                    ;
                            vertexData_[offs0 + 1] = (y + coords[j][k][1]) * height_ / (GLfloat)geometryPointsHeight_
                                    - (coords[j][k][1] * 2 - 1) * facedGeometryYCoef_
                                    ;
                        }
                    }
                    if(geometryVertexCoords_ >= 3)
                    {
                        vertexData_[offs0 + 2] = 0.1;
                    }
                    if(geometryVertexCoords_ >= 4)
                    {
                        vertexData_[offs0 + 3] = 1.0;
                    }

                    // texture coordinate
                    if(hasTextureCoords_)
                    {
                        textureData_[offs1 + 0] = (GLfloat)(x + coords[j][k][0])/(GLfloat)geomertyPointsWidth_;
                        textureData_[offs1 + 1] = 1.0 - (GLfloat)(y + coords[j][k][1])/(GLfloat)geometryPointsHeight_;
                        if(geometryTextureCoords_ >= 3)
                        {
                            textureData_[offs1 + 2] = 0.0;
                        }
                        if(geometryTextureCoords_ >= 4)
                        {
                            textureData_[offs1 + 3] = 1.0;
                        }
                    }
                }
            }
        }
    }
    indexesData_.resize(1);
    if(isGeometrySolid_)
    {
        indexesData_[0] = 1;
    }
    else
    {
        indexesData_[0] = geomertyPointsWidth_ * geometryPointsHeight_;
    }
}

void calculatePreserveAspectFitTextureMatrix(
        QMatrix4x4 & textureMatrix_,
        const QSize &imageSize_,
        const QSize &rectSize_
        )
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

void OpenGlQuizImageDemo::updateRenderArguments(bool minimal_)
{
    if(nullptr == m_drawingData) { return; }

    const int screeRectTupleSize = m_drawingData->getTupleSize(
                g_renderScreenRectName
                );
    m_drawingData->setRenderArgumentValue(
                g_renderScreenRectName,
                screeRectTupleSize <= 2
                    ? QVector<GLfloat>({GLfloat(m_width), GLfloat(m_height)})
                    : QVector<GLfloat>({GLfloat(m_left), GLfloat(m_top), GLfloat(m_width), GLfloat(m_height)})
                    ,
                -1
                );
    m_drawingData->setRenderArgumentValue(
                g_renderTName,
                QVector<GLfloat>({GLfloat(m_t),}),
                -1
                );

    QSize rectSize(m_width, m_height);
    bool sizeIsChanged = m_oldRectSize.width() != rectSize.width() ||
            m_oldRectSize.height() != rectSize.height();
    m_oldRectSize = rectSize;

    if(sizeIsChanged || !m_drawingData->isArgumentInitialized(g_renderFromImageMatrixName))
    {
        QSize imageSize;
        if(m_drawingData->getTextureSize(g_renderFromImageName, imageSize))
        {
            QMatrix4x4 imageTextureMatrix;
            calculatePreserveAspectFitTextureMatrix(imageTextureMatrix, imageSize, rectSize);
            QVector<GLfloat> mdata = matrixToVector(imageTextureMatrix);
            m_drawingData->setRenderArgumentValue(
                        g_renderFromImageMatrixName,
                        mdata,
                        mdata.size()
                        );
        }
    }
    if(sizeIsChanged || !m_drawingData->isArgumentInitialized(g_renderToImageMatrixName))
    {
        QSize imageSize;
        if(m_drawingData->getTextureSize(g_renderToImageName, imageSize))
        {
            QMatrix4x4 imageTextureMatrix;
            calculatePreserveAspectFitTextureMatrix(imageTextureMatrix, imageSize, rectSize);
            QVector<GLfloat> mdata = matrixToVector(imageTextureMatrix);
            m_drawingData->setRenderArgumentValue(
                        g_renderToImageMatrixName,
                        mdata,
                        mdata.size()
                        );
        }
    }

    if(minimal_ && !sizeIsChanged) { return; }

    const int vertexTupleSize = m_drawingData->getTupleSize(
                g_renderVertexAttributeName
                );
    const int textureTupleSize = m_drawingData->getTupleSize(
                g_renderTextureAttributeName
                );
    if(sizeIsChanged ||
            !m_drawingData->isArgumentInitialized(g_renderVertexAttributeName) ||
            (!m_drawingData->isArgumentInitialized(g_renderTextureAttributeName) &&
             textureTupleSize > 0
             )
            )
    {
        QVector<GLfloat> facedGeometryCoefs({1e-3f, 1e-3f});
        m_drawingData->getArgumentValue(
                    g_renderFacedGeometryCoefsName,
                    facedGeometryCoefs
                    );
        QVector<GLint> geomertySize({1, 1});
        m_drawingData->getArgumentValue(
                    g_renderGeomertySizeName,
                    geomertySize
                    );
        QVector<GLint> geomertySolid({1, });
        m_drawingData->getArgumentValue(
                    g_renderIsGeomertySolidName,
                    geomertySolid
                    );
        QVector<GLfloat> vertexData;
        QVector<GLfloat> textureData;
        QVector<GLint> indexesData;
        makeGeometry(
                    m_width, m_height,
                    geomertySize[0], geomertySize[1],
                    facedGeometryCoefs[0], facedGeometryCoefs[1],
                    vertexTupleSize, textureTupleSize,
                    textureTupleSize > 0, geomertySolid[0] > 0,
                    vertexData, textureData, indexesData
                    );
        m_drawingData->setRenderArgumentValue(
                    g_renderVertexAttributeName,
                    vertexData,
                    vertexData.size()
                    );
        if(textureTupleSize > 0)
        {
            m_drawingData->setRenderArgumentValue(
                        g_renderTextureAttributeName,
                        textureData,
                        textureData.size()
                        );
        }
        m_drawingData->setRenderArgumentValue(
                    g_renderIndexesName,
                    indexesData,
                    indexesData.size()
                    );
    }
}
