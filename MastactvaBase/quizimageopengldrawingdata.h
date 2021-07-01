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


namespace opengl_drawing
{
    class Texture
    {
    public:
        bool setFilename(const QString &fileName_);
        void setLocation(int location_);
        void setIndex(int index_);
        void setUniform(QOpenGLShaderProgram *program_) const;
        void bind(QOpenGLFunctions *f_) const;
        bool getSize(QSize &size_) const;

    private:
        bool isValidLocation() const;

    private:
        constexpr static int locationWrongValue{-1};
        int m_index{0};
        int m_location{locationWrongValue};
        QImage m_image;
        std::unique_ptr<QOpenGLTexture> m_texture;
    };

    class Object
    {
    public:
        void free();
        void init(
                const std::shared_ptr<drawing_data::QuizImageObject> &imageData_
                );
        void bind();
        void setUniforms();
        void enableAttributes();
        void disableAttributes();
        void setAttributeArray();
        void bindTextures(QOpenGLFunctions *f_);
        void drawTriangles(QOpenGLFunctions *f_);
        void release();

        void setTexture(const QString &name_, const QString& newFilename_);
        bool getTextureSize(const QString &name_, QSize &imageSize_) const;

        bool isUsable() const;

    private:
        void setTextureIndexes();
        static bool isIdValid(int idValue_);

    private:
        std::shared_ptr<drawing_data::QuizImageObject> m_imageData;
        std::unique_ptr<QOpenGLShaderProgram> program;
        QHash<QString, int> attributes;
        QHash<QString, int> uniforms;
        std::map<QString, std::unique_ptr<Texture>> textures;
    };

    class Objects : public IVariables
    {
    public:
        std::unique_ptr<drawing_data::QuizImageObjects> free();
        void init(std::unique_ptr<drawing_data::QuizImageObjects> &&imageData_);
        void draw(QOpenGLFunctions *f_);

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

        QColor getClearColor() const;
        int getAttributeTupleSize(const QString &name_) const;
        bool getTextureSize(const QString &name_, QSize &size_) const;
        void setTexture(const QString &name_, const QString &newFilename_);
        void setFromImage(const QString &url_);
        void setToImage(const QString &url_);

    private:
        std::unique_ptr<drawing_data::QuizImageObjects> m_imageData;
        std::vector<std::unique_ptr<Object>> m_objects;
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
            bool imageDataChanged_, bool sizeChanged_,
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
    void updateGeometry(const QVector2D &proportinalRect_);
    void updateSize(const QVector2D &windowSize_);
    void initialize();
    bool isValidData() const;
    QMatrix4x4 getImageMatrix(const QString &imageName_, const QSize &windowSize_) const;
    static QMatrix4x4 getScreenMatrix(const QVector2D &proportinalRect_);

private:
    std::unique_ptr<opengl_drawing::Objects> m_openglData;
};


void makeGeometry(
        float width_, float height_,
        int geomertyPointsWidth_, int geometryPointsHeight_,
        float facedGeometryXCoef_, float facedGeometryYCoef_,
        int geometryVertexCoords_, int geometryTextureCoords_,
        bool hasTextureCoords_,
        bool isGeometrySolid_,
        std::vector<GLfloat> &vertexData_,
        std::vector<GLfloat> &textureData_
        );

void calculatePreserveAspectFitTextureMatrix(
        QMatrix4x4 & textureMatrix_,
        const QSize &imageSize_,
        const QSize &rectSize_
        );


class QuizImageFboRendererImpl
{
public:
    QuizImageFboRendererImpl() = default;

protected:
    void renderImpl();
    QOpenGLFramebufferObject *createFramebufferObjectImpl(const QSize &size_);
    const QVector2D &getWindowSize() const;
    void setWindowSize(const QVector2D &windowSize_);
    void synchronizeImpl(const QVector2D &rectSize_, bool imageDataChanged_, bool sizeChanged_, qreal t_);
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
        const bool sizeChanged = getWindowSize() != rectSize;
        setWindowSize(rectSize);
        const qreal t = quizImage->t();

        const bool imageDataChanged = quizImage->isImageDataUpdated();
        if(imageDataChanged)
        {
            quizImage->setDataToFree(releaseImageData());
            setImageData(quizImage->getData());
        }
        synchronizeImpl(rectSize, imageDataChanged, sizeChanged, t);
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
        virtual void initialize(QuizImageObjects &data_) const override;
    };
}


#endif // QUIZIMAGEOPENGLDRAWINGDATA_H
