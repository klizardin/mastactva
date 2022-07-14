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

#ifndef QUIZIMAGEOPENGLDRAWINGDATA_H
#define QUIZIMAGEOPENGLDRAWINGDATA_H


#include <QtGui/QOpenGLFramebufferObject>
#include <QtGui/qopenglshaderprogram.h>
#include <QtGui/qopenglfunctions.h>
#include <QtGui/QOpenGLFramebufferObject>
#include <QtQuick/QQuickWindow>
#include <QQuickFramebufferObject>
#include "../MastactvaBase/quizimagedrawingdata.h"
#include "../MastactvaBase/opengldrawing_utils.h"
#include "../MastactvaBase/drawingdata_utils.h"


/*
 * open gl drawing classes
*/
namespace opengl_drawing
{
    /*
     * open gl drawing texture
    */
    class Texture
    {
    public:
        /*
         * set file name of the texture
         * setTexture()
        */
        bool setFilename(const QString &fileName_, const QColor &backgroundColor_);

        /*
         * setTexture() from the open gl frame buffer object
        */
        bool setFromFrameBufferObject(QOpenGLFramebufferObject *frameBufferObject_, const QColor &backgroundColor_);

        /*
         * set up texture location
        */
        void setLocation(int location_);

        /*
         * set up index
        */
        void setIndex(int index_);

        /*
         * do setUniform() for the texture
        */
        void setUniform(QOpenGLShaderProgram *program_) const;

        /*
         * do bind() for the texture object
        */
        void bind(QOpenGLFunctions *f_) const;

        /*
         * return size of the texture
        */
        bool getSize(QSize &size_) const;

        /*
         * (for drawing states)
         * set mode to wrap clamp texture to border
        */
        void setWrapClampToBorder();

        /*
         * (for drawing states)
         * set mode to wrap clamp texture to edge
        */
        void setWrapClampToEdge();

        /*
         * (for drawing states)
         * set border color
        */
        void setBorderColor(const QColor &backgroundColor_);

        /*
         * is location valid (is location initialized, if we have correct texture)
        */
        bool isValidLocation() const;

    private:
        void createTextureFromImage();

    private:
        constexpr static int locationWrongValue{-1};
        int m_index{0};
        int m_location{locationWrongValue};
        QImage m_image;
        std::unique_ptr<QOpenGLTexture> m_texture;
    };


    /*
     * open gl drawing state
    */
    class State
    {
    public:
        /*
         * check if the state str represent the current state object
        */
        virtual bool canProcess(const QString &stateStr_) const = 0;

        /*
         * init open gl drawing state step
         * switch on this state
        */
        virtual void init(const QString &stateStr_, const std::vector<GLfloat> &args_) = 0;

        /*
         * release open gl drawing state step
         * switch off this state
        */
        virtual void release(const QString &stateStr_, const std::vector<GLfloat> &args_) = 0;

        /*
         * init open gl drawing state step for texture
         * switch on this state for texture
        */
        virtual void init(const QString &stateStr_, Texture* texture_, const std::vector<GLfloat> &args_);

        /*
         * release open gl drawing state step for texture
         * switch off this state for texture
        */
        virtual void release(const QString &stateStr_, Texture* texture_, const std::vector<GLfloat> &args_);
    };

    class States
    {
    public:
        State * find(const QString &stateStr_) const;
        void init(const QString &stateStr_, const std::vector<GLfloat> &args_);
        void release(const QString &stateStr_, const std::vector<GLfloat> &args_);

        static std::unique_ptr<States> create();

    private:
        std::vector<std::unique_ptr<State>> m_states;
    };

    class Object
    {
    public:
        void free();
        void init(
                const std::shared_ptr<drawing_data::QuizImageObject> &imageData_,
                const QColor &backgroundColor_
                );
        void bind();
        void setUniforms();
        void enableAttributes();
        void disableAttributes();
        void setAttributeArray();
        void bindTextures(QOpenGLFunctions *f_);
        void drawTriangles(QOpenGLFunctions *f_);
        void release();

        void setTexture(const QString &name_, const QString& newFilename_, const QColor &backgroundColor_);
        void setTextureFromFrameBuffer(const QString &name_, QOpenGLFramebufferObject *currentFrameBufferObject_, const QColor &backgroundColor_);
        bool getTextureSize(const QString &name_, QSize &imageSize_) const;

        bool isUsable() const;
        void initStates();
        void releaseStates();

    private:
        void setTextureIndexes();
        static bool isIdValid(int idValue_);
        QString getTextureStateName(const QString &argumentName_) const;

    private:
        std::shared_ptr<drawing_data::QuizImageObject> m_imageData;
        std::unique_ptr<QOpenGLShaderProgram> program;
        QHash<QString, int> attributes;
        QHash<QString, int> uniforms;
        using TexturesMap = std::map<QString, std::unique_ptr<Texture>>;
        TexturesMap textures;
        std::unique_ptr<States> m_states;
    };

    class Objects : public IVariables, public drawingdata::ITextures
    {
    public:
        std::unique_ptr<drawing_data::QuizImageObjects> free();
        void init(std::unique_ptr<drawing_data::QuizImageObjects> &&imageData_);
        void reinit();
        bool needToReinit(double tNew_) const;
        void calculate();
        void draw(QOpenGLFunctions *f_);
        void initStates();
        void releaseStates();

        bool get(const QString &name_, QVector<double> &data_) const override
        {
            if(!m_imageData.operator bool())
            {
                return false;
            }

            return m_imageData->get(name_, data_);
        }

        void set(const QString &name_, const QVector<double> &data_) override
        {
            if(!m_imageData.operator bool())
            {
                return;
            }

            m_imageData->set(name_, data_);
        }

        void set(const QString &name_, QVector<double> &&data_) override
        {
            if(!m_imageData.operator bool())
            {
                return;
            }

            m_imageData->set(name_, std::move(data_));
        }

        bool get(const QString &name_, QStringList &data_) const override
        {
            if(!m_imageData.operator bool())
            {
                return false;
            }

            return m_imageData->get(name_, data_);
        }

        void set(const QString &name_, const QStringList &data_) override
        {
            if(!m_imageData.operator bool())
            {
                return;
            }

            m_imageData->set(name_, data_);
        }

        void set(const QString &name_, QStringList &&data_) override
        {
            if(!m_imageData.operator bool())
            {
                return;
            }

            m_imageData->set(name_, std::move(data_));
        }

        bool isUpdated(const QSet<QString> &vars_, IVariables *base_) const override;

        template<typename ItemType_>
        void setAttribute(const QString &name_, const std::vector<ItemType_> &value_, int tupleSize_ = 0)
        {
            if(!m_imageData.operator bool())
            {
                return;
            }

            m_imageData->setAttribute(name_, value_, tupleSize_);
        }

        template<typename ItemType_>
        void setUniform(const QString &name_, const ItemType_ &value_)
        {
            if(!m_imageData.operator bool())
            {
                return;
            }

            m_imageData->setUniform(name_, value_);
        }

        template<typename ItemType_>
        bool getUniform(const QString &name_, ItemType_ &value_) const
        {
            if(!m_imageData.operator bool())
            {
                return false;
            }

            return m_imageData->getUniform(name_, value_);
        }

        template<typename ItemType_>
        ItemType_ getUniform(const QString &name_, const ItemType_ &defaultValue_) const
        {
            ItemType_ result{};
            if(getUniform(name_, result))
            {
                return result;
            }
            else
            {
                return defaultValue_;
            }
        }

        QStringList getArgumentNames() const
        {
            return m_imageData ? m_imageData->getArgumentNames() : QStringList{};
        }

        bool getArgumentValue(const QString &name_, std::vector<GLfloat> &values_) const
        {
            values_.clear();
            if(!m_imageData)
            {
                return false;
            }
            return m_imageData->getArgumentValue(name_, values_);
        }

        void setTexture(const QString &textureName_, const QString &newFilename_) override;
        void setTexture(const QString &textureName_, const QString &newFilename_, const QColor &newBackgroundColor_) override;
        void setTextureFromCurrentFrameBuffer(const QString &textureName_) override;
        void setTextureFromCurrentFrameBuffer(const QString &textureName_, const QColor &newBackgroundColor_) override;

        QColor getClearColor() const;
        int getAttributeTupleSize(const QString &name_) const;
        bool getTextureSize(const QString &name_, QSize &size_) const;
        QSize getTextureSize(const QString &name_, const QSize &size_) const;
        //void setTexture(const QString &name_, const QString &newFilename_, const QColor &backgroundColor_);
        void setFromImage(const QString &url_);
        void setToImage(const QString &url_);
        void setCurrentFrameBufferObject(QOpenGLFramebufferObject *currentFrameBufferObject_);

    private:
        QMatrix4x4 getImageMatrix(const QString &imageName_, const QSize &windowSize_) const;
        void clearUpdated();
        void clearObjects();

    private:
        std::unique_ptr<drawing_data::QuizImageObjects> m_imageData;
        std::vector<std::unique_ptr<Object>> m_objects;
        opengl_drawing::IEffectCalculation *m_imageMatrixDefault = nullptr;
        opengl_drawing::IEffectCalculation *m_geometryMatrixDefault = nullptr;
        opengl_drawing::IEffectCalculation *m_geometryDefault = nullptr;
        QStringList m_updated;
        std::unique_ptr<States> m_states;
        QOpenGLFramebufferObject *m_currentFrameBufferObject = nullptr;
    };
}


class ObjectsRenderer : protected QOpenGLFunctions, public opengl_drawing::IVariables
{
public:
    ObjectsRenderer();
    ~ObjectsRenderer();

    void setImageData(std::unique_ptr<drawing_data::QuizImageObjects> imageData_);
    std::unique_ptr<drawing_data::QuizImageObjects> releaseImageData();
    void render();
    void setFromImage(const QString &url_);
    void setToImage(const QString &url_);
    void updateVariables(
            const QVector2D &rectSize_,
            qreal t_,
            const QVector2D &windowSize_);

    bool get(const QString &name_, QVector<double> &data_) const override
    {
        if(!m_openglData.operator bool())
        {
            return false;
        }

        return m_openglData->get(name_, data_);
    }

    void set(const QString &name_, const QVector<double> &data_) override
    {
        if(!m_openglData.operator bool())
        {
            return;
        }

        m_openglData->set(name_, data_);
    }

    void set(const QString &name_, QVector<double> &&data_) override
    {
        if(!m_openglData.operator bool())
        {
            return;
        }

        m_openglData->set(name_, std::move(data_));
    }

    bool get(const QString &name_, QStringList &data_) const override
    {
        if(!m_openglData.operator bool())
        {
            return false;
        }

        return m_openglData->get(name_, data_);
    }

    void set(const QString &name_, const QStringList &data_) override
    {
        if(!m_openglData.operator bool())
        {
            return;
        }

        m_openglData->set(name_, data_);
    }

    void set(const QString &name_, QStringList &&data_) override
    {
        if(!m_openglData.operator bool())
        {
            return;
        }

        m_openglData->set(name_, std::move(data_));
    }

    bool isUpdated(const QSet<QString> &vars_, IVariables *base_) const override
    {
        if(!m_openglData.operator bool())
        {
            return false;
        }

        return m_openglData->isUpdated(vars_, base_);
    }

    template<typename ItemType_>
    void setAttribute(const QString &name_, const std::vector<ItemType_> &value_, int tupleSize_ = 0)
    {
        if(!m_openglData.operator bool())
        {
            return;
        }

        m_openglData->setAttribute(name_, value_, tupleSize_);
    }

    template<typename ItemType_>
    void setUniform(const QString &name_, const ItemType_ &value_)
    {
        if(!m_openglData.operator bool())
        {
            return;
        }

        m_openglData->setUniform(name_, value_);
    }

    template<typename ItemType_>
    void setUniformIfExistsAndChanged(const QString &name_, const ItemType_ &value_)
    {
        ItemType_ oldValue{};
        if(getUniform(name_, oldValue)
                && value_ != oldValue)
        {
            setUniform(name_, value_);
        }
    }

    template<typename ItemType_>
    bool getUniform(const QString &name_, ItemType_ &value_) const
    {
        if(!m_openglData.operator bool())
        {
            return false;
        }

        return m_openglData->getUniform(name_, value_);
    }

    template<typename ItemType_>
    ItemType_ getUniform(const QString &name_, const ItemType_ &defaultValue_) const
    {
        ItemType_ result{};
        if(getUniform(name_, result))
        {
            return result;
        }
        else
        {
            return defaultValue_;
        }
    }

    int getAttributeTupleSize(const QString &name_) const;
    QSize getTextureSize(const QString &name_, const QSize &size_) const;

private:
    bool getTextureSize(const QString &name_, QSize &size_) const;
    void initialize();
    bool isValidData() const;
    static QMatrix4x4 getScreenMatrix(const QVector2D &proportinalRect_);

private:
    std::unique_ptr<opengl_drawing::Objects> m_openglData;
};


class QuizImageFboRendererImpl
{
public:
    QuizImageFboRendererImpl() = default;

protected:
    void renderImpl();
    QOpenGLFramebufferObject *createFramebufferObjectImpl(const QSize &size_);
    const QVector2D &getWindowSize() const;
    void setWindowSize(const QVector2D &windowSize_);
    void synchronizeImpl(const QVector2D &rectSize_, qreal t_);
    std::unique_ptr<drawing_data::QuizImageObjects> releaseImageData();
    void setImageData(std::unique_ptr<drawing_data::QuizImageObjects> imageData_);
    void setFromImage(const QString &url_);
    void setToImage(const QString &url_);

private:
    ObjectsRenderer m_objectRenderer;
    QVector2D m_windowSize;
};


template<typename QuizImageType_>
class QuizImageFboRenderer :
        public QuizImageFboRendererImpl
        , public QQuickFramebufferObject::Renderer
{
public:
    QuizImageFboRenderer() = default;

    void render() override
    {
        renderImpl();
    }

    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size_) override
    {
        return createFramebufferObjectImpl(size_);
    }

    void synchronize(QQuickFramebufferObject *frameBufferObject_) override
    {
        QuizImageType_ *quizImage = static_cast<QuizImageType_ *>(frameBufferObject_);
        if(!quizImage)
        {
            return;
        }

        const QVector2D rectSize(quizImage->width(), quizImage->height());
        setWindowSize(rectSize);
        const qreal t = quizImage->t();

        const bool imageDataChanged = quizImage->isImageDataUpdated();
        if(imageDataChanged)
        {
            quizImage->setDataToFree(releaseImageData());
            setImageData(quizImage->getData());
        }
        synchronizeImpl(rectSize, t);
        if(quizImage->isFromImageReady() && !quizImage->getFromImageUrl().isEmpty())
        {
            setFromImage(quizImage->getFromImageUrl());
        }
        if(quizImage->isToImageReady() && !quizImage->getToImageUrl().isEmpty())
        {
            setToImage(quizImage->getToImageUrl());
        }
    }
};


namespace drawing_data
{
    class DefaultQuizImageObject : public IDefaultData<QuizImageObjects>
    {
    public:
        virtual void initialize(QuizImageObjects &data_, int argsSetIndex_ = 0) const override;
    };
}


#endif // QUIZIMAGEOPENGLDRAWINGDATA_H
