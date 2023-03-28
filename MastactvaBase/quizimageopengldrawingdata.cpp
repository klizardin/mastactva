/*
    Copyright 2021

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

#include <math.h>
#include "quizimageopengldrawingdata.h"
#include "../MastactvaBase/iquizimageqwindow.h"
#include "../MastactvaBase/names.h"
#include "../MastactvaBase/drawingdata_utils.h"
#include "../MastactvaBase/utils.h"
#include "../MastactvaBase/defines.h"


bool opengl_drawing::Texture::setFilename(const QString &fileName_, const QColor &backgroundColor_)
{
    m_texture.reset();
    if(fileName_.isEmpty())
    {
        return true;
    }
    m_image.load(fileName_);
    if(m_image.isNull())
    {
        return false;
    }
    createTextureFromImage();
    setWrapClampToBorder();
    setBorderColor(backgroundColor_);
    return true;
}

bool opengl_drawing::Texture::setPipelineTexture(const QString &textureName_, const QColor &backgroundColor_)
{
    m_texture.reset();
    if(textureName_.isEmpty())
    {
        return false;
    }
    // TODO: add implementation
    setBorderColor(backgroundColor_);
    return false;
}

bool opengl_drawing::Texture::setFromFrameBufferObject(QOpenGLFramebufferObject *frameBufferObject_, const QColor &backgroundColor_)
{
    m_texture.reset();
    if(!frameBufferObject_)
    {
        return false;
    }
    //QOpenGLFramebufferObject fb(frameBufferObject_->size());
    //QOpenGLFramebufferObject::blitFramebuffer(&fb, frameBufferObject_);
    //m_image = fb.toImage(true);
    m_image = frameBufferObject_->toImage(true);
    //qDebug() << m_image.save("000-image.PNG");
    createTextureFromImage();
    setWrapClampToBorder();
    setBorderColor(backgroundColor_);
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
    if(!m_texture)
    {
        return;
    }

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

void opengl_drawing::Texture::setWrapClampToBorder()
{
    if(m_texture)
    {
        m_texture->setWrapMode(QOpenGLTexture::WrapMode::ClampToBorder);
    }
}

void opengl_drawing::Texture::setWrapClampToEdge()
{
    if(m_texture)
    {
        m_texture->setWrapMode(QOpenGLTexture::WrapMode::ClampToEdge);
    }
}

void opengl_drawing::Texture::setBorderColor(const QColor &backgroundColor_)
{
    if(m_texture)
    {
        m_texture->setBorderColor(backgroundColor_);
    }
}

bool opengl_drawing::Texture::isValidLocation() const
{
    return m_location >= 0;
}

void opengl_drawing::Texture::createTextureFromImage()
{
    m_texture = std::make_unique<QOpenGLTexture>(m_image.mirrored());
    m_texture->setMagnificationFilter(QOpenGLTexture::Filter::Linear);
}


class AlphaBlindingDisable : public opengl_drawing::State
{
public:
    bool canProcess(const QString &stateStr_) const override
    {
        return g_alphaBlendingDisable == stateStr_;
    }

    void init(const QString &stateStr_, const std::vector<GLfloat> &) override
    {
        Q_UNUSED(stateStr_);
        glDisable(GL_BLEND);
    }

    void release(const QString &stateStr_, const std::vector<GLfloat> &) override
    {
        Q_UNUSED(stateStr_);
        glDisable(GL_BLEND);
    }
};


class AlphaBlindingDefault : public opengl_drawing::State
{
public:
    bool canProcess(const QString &stateStr_) const override
    {
        return g_alphaBlendingAddSrcAlphaOneMinusSrcAlpha == stateStr_
                || g_alphaBlendingDefault == stateStr_
                ;
    }

    void init(const QString &stateStr_, const std::vector<GLfloat> &) override
    {
        Q_UNUSED(stateStr_);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
        //glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
        glBlendEquation(GL_FUNC_ADD);
    }

    void release(const QString &stateStr_, const std::vector<GLfloat> &) override
    {
        Q_UNUSED(stateStr_);
        glDisable(GL_BLEND);
    }
};


class DepthTestEnable : public opengl_drawing::State
{
public:
    bool canProcess(const QString &stateStr_) const override
    {
        return g_depthTestDefault == stateStr_
                || g_depthTestEnable == stateStr_
                ;
    }

    void init(const QString &stateStr_, const std::vector<GLfloat> &) override
    {
        Q_UNUSED(stateStr_);
        glEnable(GL_DEPTH_TEST);
    }

    void release(const QString &stateStr_, const std::vector<GLfloat> &) override
    {
        Q_UNUSED(stateStr_);
        glDisable(GL_DEPTH_TEST);
    }
};


class DepthTestDisable : public opengl_drawing::State
{
public:
    bool canProcess(const QString &stateStr_) const override
    {
        return g_depthTestDisable == stateStr_;
    }

    void init(const QString &stateStr_, const std::vector<GLfloat> &) override
    {
        Q_UNUSED(stateStr_);
        glDisable(GL_DEPTH_TEST);
    }

    void release(const QString &stateStr_, const std::vector<GLfloat> &) override
    {
        Q_UNUSED(stateStr_);
        glDisable(GL_DEPTH_TEST);
    }
};


class DrawClipRectState : public opengl_drawing::State
{
public:
    bool canProcess(const QString &stateStr_) const override
    {
        return g_renderClipRectStateName == stateStr_;
    }

    void init(const QString &stateStr_, const std::vector<GLfloat> &args_) override
    {
        Q_UNUSED(stateStr_);
        if(args_.size() >= 4)
        {
            glEnable(GL_SCISSOR_TEST);
            glScissor(args_[0], args_[1], args_[2], args_[3]);
        }
    }

    void release(const QString &stateStr_, const std::vector<GLfloat> &) override
    {
        Q_UNUSED(stateStr_);
        glDisable(GL_SCISSOR_TEST);
    }
};


class TextureBorderColorState : public opengl_drawing::State
{
public:
    bool canProcess(const QString &stateStr_) const override
    {
        return g_renderBorderColorStateName == stateStr_;
    }

    void init(const QString &stateStr_, const std::vector<GLfloat> &) override
    {
        Q_UNUSED(stateStr_);
    }

    void release(const QString &stateStr_, const std::vector<GLfloat> &) override
    {
        Q_UNUSED(stateStr_);
    }

    void init(
            const QString &stateStr_,
            opengl_drawing::Texture* texture_,
            const std::vector<GLfloat> &args_
            ) override
    {
        Q_UNUSED(stateStr_);
        if(args_.size() >= 4
                && texture_
                && texture_->isValidLocation()
                )
        {
            texture_->setWrapClampToBorder();
            const QColor backgroundColor(
                    (int)(args_[0]*255),
                    (int)(args_[1]*255),
                    (int)(args_[2]*255),
                    (int)(args_[3]*255)
                    );
            texture_->setBorderColor(backgroundColor);
        }
    }

    void release(
            const QString &stateStr_,
            opengl_drawing::Texture* texture_,
            const std::vector<GLfloat> &args_
            ) override
    {
        Q_UNUSED(stateStr_);
        Q_UNUSED(texture_);
        Q_UNUSED(args_);
    }
};


class TextureWrapToEdgeState : public opengl_drawing::State
{
public:
    bool canProcess(const QString &stateStr_) const override
    {
        return g_renderWrapToEdgeStateName == stateStr_;
    }

    void init(const QString &stateStr_, const std::vector<GLfloat> &) override
    {
        Q_UNUSED(stateStr_);
    }

    void release(const QString &stateStr_, const std::vector<GLfloat> &) override
    {
        Q_UNUSED(stateStr_);
    }

    void init(
            const QString &stateStr_,
            opengl_drawing::Texture* texture_,
            const std::vector<GLfloat> &args_
            ) override
    {
        Q_UNUSED(stateStr_);
        Q_UNUSED(args_);
        if(texture_
                && texture_->isValidLocation()
                )
        {
            texture_->setWrapClampToEdge();
        }
    }

    void release(
            const QString &stateStr_,
            opengl_drawing::Texture* texture_,
            const std::vector<GLfloat> &args_
            ) override
    {
        Q_UNUSED(stateStr_);
        Q_UNUSED(texture_);
        Q_UNUSED(args_);
    }
};


class ClearBackgroundState : public opengl_drawing::State
{
public:
    bool canProcess(const QString &stateStr_) const override
    {
        return g_renderClearBackgroundStateName == stateStr_;
    }

    void init(const QString &stateStr_, const std::vector<GLfloat> &args_) override
    {
        Q_UNUSED(stateStr_);
        if(args_.size() >= 4)
        {
            glClearColor(args_[0], args_[1], args_[2], args_[3]);
        }
        else if(args_.size() >= 3)
        {
            glClearColor(args_[0], args_[1], args_[2], 1.0);
        }
        else
        {
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void release(const QString &stateStr_, const std::vector<GLfloat> &) override
    {
        Q_UNUSED(stateStr_);
    }
};


void opengl_drawing::State::init(
        const QString &stateStr_,
        Texture* texture_,
        const std::vector<GLfloat> &args_
        )
{
    Q_UNUSED(stateStr_);
    Q_UNUSED(texture_);
    Q_UNUSED(args_);
}

void opengl_drawing::State::release(
        const QString &stateStr_,
        Texture* texture_,
        const std::vector<GLfloat> &args_
        )
{
    Q_UNUSED(stateStr_);
    Q_UNUSED(texture_);
    Q_UNUSED(args_);
}

void opengl_drawing::States::init(const QString &stateStr_, const std::vector<GLfloat> &args_)
{
    for(std::unique_ptr<State> &state_ : m_states)
    {
        if(!state_)
        {
            continue;
        }
        if(state_->canProcess(stateStr_))
        {
            state_->init(stateStr_, args_);
        }
    }
}

void opengl_drawing::States::release(const QString &stateStr_, const std::vector<GLfloat> &args_)
{
    for(std::unique_ptr<State> &state_ : m_states)
    {
        if(!state_)
        {
            continue;
        }
        if(state_->canProcess(stateStr_))
        {
            state_->release(stateStr_, args_);
        }
    }
}

opengl_drawing::State * opengl_drawing::States::find(const QString &stateStr_) const
{
    for(const std::unique_ptr<State> &state_ : qAsConst(m_states))
    {
        if(!state_)
        {
            continue;
        }
        if(state_->canProcess(stateStr_))
        {
            return state_.get();
        }
    }
    return nullptr;
}

std::unique_ptr<opengl_drawing::States> opengl_drawing::States::create()
{
    std::unique_ptr<opengl_drawing::States> result = std::make_unique<opengl_drawing::States>();
    result->m_states.push_back(std::make_unique<AlphaBlindingDisable>());
    result->m_states.push_back(std::make_unique<AlphaBlindingDefault>());
    result->m_states.push_back(std::make_unique<DepthTestEnable>());
    result->m_states.push_back(std::make_unique<DepthTestDisable>());
    result->m_states.push_back(std::make_unique<DrawClipRectState>());
    result->m_states.push_back(std::make_unique<TextureBorderColorState>());
    result->m_states.push_back(std::make_unique<ClearBackgroundState>());
    result->m_states.push_back(std::make_unique<TextureWrapToEdgeState>());
    return result;
}


void opengl_drawing::Object::free()
{
    program.reset();
    m_imageData.reset();
}

void opengl_drawing::Object::init(
        const std::shared_ptr<drawing_data::QuizImageObject> &imageData_,
        const QColor &backgroundColor_
        )
{
    m_states = States::create();

    free();

    m_imageData = imageData_;

    if(!m_imageData
            || m_imageData->vertexShader.isEmpty()
            || m_imageData->fragmentShader.isEmpty()
            )
    {
        return;
    }

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

    for(const auto &attribute : qAsConst(m_imageData->attributes))
    {
        if(!attribute.operator bool())
        {
            continue;
        }

        attributes[attribute->name()] = program->attributeLocation(
                    attribute->name()
                    );
    }

    for(const auto &uniform : qAsConst(m_imageData->uniforms))
    {
        if(!uniform.operator bool())
        {
            continue;
        }

        uniforms[uniform->name()] = program->uniformLocation(
                    uniform->name()
                    );
    }

    for(const drawing_data::Texture &texture_ : qAsConst(m_imageData->textures))
    {
        if(texture_.name.trimmed().isEmpty())
        {
            continue;
        }

        std::unique_ptr<Texture> texture = std::make_unique<Texture>();
        if(!texture_.filename.trimmed().isEmpty())
        {
            if(!texture->setFilename(texture_.filename, backgroundColor_))
            {
                continue;
            }
        }
        else if(!texture_.textureName.trimmed().isEmpty())
        {
            if(!texture->setPipelineTexture(texture_.textureName, backgroundColor_))
            {
                continue;
            }
        }
        else
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

    for(const auto &uniform : qAsConst(m_imageData->uniforms))
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
    for(const drawing_data::Texture &texture_ : qAsConst(m_imageData->textures))
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

    for(const auto &attribute : qAsConst(attributes))
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

    for(const auto &attribute : qAsConst(attributes))
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

    for(const auto &attribute : qAsConst(m_imageData->attributes))
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
    for(const drawing_data::Texture &texture_ : qAsConst(m_imageData->textures))
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
        const QString& newFilename_,
        const QColor &backgroundColor_
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
        textures[name_]->setFilename(newFilename_, backgroundColor_);
        fit->filename = newFilename_;
        return;
    }

    if(newFilename_ == fit->filename)
    {
        return;
    }

    std::unique_ptr<Texture> texture = std::make_unique<Texture>();
    if(!texture->setFilename(newFilename_, backgroundColor_))
    {
        return;
    }

    if(std::end(m_imageData->textures) == fit)
    {
        m_imageData->textures.push_back({name_, newFilename_, QString{}});
    }
    else
    {
        fit->filename = newFilename_;
    }
    textures[name_] = std::move(texture);

    setTextureIndexes();
}

void opengl_drawing::Object::setTextureFromFrameBuffer(
        const QString &name_,
        QOpenGLFramebufferObject *currentFrameBufferObject_,
        const QColor &backgroundColor_
        )
{
    auto fit = std::find_if(
                std::begin(m_imageData->textures),
                std::end(m_imageData->textures),
                [&name_](const drawing_data::Texture &texture_)->bool
    {
        return name_ == texture_.name;
    });

    if(std::end(m_imageData->textures) == fit)
    {
        return;
    }

    if(textures.find(name_) != std::end(textures)
            && textures[name_].operator bool()
            )
    {
        textures[name_]->setFromFrameBufferObject(currentFrameBufferObject_, backgroundColor_);
        return;
    }

    std::unique_ptr<Texture> texture = std::make_unique<Texture>();
    if(!texture->setFromFrameBufferObject(currentFrameBufferObject_, backgroundColor_))
    {
        return;
    }
    if(textures.find(name_) == std::end(textures))
    {
        textures.insert({name_, std::move(texture)});
    }
    else
    {
        textures[name_] = std::move(texture);
    }

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

void opengl_drawing::Object::initStates()
{
    if(!m_imageData)
    {
        return;
    }
    const std::vector<GLfloat> emptyArgs;
    if(!m_imageData->objectStates.empty() && m_states)
    {
        for(const QString &stateStr_ : qAsConst(m_imageData->objectStates))
        {
            m_states->init(stateStr_, emptyArgs);
        }
    }
    const QStringList argumentNames = getUniqueValues(m_imageData->getArgumentNames());
    for(const QString &argumentName_ : qAsConst(argumentNames))
    {
        if(argumentName_.startsWith(g_renderObjectsStateStartName)
                && argumentName_ != g_renderObjectsStatesName
                )
        {
            const QString stateName = argumentName_.mid(QString(g_renderObjectsStateStartName).length());
            std::vector<GLfloat> argumentValues;
            m_imageData->getArgumentValue(argumentName_, argumentValues);
            m_states->init(stateName, argumentValues);
        }
        const QString textureName = getTextureStateName(argumentName_);
        if(!textureName.isEmpty())
        {
            const QString stateName = argumentName_.mid(textureName.length());
            State * state = m_states->find(stateName);
            std::vector<GLfloat> argumentValues;
            m_imageData->getArgumentValue(argumentName_, argumentValues);
            const auto fit = textures.find(textureName);
            if(state && std::end(textures) != fit)
            {
                state->init(stateName, fit->second.get(), argumentValues);
            }
        }
    }
}

void opengl_drawing::Object::releaseStates()
{
    if(!m_imageData)
    {
        return;
    }
    const std::vector<GLfloat> emptyArgs;
    if(!m_imageData->objectStates.empty() && m_states)
    {
        for(const QString &stateStr_ : qAsConst(m_imageData->objectStates))
        {
            m_states->release(stateStr_, emptyArgs);
        }
    }
    const QStringList argumentNames = getUniqueValues(m_imageData->getArgumentNames());
    for(const QString &argumentName_ : qAsConst(argumentNames))
    {
        if(argumentName_.startsWith(g_renderObjectsStateStartName)
                && argumentName_ != g_renderObjectsStatesName
                )
        {
            const QString stateName = argumentName_.mid(QString(g_renderObjectsStateStartName).length());
            std::vector<GLfloat> argumentValues;
            m_imageData->getArgumentValue(argumentName_, argumentValues);
            m_states->release(stateName, argumentValues);
        }
        const QString textureName = getTextureStateName(argumentName_);
        if(!textureName.isEmpty())
        {
            const QString stateName = argumentName_.mid(textureName.length());
            State * state = m_states->find(stateName);
            std::vector<GLfloat> argumentValues;
            m_imageData->getArgumentValue(argumentName_, argumentValues);
            const auto fit = textures.find(textureName);
            if(state && std::end(textures) != fit)
            {
                state->release(stateName, fit->second.get(), argumentValues);
            }
        }
    }
}

void opengl_drawing::Object::setTextureIndexes()
{
    int textureIndex = 0;
    for(const drawing_data::Texture &texture_ : qAsConst(m_imageData->textures))
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

QString opengl_drawing::Object::getTextureStateName(const QString &argumentName_) const
{
    for(const TexturesMap::value_type &val_ : qAsConst(textures))
    {
        if(argumentName_.startsWith(val_.first))
        {
            return val_.first;
        }
    }
    return {};
}


std::shared_ptr<drawing_data::QuizImageObjects> opengl_drawing::Objects::free()
{
    clearObjects();
    return std::move(m_imageData);
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
                getImageMatrix(objects, g_renderFromImageName, windowSize )
                );
    objects->setUniform(
                g_renderToImageMatrixName,
                getImageMatrix(objects, g_renderToImageName, windowSize )
                );
}

QMatrix4x4 ImageMatrixDefaultCalculation::getImageMatrix(
        opengl_drawing::Objects *objects_,
        const QString &imageName_,
        const QSize &windowSize_
        ) const
{
    Q_UNUSED(objects_);
    Q_UNUSED(imageName_);
    Q_UNUSED(windowSize_);

    //const QSize imageSize = objects_->getTextureSize(imageName_ , windowSize_);
    //qDebug() << "ImageMatrixDefaultCalculation::getImageMatrix()";
    //qDebug() << "imageSize = " << imageSize << " windowSize_ = " << windowSize_;
    //QMatrix4x4 m = ::opengl_drawing::calculatePreserveAspectFitTextureMatrix(imageSize, windowSize_);

    //std::vector<GLfloat> vertexDataRow;
    //objects_->getArgumentValue(g_renderTextureAttributeName, vertexDataRow);
    //std::vector<QVector4D> vertexData;
    //drawingdata::utils::vecToAttribute(vertexDataRow, vertexData);
    //std::vector<QVector4D> vertexDataM;
    //vertexDataM.resize(vertexData.size());
    //std::transform(
    //            std::begin(vertexData), std::end(vertexData),
    //            std::begin(vertexDataM),
    //            [&m](const QVector4D &pt_)->QVector4D
    //{
    //    return m*pt_;
    //});
    //qDebug() << "vertexData :" << g_renderTextureAttributeName << vertexData;
    //qDebug() << "vertexDataM :" << g_renderTextureAttributeName << vertexDataM;

    return QMatrix4x4{}; //m;
}


class GeometryMatrixDefaultCalculation : public opengl_drawing::IEffectCalculation
{
public:
    GeometryMatrixDefaultCalculation();
    void calculate(opengl_drawing::IVariables *variables_) const override;

private:
    QMatrix4x4 getGeometryMatrix(
            opengl_drawing::Objects *objects_
            ) const;
};


GeometryMatrixDefaultCalculation::GeometryMatrixDefaultCalculation()
{
    setFilename(g_renderImageGeometryMatrixDefaultCalculation);
    setRequiredVariables({
                 g_renderScreenRectName,
                 g_renderWindowSizeName,
                 g_renderFromImageName,
                 g_renderToImageName
                });
}

void GeometryMatrixDefaultCalculation::calculate(opengl_drawing::IVariables *variables_) const
{
    opengl_drawing::Objects *objects = dynamic_cast<opengl_drawing::Objects *>(variables_);
    if(!objects)
    {
        return;
    }

    objects->setUniform( g_renderMatrixName, getGeometryMatrix(objects));
}

QMatrix4x4 GeometryMatrixDefaultCalculation::getGeometryMatrix(
        opengl_drawing::Objects *objects_
        ) const
{
    Q_UNUSED(objects_);
    QMatrix4x4 renderMatrix;
    renderMatrix.ortho(-1.0, 1.0, 1.0, -1.0, -1.0, 1.0);
    return renderMatrix;
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
                  //g_renderScreenRectName,
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
    const bool textureAttributeExist = textureAttributeTupleSize > 0;

    //qDebug() << "windowSize" << windowSize << "g_renderScreenRectName = " << proportinalRect << "geometryFacedSize =" << geometryFacedSize << "isSolidGeometry" << isSolidGeometry;

    std::vector<GLfloat> vertexData;
    std::vector<GLfloat> textureData;

    /*proportinalRect.x(), proportinalRect.y(),*/
    opengl_drawing::makeGeometry(
                 (int)geometryFacedSize.x(), (int)geometryFacedSize.y(),
                 geometryFacedInterval.x(), geometryFacedInterval.y(),
                 vertexAttributeTupleSize,
                 textureAttributeTupleSize, textureAttributeExist,
                 isSolidGeometry,
                 vertexData, textureData);

    //qDebug() << "vertexData" << vertexData;
    //qDebug() << textureData;

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
GeometryMatrixDefaultCalculation g_geometryMatrixDefaultCalculation;
GeometryDefaultCalculation g_geometryDefaultCalculation;

void opengl_drawing::Objects::init(
        std::shared_ptr<drawing_data::QuizImageObjects> &&imageData_,
        int windowsId_,
        const QString &_renderTextureName
        )
{
    m_renderWindowsId = windowsId_;
    m_renderTextureName = _renderTextureName;
    m_imageData = std::move(imageData_);
    if(m_imageData)
    {
        m_imageData->postInitialization();
    }
    m_states = States::create();
    reinit();
}

void opengl_drawing::Objects::clearObjects()
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
    m_imageMatrixDefault = nullptr;
    m_geometryDefault = nullptr;
    m_updated.clear();
}

void opengl_drawing::Objects::reinit()
{
    clearObjects();
    QVector<double> currentT;
    const bool tExist = get(g_renderTName, currentT);
    m_objects.reserve(m_imageData->objects.size());
    for(const auto &object : qAsConst(m_imageData->objects))
    {
        if(tExist
                && !object->allowedForTime(currentT[0])
                )
        {
            continue;
        }
        m_objects.push_back(
                    std::make_unique<opengl_drawing::Object>()
                    );
        m_objects.back()->init(object, m_imageData ? m_imageData->clearColor : QColor(0,0,0));
    }

    auto doExtend = [](
            drawing_data::QuizImageObjects *objects_,
            opengl_drawing::IEffectCalculation *newCalc_
            )->bool
    {
        for(const auto &calc_ : qAsConst(objects_->calculations))
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
    if(m_imageMatrixDefault)
    {
        m_imageMatrixDefault->clearUpdated();
    }
    m_geometryMatrixDefault = !doExtend(m_imageData.get(), &g_geometryMatrixDefaultCalculation)
            ? &g_geometryMatrixDefaultCalculation
            : nullptr
              ;
    if(m_geometryMatrixDefault)
    {
        m_geometryMatrixDefault->clearUpdated();
    }
    m_geometryDefault = !doExtend(m_imageData.get(), &g_geometryDefaultCalculation)
            ? &g_geometryDefaultCalculation
            : nullptr
              ;
    if(m_geometryDefault)
    {
        m_geometryDefault->clearUpdated();
    }
    // m_imageData->clearUpdated(); TODO: add update strategy for data objects
}

bool opengl_drawing::Objects::needToReinit(double tNew_) const
{
    QVector<double> currentT;
    const bool tExist = get(g_renderTName, currentT);
    if(!tExist)
    {
        return false;
    }
    for(const auto &object : qAsConst(m_imageData->objects))
    {
        if(object->changeAllowedForTime(currentT[0], tNew_))
        {
            return true;
        }
    }
    return false;
}

void opengl_drawing::Objects::calculate()
{
    if(!m_imageData.operator bool())
    {
        return;
    }

    if(m_geometryDefault
            && (
                isUpdated(m_geometryDefault->getRequiredVariables(), nullptr)
                || m_geometryDefault->doNeedUpdate()
                )
            )
    {
        m_geometryDefault->calculate(this);
        m_geometryDefault->setUpdated();
    }
    if(m_imageMatrixDefault
            && (isUpdated(m_imageMatrixDefault->getRequiredVariables(), nullptr)
                || m_imageMatrixDefault->doNeedUpdate()
                )
            )
    {
        m_imageMatrixDefault->calculate(this);
        m_imageMatrixDefault->setUpdated();
    }
    if(m_geometryMatrixDefault
            && (isUpdated(m_geometryMatrixDefault->getRequiredVariables(), nullptr)
                || m_geometryMatrixDefault->doNeedUpdate()
                )
            )
    {
        m_geometryMatrixDefault->calculate(this);
        m_geometryMatrixDefault->setUpdated();
    }
    m_imageData->calculate(this);
    clearUpdated();
}

void opengl_drawing::Objects::draw(QOpenGLFunctions *f_)
{
    const IQuizImageQWindowOperations* windowsOperations = IQuizImageQWindow::findQuizImageWindows(m_renderWindowsId);
    for(std::unique_ptr<opengl_drawing::Object> &object_ : m_objects)
    {
        if(!object_.operator bool())
        {
            continue;
        }
        if(windowsOperations
                && windowsOperations->getCurrentTextureName() != m_renderTextureName
                )
        {
            continue;
        }

        object_->initStates();
        object_->bind();
        object_->setUniforms();
        object_->enableAttributes();
        object_->setAttributeArray();
        object_->bindTextures(f_);
        object_->drawTriangles(f_);
        object_->disableAttributes();
        object_->release();
        object_->releaseStates();
    }
}

void opengl_drawing::Objects::initStates()
{
    if(m_imageData && !m_imageData->globalStates.empty() && m_states)
    {
        const std::vector<GLfloat> emptyArgs;
        for(const QString &stateStr_ : qAsConst(m_imageData->globalStates))
        {
            m_states->init(stateStr_, emptyArgs);
        }
    }
    else
    {
        glFrontFace(GL_CW);
        glCullFace(GL_FRONT);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
    }
    if(m_imageData)
    {
        const QStringList argumentNames = getUniqueValues(m_imageData->getArgumentNames());
        for(const QString &argumentName_ : qAsConst(argumentNames))
        {
            if(!argumentName_.startsWith(g_renderGlobalStatesName)
                    || argumentName_ == g_renderGlobalStatesName
                    )
            {
                continue;
            }
            const QString stateName = argumentName_.mid(QString(g_renderGlobalStatesName).length());
            std::vector<GLfloat> argumentValues;
            m_imageData->getArgumentValue(argumentName_, argumentValues);
            m_states->init(stateName, argumentValues);
        }
    }
}

void opengl_drawing::Objects::releaseStates()
{
    if(m_imageData && !m_imageData->globalStates.empty() && m_states)
    {
        const std::vector<GLfloat> emptyArgs;
        for(const QString &stateStr_ : qAsConst(m_imageData->globalStates))
        {
            m_states->release(stateStr_, emptyArgs);
        }
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
    }
    if(m_imageData)
    {
        const QStringList argumentNames = getUniqueValues(m_imageData->getArgumentNames());
        for(const QString &argumentName_ : qAsConst(argumentNames))
        {
            if(!argumentName_.startsWith(g_renderGlobalStatesName)
                    || argumentName_ == g_renderGlobalStatesName
                    )
            {
                continue;
            }
            const QString stateName = argumentName_.mid(QString(g_renderGlobalStatesName).length());
            std::vector<GLfloat> argumentValues;
            m_imageData->getArgumentValue(argumentName_, argumentValues);
            m_states->release(stateName, argumentValues);
        }
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
    for(const std::unique_ptr<Object> &object_ : qAsConst(m_objects))
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
        const QString &textureName_,
        const QString &newFilename_
        )
{
    setTexture(textureName_, newFilename_, getClearColor());
}

void opengl_drawing::Objects::setTexture(
        const QString &name_,
        const QString &newFilename_,
        const QColor &backgroundColor_
        )
{
    if(!m_updated.contains(name_))
    {
        m_updated.push_back(name_);
    }
    for(std::unique_ptr<Object> &object_ : m_objects)
    {
        object_->setTexture(name_, newFilename_, backgroundColor_);
    }
}

void opengl_drawing::Objects::setTextureFromCurrentFrameBuffer(
        const QString &textureName_
        )
{
    setTextureFromCurrentFrameBuffer(textureName_, getClearColor());
}

void opengl_drawing::Objects::setTextureFromCurrentFrameBuffer(
        const QString &textureName_
        , const QColor &newBackgroundColor_
        )
{
    if(!m_updated.contains(textureName_))
    {
        m_updated.push_back(textureName_);
    }
    for(std::unique_ptr<Object> &object_ : m_objects)
    {
        object_->setTextureFromFrameBuffer(textureName_, m_currentFrameBufferObject, newBackgroundColor_);
    }
}

void opengl_drawing::Objects::setFromImage(const QString &url_)
{
    setTexture(g_renderFromImageName, url_, m_imageData ? m_imageData->clearColor : QColor(0,0,0));    // TODO: dependency inversion
}

void opengl_drawing::Objects::setToImage(const QString &url_)
{
    setTexture(g_renderToImageName, url_, m_imageData ? m_imageData->clearColor : QColor(0,0,0));  // TODO: dependency inversion
}

void opengl_drawing::Objects::setCurrentFrameBufferObject(QOpenGLFramebufferObject *currentFrameBufferObject_)
{
    m_currentFrameBufferObject = currentFrameBufferObject_;
}

QMatrix4x4 opengl_drawing::Objects::getImageMatrix(const QString &imageName_, const QSize &windowSize_) const
{
    const QSize imageSize = getTextureSize(imageName_ , windowSize_);
    return ::opengl_drawing::calculatePreserveAspectFitTextureMatrix(imageSize, windowSize_);
}

bool opengl_drawing::Objects::isUpdated(const QSet<QString> &vars_, IVariables *base_) const
{
    for(const QString &var_ : qAsConst(m_updated))
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

void ObjectsRenderer::setCurrentFrameBufferObject(QOpenGLFramebufferObject *fbobj_)
{
    m_currentFrameBufferObject = fbobj_;
    if(m_openglData)
    {
        m_openglData->setCurrentFrameBufferObject(m_currentFrameBufferObject);
    }
}

void ObjectsRenderer::setImageData(
        std::shared_ptr<drawing_data::QuizImageObjects> imageData_,
        int windowsId_,
        const QString &_renderTextureName
        )
{
    m_openglData = std::make_unique<opengl_drawing::Objects>();
    m_openglData->init(std::move(imageData_), windowsId_, _renderTextureName);
    m_openglData->setCurrentFrameBufferObject(m_currentFrameBufferObject);
    initialize();
}

std::shared_ptr<drawing_data::QuizImageObjects> ObjectsRenderer::releaseImageData()
{
    if(!isValidData())
    {
        return {nullptr};
    }

    if(m_openglData)
    {
        m_openglData->setCurrentFrameBufferObject(nullptr);
    }
    return m_openglData ? m_openglData->free() : std::shared_ptr<drawing_data::QuizImageObjects>{nullptr};
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

inline
qreal alignValueToEPS(qreal v_, qreal eps_)
{
    double v = v_;
    v /= eps_;
    v = floor(v);
    v *= eps_;
    return v;
}

void ObjectsRenderer::updateVariables(
        const QVector2D &rectSize_,
        qreal t_,
        const QVector2D &windowSize_)
{
    t_ = alignValueToEPS(t_, g_renderTeps);

    if(m_openglData
            && m_openglData->needToReinit(t_)
            )
    {
        setUniformIfExistsAndChanged( g_renderTName, t_ );
        m_openglData->reinit();
        initialize();
    }

    const float maxCxCy = std::max(std::max(rectSize_.x(), rectSize_.y()), 1.0f);
    const QVector2D proportinalRect(rectSize_.x() / maxCxCy, rectSize_.y() / maxCxCy);

    setUniformIfExistsAndChanged( g_renderTName, t_ );
    setUniformIfExistsAndChanged( g_renderScreenRectName, proportinalRect );
    //setUniformIfExistsAndChanged( g_renderMatrixName, getScreenMatrix(proportinalRect));
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
    Q_UNUSED(proportinalRect_);
    QMatrix4x4 renderMatrix;
    //renderMatrix.ortho(QRectF(0,0, proportinalRect_.x(), proportinalRect_.y()));
    //renderMatrix.ortho(0.0, proportinalRect_.x(), proportinalRect_.y(), 0.0, -10.0, 10.0);
    renderMatrix.ortho(-1.0, 1.0, 1.0, -1.0, -1.0, 1.0);
    // it is scale matrix
    //         1         0         0         0
    //         0        -1         0         0
    //         0         0         1         0
    //         0         0         0         1
    // it changes coordinates independent from each other
    //qDebug() << "ObjectsRenderer::getScreenMatrix()" << renderMatrix;
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
        const auto clearColor = m_openglData->getClearColor();
        const auto r = clearColor.redF();
        const auto g = clearColor.greenF();
        const auto b = clearColor.blueF();
        glClearColor(r, g, b, 1.0f);
    }
    else
    {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    if(isValidData())
    {
        m_openglData->initStates();
    }
    else
    {
        glFrontFace(GL_CW);
        glCullFace(GL_FRONT);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
    }

    if(isValidData())
    {
        m_openglData->draw(this);
    }

    if(isValidData())
    {
        m_openglData->releaseStates();
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
    }
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

namespace opengl_drawing
{

/*float width_, float height_,*/
void makeGeometry(
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
                                        ((x + coords[j][k][0]) / (GLfloat)geomertyPointsWidth_ - 0.5) * 2.0; //  * width_
                                vertexData_[offs0 + 1] =
                                        ((y + coords[j][k][1]) / (GLfloat)geometryPointsHeight_ - 0.5) * 2.0; //  * height_
                        }
                        else
                        {
                            vertexData_[offs0 + 0] =
                                    ((x + coords[j][k][0]) / (GLfloat)geomertyPointsWidth_ - 0.5) * 2.0//  * width_
                                    - (coords[j][k][0] * 2 - 1) * facedGeometryXCoef_
                                    ;
                            vertexData_[offs0 + 1] =
                                    ((y + coords[j][k][1]) / (GLfloat)geometryPointsHeight_ - 0.5) * 2.0 //  * height_
                                    - (coords[j][k][1] * 2 - 1) * facedGeometryYCoef_
                                    ;
                        }
                    }
                    if(geometryVertexCoords_ >= 3)
                    {
                        vertexData_[offs0 + 2] = -0.5;
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

} // namespace opengl_drawing


void QuizImageFboRendererImpl::renderImpl()
{
    m_objectRenderer.render();
}

QOpenGLFramebufferObject *QuizImageFboRendererImpl::createFramebufferObjectImpl(const QSize &size_)
{
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    //format.setSamples(4);
    auto fbobj = new QOpenGLFramebufferObject(size_, format);
    m_objectRenderer.setCurrentFrameBufferObject(fbobj);
    return fbobj;
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

std::shared_ptr<drawing_data::QuizImageObjects> QuizImageFboRendererImpl::releaseImageData()
{
    return m_objectRenderer.releaseImageData();
}

void QuizImageFboRendererImpl::setImageData(
        std::shared_ptr<drawing_data::QuizImageObjects> imageData_,
        int windowsId_,
        const QString &_renderTextureName
        )
{
    m_objectRenderer.setImageData(std::move(imageData_), windowsId_, _renderTextureName);
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
        QuizImageObjects &data_,
        int argsSetIndex_ /*= 0*/
        ) const
{
    Q_UNUSED(argsSetIndex_);

    std::unique_ptr<QuizImageObject> object = std::make_unique<QuizImageObject>();

    static QByteArray vertex = loadTextFile( g_defaultVertexShaderName ).toUtf8();
    static QByteArray fragment = loadTextFile( g_defaultFragmentShaderName ).toUtf8();
    object->vertexShader = vertex.constData();
    object->fragmentShader = fragment.constData();

    object->textures = {
        { g_renderFromImageName, g_noImage001Name, QString{} },
        { g_renderToImageName, g_noImage002Name, QString{} },
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
