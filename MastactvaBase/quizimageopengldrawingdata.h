#ifndef QUIZIMAGEOPENGLDRAWINGDATA_H
#define QUIZIMAGEOPENGLDRAWINGDATA_H


#include <QtGui/QOpenGLFramebufferObject>
#include <QtGui/qopenglshaderprogram.h>
#include <QtGui/qopenglfunctions.h>
#include <QtGui/QOpenGLFramebufferObject>
#include <QtQuick/QQuickWindow>
#include <QQuickFramebufferObject>
#include "../MastactvaBase/quizimagedrawingdata.h"


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
        int m_index = 0;
        int m_location = -1;
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

    protected:
        void setTextureIndexes();

    private:
        std::shared_ptr<drawing_data::QuizImageObject> m_imageData;
        std::unique_ptr<QOpenGLShaderProgram> program;
        QHash<QString, int> attributes;
        QHash<QString, int> uniforms;
        std::map<QString, std::unique_ptr<Texture>> textures;
    };

    class Objects
    {
    public:
        std::unique_ptr<drawing_data::QuizImageObjects> free();
        void init(std::unique_ptr<drawing_data::QuizImageObjects> &&imageData_);
        void draw(QOpenGLFunctions *f_);

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

    private:
        std::unique_ptr<drawing_data::QuizImageObjects> m_imageData;
        std::vector<std::unique_ptr<Object>> m_objects;
    };
}


class ObjectsRenderer : protected QOpenGLFunctions
{
public:
    ObjectsRenderer();
    ~ObjectsRenderer();

    void setImageData(std::unique_ptr<drawing_data::QuizImageObjects> imageData_);
    std::unique_ptr<drawing_data::QuizImageObjects> releaseImageData();
    void render();

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

    int getAttributeTupleSize(const QString &name_) const;
    bool getTextureSize(const QString &name_, QSize &size_) const;

protected:
    void initialize();

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


template<typename QuizImageType_>
class QuizImageFboRenderer : public QQuickFramebufferObject::Renderer
{
public:
    QuizImageFboRenderer() = default;

    void render() override
    {
        m_objectRenderer.render();
    }

    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size_) override
    {
        QOpenGLFramebufferObjectFormat format;
        format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
        format.setSamples(4);
        return new QOpenGLFramebufferObject(size_, format);
    }

    void synchronize(QQuickFramebufferObject *frameBufferObject_) override
    {
        QuizImageType_ *quizImage = static_cast<QuizImageType_ *>(frameBufferObject_);
        if(nullptr == quizImage)
        {
            return;
        }

        const QVector2D rectSize(quizImage->width(), quizImage->height());
        const bool sizeChanged = m_windowSize != rectSize;
        m_windowSize = rectSize;
        const qreal t = quizImage->t();

        const bool imageDataChanged = quizImage->isImageDataUpdated();
        if(imageDataChanged)
        {
            quizImage->setDataToFree(m_objectRenderer.releaseImageData());
            m_objectRenderer.setImageData(quizImage->getData());
        }

        const float sm = std::max(std::max(rectSize.x(), rectSize.y()), 1.0f);
        QVector2D rect(rectSize.x() / sm, rectSize.y() / sm);

        m_objectRenderer.setUniform("renderScreenRect", rect);
        m_objectRenderer.setUniform("renderT", t);
        QMatrix4x4 renderMatrix;
        renderMatrix.ortho(QRectF(0, 0, rect.x(), rect.y()));
        m_objectRenderer.setUniform("renderMatrix", renderMatrix);

        if(!(imageDataChanged || sizeChanged))
        {
            return;
        }

        GLint isSolid = 1;
        m_objectRenderer.getUniform("renderIsGeomertySolid", isSolid);
        QVector2D geometrySize(1.0, 1.0);
        m_objectRenderer.getUniform("renderGeomertySize", geometrySize);
        QVector2D geometryFacedCoef(0.0, 0.0);
        m_objectRenderer.getUniform("renderFacedGeometryCoefs", geometryFacedCoef);

        const int vertexAttributeTupleSize = m_objectRenderer.getAttributeTupleSize("renderVertexAttribute");
        const int textureAttributeTupleSize = m_objectRenderer.getAttributeTupleSize("renderTextureAttribute");
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

        m_objectRenderer.setAttribute("renderVertexAttribute", vertexData, vertexAttributeTupleSize);
        if(textureAttributeTupleSize > 0)
        {
            m_objectRenderer.setAttribute("renderTextureAttribute", textureData, textureAttributeTupleSize);
        }

        QSize windowSize((int)m_windowSize.x(), (int)m_windowSize.y());
        QSize imageSize;
        if(m_objectRenderer.getTextureSize("renderFromImage", imageSize))
        {
            QMatrix4x4 imageTextureMatrix;
            calculatePreserveAspectFitTextureMatrix(imageTextureMatrix, imageSize, windowSize);
            m_objectRenderer.setUniform("renderFromImageMatrix", imageTextureMatrix);
        }
        if(m_objectRenderer.getTextureSize("renderToImage", imageSize))
        {
            QMatrix4x4 imageTextureMatrix;
            calculatePreserveAspectFitTextureMatrix(imageTextureMatrix, imageSize, windowSize);
            m_objectRenderer.setUniform("renderToImageMatrix", imageTextureMatrix);
        }
    }

private:
    ObjectsRenderer m_objectRenderer;
    QVector2D m_windowSize;
};


#endif // QUIZIMAGEOPENGLDRAWINGDATA_H
