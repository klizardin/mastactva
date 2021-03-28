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

void OpenGlQuizImageDemo::init(QOpenGLFunctions *f_)
{
    if(nullptr == m_drawingData ||
            m_drawingData->isInitialized()
            ) { return; }

#if defined(TRACE_THREADS_QUIZIMAGE)
    qDebug() << "OpenGlQuizImageDemo::init()" << QThread::currentThread() << QThread::currentThreadId();
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

void OpenGlQuizImageDemo::paintGL(QOpenGLFunctions *f_, const RenderState *state_)
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

void OpenGlQuizImageDemo::noPaintGL(QOpenGLFunctions *f_, const RenderState *state_)
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

void OpenGlQuizImageDemo::updateRenderArguments()
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
