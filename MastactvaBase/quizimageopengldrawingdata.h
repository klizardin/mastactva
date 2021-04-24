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
        void setUniform(QOpenGLShaderProgram *program_) const; //, const QString &name_
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

        QColor clearColor() const;

        template<typename ItemType_>
        void setAttribute(const QString &name_, const std::vector<ItemType_> &value_, int tupleSize_ = 0)
        {
            if(!m_imageData.operator bool())
            {
                return;
            }
            m_imageData->setAttribute(name_, value_, tupleSize_);
        }

        int getAttributeTupleSize(const QString &name_) const
        {
            if(!m_imageData.operator bool())
            {
                return 0;
            }
            return m_imageData->getAttributeTupleSize(name_);
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

        bool getTextureSize(const QString &name_, QSize &size_) const
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

        void setTexture(const QString &name_, const QString &newFilename_)
        {
            for(std::unique_ptr<Object> &object_ : m_objects)
            {
                object_->setTexture(name_, newFilename_);
            }
        }

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

    int getAttributeTupleSize(const QString &name_) const
    {
        if(!m_openglData.operator bool())
        {
            return 0;
        }
        return m_openglData->getAttributeTupleSize(name_);
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

    bool getTextureSize(const QString &name_, QSize &size_) const
    {
        if(!m_openglData.operator bool())
        {
            return false;
        }
        return m_openglData->getTextureSize(name_, size_);
    }

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


#endif // QUIZIMAGEOPENGLDRAWINGDATA_H
