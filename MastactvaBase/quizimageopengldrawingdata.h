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
        /*
         * find state for the state string
        */
        State * find(const QString &stateStr_) const;

        /*
         * init open gl drawing state step
         * switch on state of the state string
        */
        void init(const QString &stateStr_, const std::vector<GLfloat> &args_);

        /*
         * release open gl drawing state step
         * switch off state of the state string
        */
        void release(const QString &stateStr_, const std::vector<GLfloat> &args_);

        /*
         * factory method
        */
        static std::unique_ptr<States> create();

    private:
        std::vector<std::unique_ptr<State>> m_states;
    };


    /*
     * open gl drawing object class
    */
    class Object
    {
    public:
        /*
         * release this object and it's open gl datas
        */
        void free();

        /*
         * init this object with quiz image opern gl drawing object
         * TODO: move color to the some argument's set
        */
        void init(
                const std::shared_ptr<drawing_data::QuizImageObject> &imageData_,
                const QColor &backgroundColor_
                );

        // pipeline operations
        // {
        /*
         * do open gl bind for arguments and textures and so on in the open gl pipeline
        */
        void bind();

        /*
         * do set uniform for open gl arguments in the open gl pipeline
        */
        void setUniforms();

        /*
         * do enable attributes for open gl arguments in the open gl pipeline
        */
        void enableAttributes();

        /*
         * do disable attributes for open gl arguments in the open gl pipeline
        */
        void disableAttributes();

        /*
         * do set attribute's arrays for open gl arguments in the open gl pipeline
        */
        void setAttributeArray();

        /*
         * do bind textures in the open gl pipeline
        */
        void bindTextures(QOpenGLFunctions *f_);

        /*
         * do draw triangles in the open gl pipeline
        */
        void drawTriangles(QOpenGLFunctions *f_);

        /*
         * do draw release in the open gl pipeline
        */
        void release();
        // }

        /*
         * set up texture for the drawing object
        */
        void setTexture(const QString &name_, const QString& newFilename_, const QColor &backgroundColor_);

        /*
         * set up texture for the drawing object from the frame buffer
        */
        void setTextureFromFrameBuffer(const QString &name_, QOpenGLFramebufferObject *currentFrameBufferObject_, const QColor &backgroundColor_);

        /*
         * return texture size
        */
        bool getTextureSize(const QString &name_, QSize &imageSize_) const;

        /*
         * is object correctly minimum initialized
        */
        bool isUsable() const;

        /*
         * init object's states
         * switch on all object's states
        */
        void initStates();

        /*
         * release object's states
         * switch off all object's states
        */
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


    /*
     * open gl drawing list of objects class
    */
    class Objects : public IVariables, public drawingdata::ITextures
    {
    public:
        /*
         * release quiz image drawing objects list datas
        */
        std::unique_ptr<drawing_data::QuizImageObjects> free();

        /*
         * init from quiz image drawing objcts list datas
         * capture quiz image objects list datas
        */
        void init(std::unique_ptr<drawing_data::QuizImageObjects> &&imageData_);

        /*
         * do low level reinitialization
        */
        void reinit();

        /*
         * do we need to reinit this objects list for new time value
        */
        bool needToReinit(double tNew_) const;

        /*
         * process all required claculations
        */
        void calculate();

        /*
         * open gl pipeline operations - draw objects list
        */
        void draw(QOpenGLFunctions *f_);

        /*
         * init global states
         * switch on global states
        */
        void initStates();

        /*
         * release global states
         * switch off global states
        */
        void releaseStates();

        // variable operations
        // {
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
        // }

        /*
         * set texture at the objects root level
        */
        void setTexture(const QString &textureName_, const QString &newFilename_) override;
        void setTexture(const QString &textureName_, const QString &newFilename_, const QColor &newBackgroundColor_) override;

        /*
         * set texture from the frame buffer at the objects root level
        */
        void setTextureFromCurrentFrameBuffer(const QString &textureName_) override;
        void setTextureFromCurrentFrameBuffer(const QString &textureName_, const QColor &newBackgroundColor_) override;

        /*
         * return clear color
        */
        QColor getClearColor() const;

        /*
         * return sttribute variable tuple size
        */
        int getAttributeTupleSize(const QString &name_) const;

        /*
         * return texture size
        */
        bool getTextureSize(const QString &name_, QSize &size_) const;
        QSize getTextureSize(const QString &name_, const QSize &size_) const;
        //void setTexture(const QString &name_, const QString &newFilename_, const QColor &backgroundColor_);

        /*
         * set up g_renderFromImageName texture
        */
        void setFromImage(const QString &url_);

        /*
         * set up g_renderToImageName texture
        */
        void setToImage(const QString &url_);

        /*
         * helper function to set up frame buffer object
        */
        void setCurrentFrameBufferObject(QOpenGLFramebufferObject *currentFrameBufferObject_);

    private:
        QMatrix4x4 getImageMatrix(const QString &imageName_, const QSize &windowSize_) const;
        void clearUpdated();
        void clearObjects();

    private:
        std::unique_ptr<drawing_data::QuizImageObjects> m_imageData;                // quiz image objects list
        std::vector<std::unique_ptr<Object>> m_objects;                             // objects list
        opengl_drawing::IEffectCalculation *m_imageMatrixDefault = nullptr;         // some default calculations TODO: extract to the separate class
        opengl_drawing::IEffectCalculation *m_geometryMatrixDefault = nullptr;
        opengl_drawing::IEffectCalculation *m_geometryDefault = nullptr;
        QStringList m_updated;                                                      // list of variable that was updated duiring last calculations
        std::unique_ptr<States> m_states;                                           // global states
        QOpenGLFramebufferObject *m_currentFrameBufferObject = nullptr;             // frame buffer object pointer
    };
}


/*
 * render class
*/
class ObjectsRenderer : protected QOpenGLFunctions, public opengl_drawing::IVariables
{
public:
    ObjectsRenderer();
    ~ObjectsRenderer();

    /*
     * set up quiz image data
    */
    void setImageData(std::unique_ptr<drawing_data::QuizImageObjects> imageData_);

    /*
     * release the current quiz image data
    */
    std::unique_ptr<drawing_data::QuizImageObjects> releaseImageData();

    /*
     * render the current objects
    */
    void render();

    /*
     * set up g_renderFromImageName texture
    */
    void setFromImage(const QString &url_);

    /*
     * set up g_renderToImageName texture
    */
    void setToImage(const QString &url_);

    /*
     * update size, time and derived variables
    */
    void updateVariables(
            const QVector2D &rectSize_,
            qreal t_,
            const QVector2D &windowSize_);

    /*
     * work with variables
     * {
    */
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
    // }

    /*
     * return attribute variable tuple size
    */
    int getAttributeTupleSize(const QString &name_) const;

    /*
     * return texture size
    */
    QSize getTextureSize(const QString &name_, const QSize &size_) const;

private:
    bool getTextureSize(const QString &name_, QSize &size_) const;
    void initialize();
    bool isValidData() const;
    static QMatrix4x4 getScreenMatrix(const QVector2D &proportinalRect_);

private:
    std::unique_ptr<opengl_drawing::Objects> m_openglData;  // contains opne gl drawing objects
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
