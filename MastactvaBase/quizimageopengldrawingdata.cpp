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

    m_texture = std::make_unique<QOpenGLTexture>(m_image.mirrored());
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
    if(!program_
            || !isValidLocation())
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

bool opengl_drawing::Texture::isValidLocation() const
{
    return m_location >= 0;
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
    program = std::make_unique<QOpenGLShaderProgram>();
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

        std::unique_ptr<Texture> texture = std::make_unique<Texture>();
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
    if(!isUsable())
    {
        return;
    }

    program->bind();
}

void opengl_drawing::Object::setUniforms()
{
    if(!isUsable())
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
        if(!isIdValid(uniformId))
        {
            continue;
        }

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
    if(!isUsable())
    {
        return;
    }

    for(const auto &attribute : attributes)
    {
        if(!isIdValid(attribute))
        {
            continue;
        }

        program->enableAttributeArray(attribute);
    }
}

void opengl_drawing::Object::disableAttributes()
{
    if(!isUsable())
    {
        return;
    }

    for(const auto &attribute : attributes)
    {
        if(!isIdValid(attribute))
        {
            continue;
        }

        program->disableAttributeArray(attribute);
    }
}

void opengl_drawing::Object::setAttributeArray()
{
    if(!isUsable())
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
        if(!isIdValid(attributeId))
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
                || textures.find(texture_.name) == std::cend(textures)
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
    if(!isUsable()
            || m_imageData->attributes.empty()
            || !f_
            )
    {
        return;
    }

    const auto fit = std::min_element(
                std::cbegin(m_imageData->attributes),
                std::cend(m_imageData->attributes),
                [](
                const std::unique_ptr<drawing_data::IAttribute> &left_,
                const std::unique_ptr<drawing_data::IAttribute> &right_)->bool
    {
       return left_.operator bool()
               && right_.operator bool()
               && left_->size() < right_->size()
               ;
    });
    if(std::cend(m_imageData->attributes) == fit)
    {
        return;
    }

    f_->glDrawArrays(GL_TRIANGLES, 0, (*fit)->size());
}

void opengl_drawing::Object::release()
{
    if(!isUsable())
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

    if(newFilename_ == fit->filename)
    {
        return;
    }

    std::unique_ptr<Texture> texture = std::make_unique<Texture>();
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
    if(fit == std::cend(textures)
            || !fit->second.operator bool()
            )
    {
        return false;
    }

    return fit->second->getSize(imageSize_);
}

bool opengl_drawing::Object::isUsable() const
{
    return program.operator bool();
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

bool opengl_drawing::Object::isIdValid(int idValue_)
{
    return idValue_ >= 0;
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


QMatrix4x4 calculatePreserveAspectFitTextureMatrix(
        const QSize &imageSize_,
        const QSize &rectSize_
        )
{
    QMatrix4x4 textureMatrix;
    const qreal imageRate = (qreal)std::max(1, imageSize_.width())
            / (qreal)std::max(1, imageSize_.height())
            ;
    const qreal rectRate = (qreal)std::max(1, rectSize_.width())
            / (qreal)std::max(1, rectSize_.height())
            ;
    if(rectRate >= imageRate)
    {
        textureMatrix.scale(rectRate/imageRate, 1.0);
        textureMatrix.translate(-(rectRate - imageRate)/rectRate*0.5, 0.0);
    }
    else
    {
        textureMatrix.scale(1.0, imageRate/rectRate);
        textureMatrix.translate(0.0, -(imageRate - rectRate)/imageRate*0.5);
    }
    return textureMatrix;
}


class ImageMatrixDefaultCalculation : public opengl_drawing::IEffectCalculation
{
public:
    ImageMatrixDefaultCalculation();
    void calculate(opengl_drawing::IVariables *variables_) const override;

private:
    QMatrix4x4 getImageMatrix(
            opengl_drawing::Objects *objects_,
            const QString &imageName_,
            const QSize &windowSize_
            ) const;
};

ImageMatrixDefaultCalculation::ImageMatrixDefaultCalculation()
{
    setFilename(g_imageMatrixDefaultCalculationName);
    setRequiredVariables({
                 g_renderWindowSizeName,
                 g_renderFromImageName,
                 g_renderToImageName
                });
}

void ImageMatrixDefaultCalculation::calculate(opengl_drawing::IVariables *variables_) const
{
    opengl_drawing::Objects *objects = dynamic_cast<opengl_drawing::Objects *>(variables_);
    if(!objects)
    {
        return;
    }

    const QVector2D windowSizeF = objects->getUniform(g_renderWindowSizeName, QVector2D{1.0,1.0});
    const QSize windowSize((int)windowSizeF.x(), (int)windowSizeF.y());
    objects->setUniform(
                g_renderFromImageMatrixName,
                getImageMatrix(objects, g_renderFromImageName, windowSize)
                );
    objects->setUniform(
                g_renderToImageMatrixName,
                getImageMatrix(objects, g_renderToImageName, windowSize)
                );
}

QMatrix4x4 ImageMatrixDefaultCalculation::getImageMatrix(
        opengl_drawing::Objects *objects_,
        const QString &imageName_,
        const QSize &windowSize_
        ) const
{
    const QSize imageSize = objects_->getTextureSize(imageName_ , windowSize_);
    return calculatePreserveAspectFitTextureMatrix(imageSize, windowSize_);
}


class GeometryDefaultCalculation : public opengl_drawing::IEffectCalculation
{
public:
    GeometryDefaultCalculation();
    void calculate(opengl_drawing::IVariables *variables_) const override;
};

GeometryDefaultCalculation::GeometryDefaultCalculation()
{
    setFilename(g_geometryDefaultCalculationName);
    setRequiredVariables({
                  g_renderScreenRectName,
                  g_renderIsGeomertySolidName,
                  g_renderGeomertySizeName,
                  g_renderFacedGeometryCoefsName,
                  g_renderTextureAttributeName,
                 });
}

void GeometryDefaultCalculation::calculate(opengl_drawing::IVariables *variables_) const
{
    opengl_drawing::Objects *objects = dynamic_cast<opengl_drawing::Objects *>(variables_);
    if(!objects)
    {
        return;
    }

    const int vertexAttributeTupleSize = objects->getAttributeTupleSize(
                g_renderVertexAttributeName
                );
    const bool vertextAttributeExist = vertexAttributeTupleSize > 0;

    if(!vertextAttributeExist)
    {
        return;
    }

    const QVector2D proportinalRect = objects->getUniform(
                g_renderScreenRectName,
                QVector2D{1.0, 1.0}
                );
    const GLint isSolidGeometry = objects->getUniform(
                g_renderIsGeomertySolidName,
                GLint{1}
                );
    const QVector2D geometryFacedSize = objects->getUniform(
                g_renderGeomertySizeName,
                QVector2D{2.0, 2.0}
                );
    const QVector2D geometryFacedInterval = objects->getUniform(
                g_renderFacedGeometryCoefsName,
                QVector2D{0.0, 0.0}
                );
    const int textureAttributeTupleSize = objects->getAttributeTupleSize(
                g_renderTextureAttributeName
                );

    std::vector<GLfloat> vertexData;
    std::vector<GLfloat> textureData;

    const bool textureAttributeExist = textureAttributeTupleSize > 0;
    makeGeometry(proportinalRect.x(), proportinalRect.y(),
                 (int)geometryFacedSize.x(), (int)geometryFacedSize.y(),
                 geometryFacedInterval.x(), geometryFacedInterval.y(),
                 vertexAttributeTupleSize,
                 textureAttributeTupleSize, textureAttributeExist,
                 isSolidGeometry,
                 vertexData, textureData);

    objects->setAttribute(
                g_renderVertexAttributeName,
                vertexData,
                vertexAttributeTupleSize
                );
    if(textureAttributeExist)
    {
        objects->setAttribute(
                    g_renderTextureAttributeName,
                    textureData,
                    textureAttributeTupleSize
                    );
    }
}


ImageMatrixDefaultCalculation g_imageMatrixDefaultCalculation;
GeometryDefaultCalculation g_geometryDefaultCalculation;

void opengl_drawing::Objects::init(
        std::unique_ptr<drawing_data::QuizImageObjects> &&imageData_
        )
{
    m_imageData = std::move(imageData_);
    m_objects.reserve(m_imageData->objects.size());
    for(const auto &object : m_imageData->objects)
    {
        m_objects.push_back(
                    std::make_unique<opengl_drawing::Object>()
                    );
        m_objects.back()->init(object);
    }

    auto doExtend = [](
            drawing_data::QuizImageObjects *objects_,
            opengl_drawing::IEffectCalculation *newCalc_
            )->bool
    {
        for(const auto &calc_ : objects_->calculations)
        {
            if(!calc_.operator bool())
            {
                continue;
            }
            if(calc_->doExtend(newCalc_))
            {
                return true;
            }
        }
        return false;
    };

    m_imageMatrixDefault = !doExtend(m_imageData.get(), &g_imageMatrixDefaultCalculation)
            ? &g_imageMatrixDefaultCalculation
            : nullptr
              ;
    m_geometryDefault = !doExtend(m_imageData.get(), &g_geometryDefaultCalculation)
            ? &g_geometryDefaultCalculation
            : nullptr
              ;
}

void opengl_drawing::Objects::calculate()
{
    if(!m_imageData.operator bool())
    {
        return;
    }

    if(m_geometryDefault
            && isUpdated(m_geometryDefault->getRequiredVariables(), nullptr)
            )
    {
        m_geometryDefault->calculate(this);
    }
    if(m_imageMatrixDefault
            && isUpdated(m_imageMatrixDefault->getRequiredVariables(), nullptr)
            )
    {
        m_imageMatrixDefault->calculate(this);
    }
    m_imageData->calculate(this);
    clearUpdated();
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
        return QColor{255, 255, 255};
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

QSize opengl_drawing::Objects::getTextureSize(const QString &name_, const QSize &size_) const
{
    QSize result;
    if(getTextureSize(name_, result))
    {
        return result;
    }
    else
    {
        return size_;
    }
}

void opengl_drawing::Objects::setTexture(
        const QString &name_,
        const QString &newFilename_
        )
{
    if(!m_updated.contains(name_))
    {
        m_updated.push_back(name_);
    }
    for(std::unique_ptr<Object> &object_ : m_objects)
    {
        object_->setTexture(name_, newFilename_);
    }
}

void opengl_drawing::Objects::setFromImage(const QString &url_)
{
    setTexture(g_renderFromImageName, url_);    // TODO: dependency inversion
}

void opengl_drawing::Objects::setToImage(const QString &url_)
{
    setTexture(g_renderToImageName, url_);  // TODO: dependency inversion
}

QMatrix4x4 opengl_drawing::Objects::getImageMatrix(const QString &imageName_, const QSize &windowSize_) const
{
    const QSize imageSize = getTextureSize(imageName_ , windowSize_);
    return calculatePreserveAspectFitTextureMatrix(imageSize, windowSize_);
}

bool opengl_drawing::Objects::isUpdated(const QSet<QString> &vars_, IVariables *base_) const
{
    for(const QString &var_ : m_updated)
    {
        if(vars_.contains(var_))
        {
            return true;
        }
    }
    if(base_)
    {
        return base_->isUpdated(vars_, nullptr);
    }
    if(m_imageData.operator bool())
    {
        return m_imageData->isUpdated(vars_, nullptr);
    }
    return false;
}

void opengl_drawing::Objects::clearUpdated()
{
    m_updated.clear();
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
    m_openglData = std::make_unique<opengl_drawing::Objects>();
    m_openglData->init(std::move(imageData_));
    initialize();
}

std::unique_ptr<drawing_data::QuizImageObjects> ObjectsRenderer::releaseImageData()
{
    if(!isValidData())
    {
        return {nullptr};
    }

    return m_openglData->free();
}

int ObjectsRenderer::getAttributeTupleSize(const QString &name_) const
{
    if(!isValidData())
    {
        return 0;
    }

    return m_openglData->getAttributeTupleSize(name_);
}

bool ObjectsRenderer::getTextureSize(const QString &name_, QSize &size_) const
{
    if(!isValidData())
    {
        return false;
    }

    return m_openglData->getTextureSize(name_, size_);
}

QSize ObjectsRenderer::getTextureSize(const QString &name_, const QSize &size_) const
{
    QSize result;
    if(getTextureSize(name_, result))
    {
        return result;
    }
    else
    {
        return size_;
    }
}

void ObjectsRenderer::updateVariables(
        const QVector2D &rectSize_,
        qreal t_,
        const QVector2D &windowSize_)
{
    const float maxCxCy = std::max(std::max(rectSize_.x(), rectSize_.y()), 1.0f);
    const QVector2D proportinalRect(rectSize_.x() / maxCxCy, rectSize_.y() / maxCxCy);

    setUniformIfExistsAndChanged( g_renderTName, t_ );
    setUniformIfExistsAndChanged( g_renderScreenRectName, proportinalRect );
    setUniformIfExistsAndChanged( g_renderMatrixName, getScreenMatrix(proportinalRect));
    setUniformIfExistsAndChanged( g_renderWindowSizeName, windowSize_);
}

void ObjectsRenderer::initialize()
{
    initializeOpenGLFunctions();
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
}

bool ObjectsRenderer::isValidData() const
{
    return m_openglData.operator bool();
}

QMatrix4x4 ObjectsRenderer::getScreenMatrix(const QVector2D &proportinalRect_)
{
    QMatrix4x4 renderMatrix;
    renderMatrix.ortho(QRectF(0, 0, proportinalRect_.x(), proportinalRect_.y()));
    return renderMatrix;
}

void ObjectsRenderer::render()
{
    if(m_openglData.operator bool())
    {
        m_openglData->calculate();
    }

    glDepthMask(true);

    if(isValidData())
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

    if(isValidData())
    {
        m_openglData->draw(this);
    }

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
}

void ObjectsRenderer::setFromImage(const QString &url_)
{
    if(!isValidData())
    {
        return;
    }

    m_openglData->setFromImage(url_);
}

void ObjectsRenderer::setToImage(const QString &url_)
{
    if(!isValidData())
    {
        return;
    }

    m_openglData->setToImage(url_);
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

void QuizImageFboRendererImpl::synchronizeImpl(
        const QVector2D &rectSize_,
        qreal t_
        )
{
    m_objectRenderer.updateVariables(rectSize_, t_, m_windowSize);
}

std::unique_ptr<drawing_data::QuizImageObjects> QuizImageFboRendererImpl::releaseImageData()
{
    return m_objectRenderer.releaseImageData();
}

void QuizImageFboRendererImpl::setImageData(std::unique_ptr<drawing_data::QuizImageObjects> imageData_)
{
    m_objectRenderer.setImageData(std::move(imageData_));
}

void QuizImageFboRendererImpl::setFromImage(const QString &url_)
{
    m_objectRenderer.setFromImage(url_);
}

void QuizImageFboRendererImpl::setToImage(const QString &url_)
{
    m_objectRenderer.setToImage(url_);
}


void drawing_data::DefaultQuizImageObject::initialize(
        QuizImageObjects &data_
        ) const
{
    std::unique_ptr<QuizImageObject> object = std::make_unique<QuizImageObject>();

    static QByteArray vertex = loadTextFile( g_defaultVertexShaderName ).toUtf8();
    static QByteArray fragment = loadTextFile( g_defaultFragmentShaderName ).toUtf8();
    object->vertexShader = vertex.constData();
    object->fragmentShader = fragment.constData();

    object->textures = {
        { g_renderFromImageName, g_noImage001Name },
        { g_renderToImageName, g_noImage002Name },
    };

    object->attributes.push_back(
                std::make_unique<drawing_data::Attribute<QVector4D>>(
                    g_renderVertexAttributeName,
                    std::make_shared<std::vector<QVector4D>>()
                    )
                );
    object->attributes.push_back(
                std::make_unique<drawing_data::Attribute<QVector4D>>(
                    g_renderTextureAttributeName,
                    std::make_shared<std::vector<QVector4D>>()
                    )
                );

    object->uniforms.push_back(
                std::make_unique<drawing_data::Uniform<QMatrix4x4>>(
                    g_renderMatrixName,
                    std::make_shared<QMatrix4x4>()
                    )
                );
    object->uniforms.push_back(
                std::make_unique<drawing_data::Uniform<QMatrix4x4>>(
                    g_renderFromImageMatrixName,
                    std::make_shared<QMatrix4x4>()
                    )
                );
    object->uniforms.push_back(
                std::make_unique<drawing_data::Uniform<QMatrix4x4>>(
                    g_renderToImageMatrixName,
                    std::make_shared<QMatrix4x4>()
                    )
                );
    object->uniforms.push_back(
                std::make_unique<drawing_data::Uniform<QVector2D>>(
                    g_renderScreenRectName,
                    std::make_shared<QVector2D>()
                    )
                );
    object->uniforms.push_back(
                std::make_unique<drawing_data::Uniform<QVector2D>>(
                    g_renderWindowSizeName,
                    std::make_shared<QVector2D>()
                    )
                );

    object->uniforms.push_back(
                std::make_unique<drawing_data::Uniform<GLfloat>>(
                    g_renderTName,
                    std::make_shared<GLfloat>(0.5)
                    )
                );

    object->uniforms.push_back(
                std::make_unique<drawing_data::Uniform<GLfloat>>(
                    g_renderOpacityName,
                    std::make_shared<GLfloat>(1.0)
                    )
                );

    object->uniforms.push_back(
                std::make_unique<drawing_data::Uniform<QVector2D>>(
                    g_renderFacedGeometryCoefsName,
                    std::make_shared<QVector2D>(0.0, 0.0)
                    )
                );

    object->uniforms.push_back(
                std::make_unique<drawing_data::Uniform<QVector2D>>(
                    g_renderGeomertySizeName,
                    std::make_shared<QVector2D>(2.0, 2.0)
                    )
                );

    object->uniforms.push_back(
                std::make_unique<drawing_data::Uniform<GLint>>(
                    g_renderIsGeomertySolidName,
                    std::make_shared<GLint>(1)
                    )
                );

    data_.objects.push_back(std::move(object));
}
