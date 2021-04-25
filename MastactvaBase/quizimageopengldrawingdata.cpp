#include "quizimageopengldrawingdata.h"
#include "../MastactvaBase/names.h"
#include "../MastactvaBase/defines.h"


bool opengl_drawing::Texture::setFilename(const QString &fileName_)
{
    m_texture.reset();
    m_image.load(fileName_);
    if(m_image.isNull())
    {
        return false;
    }

    m_texture.reset(new QOpenGLTexture(m_image.mirrored()));
    m_texture->setMagnificationFilter(QOpenGLTexture::Filter::Linear);
    m_texture->setWrapMode(QOpenGLTexture::WrapMode::ClampToBorder);
    m_texture->setBorderColor(1, 1, 1, 0);
    return true;
}

void opengl_drawing::Texture::setLocation(int location_)
{
    m_location = location_;
}

void opengl_drawing::Texture::setIndex(int index_)
{
    m_index = index_;
}

void opengl_drawing::Texture::setUniform(QOpenGLShaderProgram *program_) const
{
    if(nullptr == program_
            || m_location < 0)
    {
        return;
    }

    program_->setUniformValue(m_location, m_index);
}

void opengl_drawing::Texture::bind(QOpenGLFunctions *f_) const
{
    f_->glActiveTexture(GL_TEXTURE0 + m_index);
    m_texture->bind();
}

bool opengl_drawing::Texture::getSize(QSize &size_) const
{
    if(m_image.isNull())
    {
        return false;
    }

    size_ = m_image.size();
    return true;
}


void opengl_drawing::Object::free()
{
    program.reset();
    m_imageData.reset();
}

void opengl_drawing::Object::init(
        const std::shared_ptr<drawing_data::QuizImageObject> &imageData_
        )
{
    free();

    m_imageData = imageData_;
    program.reset(new QOpenGLShaderProgram);
    program->addCacheableShaderFromSourceCode(
                QOpenGLShader::Vertex,
                m_imageData->vertexShader.constData()
                );
    program->addCacheableShaderFromSourceCode(
                QOpenGLShader::Fragment,
                m_imageData->fragmentShader.constData()
                );
    program->link();

    for(const auto &attribute : m_imageData->attributes)
    {
        if(!attribute.operator bool())
        {
            continue;
        }

        attributes[attribute->name()] = program->attributeLocation(
                    attribute->name()
                    );
    }

    for(const auto &uniform : m_imageData->uniforms)
    {
        if(!uniform.operator bool())
        {
            continue;
        }

        uniforms[uniform->name()] = program->uniformLocation(
                    uniform->name()
                    );
    }

    for(const drawing_data::Texture &texture_ : m_imageData->textures)
    {
        if(texture_.name.trimmed().isEmpty())
        {
            continue;
        }

        std::unique_ptr<Texture> texture(new Texture());
        if(!texture->setFilename(texture_.filename))
        {
            continue;
        }

        texture->setLocation(program->uniformLocation(
                                 texture_.name)
                             );
        textures[texture_.name] = std::move(texture);
    }
    setTextureIndexes();
}

void opengl_drawing::Object::bind()
{
    if(!program.operator bool())
    {
        return;
    }

    program->bind();
}

void opengl_drawing::Object::setUniforms()
{
    if(!program.operator bool())
    {
        return;
    }

    for(const auto &uniform : m_imageData->uniforms)
    {
        if(!uniform.operator bool())
        {
            continue;
        }

        const auto uniformId = uniforms[uniform->name()];
        if(uniformId < 0) { continue; }
        uniform->set(program.get(), uniformId);
    }
    for(const drawing_data::Texture &texture_ : m_imageData->textures)
    {
        if(texture_.name.trimmed().isEmpty()
                || textures.find(texture_.name) == std::end(textures)
                || !textures[texture_.name].operator bool()
                )
        {
            continue;
        }

        textures[texture_.name]->setUniform(program.get());
    }
}

void opengl_drawing::Object::enableAttributes()
{
    if(!program.operator bool())
    {
        return;
    }

    for(const auto &attribute : attributes)
    {
        if(attribute < 0)
        {
            continue;
        }

        program->enableAttributeArray(attribute);
    }
}

void opengl_drawing::Object::disableAttributes()
{
    if(!program.operator bool())
    {
        return;
    }

    for(const auto &attribute : attributes)
    {
        if(attribute < 0)
        {
            continue;
        }

        program->disableAttributeArray(attribute);
    }
}

void opengl_drawing::Object::setAttributeArray()
{
    if(!program.operator bool())
    {
        return;
    }

    for(const auto &attribute : m_imageData->attributes)
    {
        if(!attribute.operator bool())
        {
            continue;
        }

        const auto attributeId = attributes[attribute->name()];
        if(attributeId < 0)
        {
            continue;
        }

        program->setAttributeArray(
                    attributeId,
                    attribute->constData(),
                    attribute->tupleSize()
                    );
    }
}

void opengl_drawing::Object::bindTextures(QOpenGLFunctions *f_)
{
    for(const drawing_data::Texture &texture_ : m_imageData->textures)
    {
        if(texture_.name.trimmed().isEmpty()
                || textures.find(texture_.name) == std::end(textures)
                || !textures[texture_.name].operator bool()
                )
        {
            continue;
        }

        textures[texture_.name]->bind(f_);
    }
}

void opengl_drawing::Object::drawTriangles(QOpenGLFunctions *f_)
{
    if(!program.operator bool()
            || m_imageData->attributes.empty()
            || nullptr == f_
            )
    {
        return;
    }

    const auto fit = std::min_element(
                std::begin(m_imageData->attributes),
                std::end(m_imageData->attributes),
                [](
                const std::unique_ptr<drawing_data::IAttribute> &left_,
                const std::unique_ptr<drawing_data::IAttribute> &right_)->bool
    {
       return left_.operator bool()
               && right_.operator bool()
               && left_->size() < right_->size()
               ;
    });
    if(std::end(m_imageData->attributes) == fit)
    {
        return;
    }

    f_->glDrawArrays(GL_TRIANGLES, 0, (*fit)->size());
}

void opengl_drawing::Object::release()
{
    if(!program.operator bool())
    {
        return;
    }

    program->release();
}

void opengl_drawing::Object::setTexture(
        const QString &name_,
        const QString& newFilename_
        )
{
    auto fit = std::find_if(
                std::begin(m_imageData->textures),
                std::end(m_imageData->textures),
                [&name_](const drawing_data::Texture &texture_)->bool
    {
        return name_ == texture_.name;
    });

    if(textures.find(name_) == std::end(textures)
            && textures[name_].operator bool()
            )
    {
        textures[name_]->setFilename(newFilename_);
        fit->filename = newFilename_;
        return;
    }

    std::unique_ptr<Texture> texture(new Texture());
    if(!texture->setFilename(newFilename_))
    {
        return;
    }

    if(std::end(m_imageData->textures) == fit)
    {
        m_imageData->textures.push_back({name_, newFilename_});
    }
    else
    {
        fit->filename = newFilename_;
    }
    textures[name_] = std::move(texture);

    setTextureIndexes();
}

bool opengl_drawing::Object::getTextureSize(
        const QString &name_,
        QSize &imageSize_
        ) const
{
    const auto fit = textures.find(name_);
    if(fit == std::end(textures)
            || !fit->second.operator bool()
            )
    {
        return false;
    }

    return fit->second->getSize(imageSize_);
}

void opengl_drawing::Object::setTextureIndexes()
{
    int textureIndex = 0;
    for(const drawing_data::Texture &texture_ : m_imageData->textures)
    {
        if(texture_.name.trimmed().isEmpty()
                || textures.find(texture_.name) == std::end(textures)
                || !textures[texture_.name].operator bool()
                )
        {
            continue;
        }

        ++textureIndex;
        textures[texture_.name]->setIndex(textures.size() - textureIndex);
    }
}


std::unique_ptr<drawing_data::QuizImageObjects> opengl_drawing::Objects::free()
{
    for(std::unique_ptr<opengl_drawing::Object> &object_ : m_objects)
    {
        if(!object_.operator bool())
        {
            continue;
        }

        object_->free();
    }
    m_objects.clear();
    return std::move(m_imageData);
}

void opengl_drawing::Objects::init(
        std::unique_ptr<drawing_data::QuizImageObjects> &&imageData_
        )
{
    m_imageData = std::move(imageData_);
    m_objects.reserve(m_imageData->objects.size());
    for(const auto &object : m_imageData->objects)
    {
        m_objects.push_back(
                    std::unique_ptr<opengl_drawing::Object>(
                        new opengl_drawing::Object()
                        )
                    );
        m_objects.back()->init(object);
    }
}

void opengl_drawing::Objects::draw(QOpenGLFunctions *f_)
{
    for(std::unique_ptr<opengl_drawing::Object> &object_ : m_objects)
    {
        if(!object_.operator bool())
        {
            continue;
        }

        object_->bind();
        object_->setUniforms();
        object_->enableAttributes();
        object_->setAttributeArray();
        object_->bindTextures(f_);
        object_->drawTriangles(f_);
        object_->disableAttributes();
        object_->release();
    }
}

QColor opengl_drawing::Objects::getClearColor() const
{
    if(!m_imageData.operator bool())
    {
        return QColor(255, 255, 255);
    }
    else
    {
        return m_imageData->clearColor;
    }
}

int opengl_drawing::Objects::getAttributeTupleSize(
        const QString &name_
        ) const
{
    if(!m_imageData.operator bool())
    {
        return 0;
    }

    return m_imageData->getAttributeTupleSize(name_);
}

bool opengl_drawing::Objects::getTextureSize(
        const QString &name_,
        QSize &size_
        ) const
{
    for(const std::unique_ptr<Object> &object_ : m_objects)
    {
        if(object_->getTextureSize(name_, size_))
        {
            return true;
        }
    }
    return false;
}

void opengl_drawing::Objects::setTexture(
        const QString &name_,
        const QString &newFilename_
        )
{
    for(std::unique_ptr<Object> &object_ : m_objects)
    {
        object_->setTexture(name_, newFilename_);
    }
}


ObjectsRenderer::ObjectsRenderer()
{
}

ObjectsRenderer::~ObjectsRenderer()
{
}

void ObjectsRenderer::setImageData(
        std::unique_ptr<drawing_data::QuizImageObjects> imageData_
        )
{
    m_openglData.reset(new opengl_drawing::Objects());
    m_openglData->init(std::move(imageData_));
    initialize();
}

std::unique_ptr<drawing_data::QuizImageObjects> ObjectsRenderer::releaseImageData()
{
    if(!m_openglData.operator bool())
    {
        return {nullptr};
    }

    return m_openglData->free();
}

int ObjectsRenderer::getAttributeTupleSize(const QString &name_) const
{
    if(!m_openglData.operator bool())
    {
        return 0;
    }

    return m_openglData->getAttributeTupleSize(name_);
}

bool ObjectsRenderer::getTextureSize(const QString &name_, QSize &size_) const
{
    if(!m_openglData.operator bool())
    {
        return false;
    }

    return m_openglData->getTextureSize(name_, size_);
}

void ObjectsRenderer::initialize()
{
    initializeOpenGLFunctions();
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
}

void ObjectsRenderer::render()
{
    glDepthMask(true);

    if(m_openglData.operator bool())
    {
        glClearColor(
                    m_openglData->getClearColor().redF(),
                    m_openglData->getClearColor().greenF(),
                    m_openglData->getClearColor().blueF(),
                    1.0f
                    );
    }
    else
    {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    glFrontFace(GL_CW);
    glCullFace(GL_FRONT);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    if(m_openglData.operator bool())
    {
        m_openglData->draw(this);
    }

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
}


static const int g_trianglesCount = 2;
static const int g_triangleConers = 3;


void makeGeometry(
        float width_, float height_,
        int geomertyPointsWidth_, int geometryPointsHeight_,
        float facedGeometryXCoef_, float facedGeometryYCoef_,
        int geometryVertexCoords_, int geometryTextureCoords_,
        bool hasTextureCoords_,
        bool isGeometrySolid_,
        std::vector<GLfloat> &vertexData_,
        std::vector<GLfloat> &textureData_
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
                                vertexData_[offs0 + 0] =
                                        (x + coords[j][k][0]) * width_ / (GLfloat)geomertyPointsWidth_;
                                vertexData_[offs0 + 1] =
                                        (y + coords[j][k][1]) * height_ / (GLfloat)geometryPointsHeight_;
                        }
                        else
                        {
                            vertexData_[offs0 + 0] =
                                    (x + coords[j][k][0]) * width_ / (GLfloat)geomertyPointsWidth_
                                    - (coords[j][k][0] * 2 - 1) * facedGeometryXCoef_
                                    ;
                            vertexData_[offs0 + 1] =
                                    (y + coords[j][k][1]) * height_ / (GLfloat)geometryPointsHeight_
                                    - (coords[j][k][1] * 2 - 1) * facedGeometryYCoef_
                                    ;
                        }
                    }
                    if(geometryVertexCoords_ >= 3)
                    {
                        vertexData_[offs0 + 2] = -0.1;
                    }
                    if(geometryVertexCoords_ >= 4)
                    {
                        vertexData_[offs0 + 3] = 1.0;
                    }

                    // texture coordinate
                    if(hasTextureCoords_)
                    {
                        textureData_[offs1 + 0] =
                                (GLfloat)(x + coords[j][k][0])/(GLfloat)geomertyPointsWidth_;
                        textureData_[offs1 + 1] =
                                (GLfloat)(y + coords[j][k][1])/(GLfloat)geometryPointsHeight_;
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
}

void calculatePreserveAspectFitTextureMatrix(
        QMatrix4x4 & textureMatrix_,
        const QSize &imageSize_,
        const QSize &rectSize_
        )
{
    const qreal imageRate = (qreal)std::max(1, imageSize_.width())
            / (qreal)std::max(1, imageSize_.height())
            ;
    const qreal rectRate = (qreal)std::max(1, rectSize_.width())
            / (qreal)std::max(1, rectSize_.height())
            ;
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


void QuizImageFboRendererImpl::renderImpl()
{
    m_objectRenderer.render();
}

QOpenGLFramebufferObject *QuizImageFboRendererImpl::createFramebufferObjectImpl(const QSize &size_)
{
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(4);
    return new QOpenGLFramebufferObject(size_, format);
}

const QVector2D &QuizImageFboRendererImpl::getWindowSize() const
{
    return m_windowSize;
}

void QuizImageFboRendererImpl::setWindowSize(const QVector2D &windowSize_)
{
    m_windowSize = windowSize_;
}

void QuizImageFboRendererImpl::synchronizeImpl(const QVector2D &rectSize_, bool imageDataChanged_, bool sizeChanged_, qreal t_)
{
    const float sm = std::max(std::max(rectSize_.x(), rectSize_.y()), 1.0f);
    QVector2D rect(rectSize_.x() / sm, rectSize_.y() / sm);

    m_objectRenderer.setUniform(g_renderScreenRectName, rect);
    m_objectRenderer.setUniform(g_renderTName, t_);
    QMatrix4x4 renderMatrix;
    renderMatrix.ortho(QRectF(0, 0, rect.x(), rect.y()));
    m_objectRenderer.setUniform(g_renderMatrixName, renderMatrix);

    if(!(imageDataChanged_ || sizeChanged_))
    {
        return;
    }

    GLint isSolid = 1;
    m_objectRenderer.getUniform(g_renderIsGeomertySolidName, isSolid);
    QVector2D geometrySize(1.0, 1.0);
    m_objectRenderer.getUniform(g_renderGeomertySizeName, geometrySize);
    QVector2D geometryFacedCoef(0.0, 0.0);
    m_objectRenderer.getUniform(g_renderFacedGeometryCoefsName, geometryFacedCoef);

    const int vertexAttributeTupleSize = m_objectRenderer.getAttributeTupleSize(g_renderVertexAttributeName);
    const int textureAttributeTupleSize = m_objectRenderer.getAttributeTupleSize(g_renderTextureAttributeName);
    if(vertexAttributeTupleSize <= 0)
    {
        return;
    }

    std::vector<GLfloat> vertexData;
    std::vector<GLfloat> textureData;

    makeGeometry(rect.x(), rect.y(),
                 (int)geometrySize.x(), (int)geometrySize.y(),
                 geometryFacedCoef.x(), geometryFacedCoef.y(),
                 vertexAttributeTupleSize,
                 textureAttributeTupleSize, textureAttributeTupleSize > 0,
                 0 != isSolid,
                 vertexData, textureData);

    m_objectRenderer.setAttribute(g_renderVertexAttributeName, vertexData, vertexAttributeTupleSize);
    if(textureAttributeTupleSize > 0)
    {
        m_objectRenderer.setAttribute(g_renderTextureAttributeName, textureData, textureAttributeTupleSize);
    }

    QSize windowSize((int)m_windowSize.x(), (int)m_windowSize.y());
    QSize imageSize;
    if(m_objectRenderer.getTextureSize(g_renderFromImageName, imageSize))
    {
        QMatrix4x4 imageTextureMatrix;
        calculatePreserveAspectFitTextureMatrix(imageTextureMatrix, imageSize, windowSize);
        m_objectRenderer.setUniform(g_renderFromImageMatrixName, imageTextureMatrix);
    }
    if(m_objectRenderer.getTextureSize(g_renderToImageName, imageSize))
    {
        QMatrix4x4 imageTextureMatrix;
        calculatePreserveAspectFitTextureMatrix(imageTextureMatrix, imageSize, windowSize);
        m_objectRenderer.setUniform(g_renderToImageMatrixName, imageTextureMatrix);
    }
}

std::unique_ptr<drawing_data::QuizImageObjects> QuizImageFboRendererImpl::releaseImageData()
{
    return m_objectRenderer.releaseImageData();
}

void QuizImageFboRendererImpl::setImageData(std::unique_ptr<drawing_data::QuizImageObjects> imageData_)
{
    m_objectRenderer.setImageData(std::move(imageData_));
}


void drawing_data::DefaultQuizImageObject::initialize(
        QuizImageObjects &data_
        ) const
{
    std::unique_ptr<QuizImageObject> object(new QuizImageObject());

    static QByteArray vertex = loadTextFile(":/Shaders/Shaders/test000/default.vsh").toUtf8(); // TODO: fix resources
    static QByteArray fragment = loadTextFile(":/Shaders/Shaders/test000/default.fsh").toUtf8(); // TODO: fix resources
    object->vertexShader = vertex.constData();
    object->fragmentShader = fragment.constData();

    object->textures = {
        {g_renderFromImageName, ":/Images/Images/test001/0001.png"}, // TODO: fix resources
        {g_renderToImageName, ":/Images/Images/test001/0002.png"}, // TODO: fix resources
    };

    std::shared_ptr<std::vector<QVector4D>> vertices(new std::vector<QVector4D>());
    std::shared_ptr<std::vector<QVector4D>> textures(new std::vector<QVector4D>());

    object->attributes.push_back(
                std::unique_ptr<drawing_data::IAttribute>(
                    new drawing_data::Attribute<QVector4D>{ g_renderVertexAttributeName, vertices }
                    )
                );
    object->attributes.push_back(
                std::unique_ptr<drawing_data::IAttribute>(
                    new drawing_data::Attribute<QVector4D>{ g_renderTextureAttributeName, textures }
                    )
                );

    std::shared_ptr<QMatrix4x4> renderMatrix(new QMatrix4x4);
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<QMatrix4x4>{ g_renderMatrixName, renderMatrix }
                ));
    std::shared_ptr<QMatrix4x4> renderFromImageMatrix(new QMatrix4x4);
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<QMatrix4x4>{ g_renderFromImageMatrixName, renderFromImageMatrix }
                ));
    std::shared_ptr<QMatrix4x4> renderToImageMatrix(new QMatrix4x4);
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<QMatrix4x4>{ g_renderToImageMatrixName, renderToImageMatrix }
                ));
    std::shared_ptr<QVector2D> renderScreenRect(new QVector2D());
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<QVector2D>{ g_renderScreenRectName, renderScreenRect }
                ));

    std::shared_ptr<GLfloat> renderT(new GLfloat{0.5});
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<GLfloat>{ g_renderTName, renderT }
                ));

    std::shared_ptr<GLfloat> renderOpacity(new GLfloat{1.0});
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<GLfloat>{ g_renderOpacityName, renderOpacity }
                ));

    std::shared_ptr<QVector2D> renderFacedGeometryCoefs(new QVector2D(0.0, 0.0));
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<QVector2D>{ g_renderFacedGeometryCoefsName, renderFacedGeometryCoefs }
                ));

    std::shared_ptr<QVector2D> renderGeomertySize(new QVector2D(2.0, 2.0));
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<QVector2D>{ g_renderGeomertySizeName, renderGeomertySize }
                ));

    std::shared_ptr<GLint> renderIsGeomertySolid(new GLint{1});
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<GLint>{ g_renderIsGeomertySolidName, renderIsGeomertySolid }
                ));

    data_.objects.push_back(std::move(object));
}