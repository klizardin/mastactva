#include "quizimage.h"
#include <math.h>
#include <QtGui/qvector3d.h>
#include <QtGui/qmatrix4x4.h>
#include <QtGui/qopenglshaderprogram.h>
#include <QtGui/qopenglfunctions.h>
#include <QtGui/QOpenGLFramebufferObject>
#include <QtQuick/QQuickWindow>
#include <qsgsimpletexturenode.h>
#include <QRandomGenerator>
#include <QTime>
#include <QVector>
#include "../MastactvaBase/utils.h"


template<typename TargetType_, typename SrcType_>
std::unique_ptr<TargetType_> unique_ptr_static_cast(std::unique_ptr<SrcType_> &&ptr_)
{
    return std::unique_ptr<TargetType_>{static_cast<TargetType_*>(ptr_.release())};
}


namespace drawing_data
{
    class TestMinimalDrawQuizImageObject : public IDefaultData<QuizImageObjects>
    {
    public:
        virtual void initialize(QuizImageObjects &data_) const override;
    };

    class TestMinimal2PassDrawQuizImageObject : public IDefaultData<QuizImageObjects>
    {
    public:
        virtual void initialize(QuizImageObjects &data_) const override;
    };

    class Test0QuizImageObject : public IDefaultData<QuizImageObjects>
    {
    public:
        virtual void initialize(QuizImageObjects &data_) const override;
    };

    class Test1QuizImageObject : public IDefaultData<QuizImageObjects>
    {
    public:
        virtual void initialize(QuizImageObjects &data_) const override;
    };
}


void quad(
        std::vector<QVector3D> &vertices,
        std::vector<QVector3D> &normals,
        qreal x1, qreal y1, qreal x2, qreal y2, qreal x3, qreal y3, qreal x4, qreal y4
        )
{
    vertices.push_back(QVector3D(x1, y1, -0.05f));
    vertices.push_back(QVector3D(x2, y2, -0.05f));
    vertices.push_back(QVector3D(x4, y4, -0.05f));

    vertices.push_back(QVector3D(x3, y3, -0.05f));
    vertices.push_back(QVector3D(x4, y4, -0.05f));
    vertices.push_back(QVector3D(x2, y2, -0.05f));

    QVector3D n = QVector3D::normal
        (QVector3D(x2 - x1, y2 - y1, 0.0f), QVector3D(x4 - x1, y4 - y1, 0.0f));

    normals.push_back(n);
    normals.push_back(n);
    normals.push_back(n);

    normals.push_back(n);
    normals.push_back(n);
    normals.push_back(n);

    vertices.push_back(QVector3D(x4, y4, 0.05f));
    vertices.push_back(QVector3D(x2, y2, 0.05f));
    vertices.push_back(QVector3D(x1, y1, 0.05f));

    vertices.push_back(QVector3D(x2, y2, 0.05f));
    vertices.push_back(QVector3D(x4, y4, 0.05f));
    vertices.push_back(QVector3D(x3, y3, 0.05f));

    n = QVector3D::normal
        (QVector3D(x2 - x4, y2 - y4, 0.0f), QVector3D(x1 - x4, y1 - y4, 0.0f));

    normals.push_back(n);
    normals.push_back(n);
    normals.push_back(n);

    normals.push_back(n);
    normals.push_back(n);
    normals.push_back(n);
}

void extrude(
        std::vector<QVector3D> &vertices,
        std::vector<QVector3D> &normals,
        qreal x1, qreal y1, qreal x2, qreal y2
        )
{
    vertices.push_back(QVector3D(x1, y1, +0.05f));
    vertices.push_back(QVector3D(x2, y2, +0.05f));
    vertices.push_back(QVector3D(x1, y1, -0.05f));

    vertices.push_back(QVector3D(x2, y2, -0.05f));
    vertices.push_back(QVector3D(x1, y1, -0.05f));
    vertices.push_back(QVector3D(x2, y2, +0.05f));

    QVector3D n = QVector3D::normal
        (QVector3D(x2 - x1, y2 - y1, 0.0f), QVector3D(0.0f, 0.0f, -0.1f));

    normals.push_back(n);
    normals.push_back(n);
    normals.push_back(n);

    normals.push_back(n);
    normals.push_back(n);
    normals.push_back(n);
}

void createGeometry(
        std::vector<QVector3D> &vertices,
        std::vector<QVector3D> &normals
        )
{
    qreal x1 = +0.06f;
    qreal y1 = -0.14f;
    qreal x2 = +0.14f;
    qreal y2 = -0.06f;
    qreal x3 = +0.08f;
    qreal y3 = +0.00f;
    qreal x4 = +0.30f;
    qreal y4 = +0.22f;

    quad(vertices, normals, x1, y1, x2, y2, y2, x2, y1, x1);
    quad(vertices, normals, x3, y3, x4, y4, y4, x4, y3, x3);

    extrude(vertices, normals, x1, y1, x2, y2);
    extrude(vertices, normals, x2, y2, y2, x2);
    extrude(vertices, normals, y2, x2, y1, x1);
    extrude(vertices, normals, y1, x1, x1, y1);
    extrude(vertices, normals, x3, y3, x4, y4);
    extrude(vertices, normals, x4, y4, y4, x4);
    extrude(vertices, normals, y4, x4, y3, x3);

    const qreal Pi = M_PI;
    const int NumSectors = 100;

    for (int i = 0; i < NumSectors; ++i)
    {
        qreal angle1 = (i * 2 * Pi) / NumSectors;
        qreal x5 = 0.30 * sin(angle1);
        qreal y5 = 0.30 * cos(angle1);
        qreal x6 = 0.20 * sin(angle1);
        qreal y6 = 0.20 * cos(angle1);

        qreal angle2 = ((i + 1) * 2 * Pi) / NumSectors;
        qreal x7 = 0.20 * sin(angle2);
        qreal y7 = 0.20 * cos(angle2);
        qreal x8 = 0.30 * sin(angle2);
        qreal y8 = 0.30 * cos(angle2);

        quad(vertices, normals, x5, y5, x6, y6, x7, y7, x8, y8);

        extrude(vertices, normals, x6, y6, x7, y7);
        extrude(vertices, normals, x8, y8, x5, y5);
    }

    for (int i = 0; i < (int)vertices.size();i++)
        vertices[i] *= 2.0f;
}

void drawing_data::TestMinimalDrawQuizImageObject::initialize(
        QuizImageObjects &data_
        ) const
{
    std::unique_ptr<QuizImageObject> object(new QuizImageObject());

    const char *vsrc1 =
        "attribute highp vec4 vertex;\n"
        "attribute mediump vec3 normal;\n"
        "uniform mediump mat4 matrix;\n"
        "varying mediump vec4 color;\n"
        "void main(void)\n"
        "{\n"
        "    vec3 toLight = normalize(vec3(0.0, 0.3, 1.0));\n"
        "    float angle = max(dot(normal, toLight), 0.0);\n"
        "    vec3 col = vec3(0.40, 1.0, 0.0);\n"
        "    color = vec4(col * 0.2 + col * 0.8 * angle, 1.0);\n"
        "    color = clamp(color, 0.0, 1.0);\n"
        "    gl_Position = matrix * vertex;\n"
        "}\n";

    const char *fsrc1 =
        "varying mediump vec4 color;\n"
        "void main(void)\n"
        "{\n"
        "    gl_FragColor = color;\n"
        "}\n";

    object->vertexShader = vsrc1;
    object->fragmentShader = fsrc1;

    std::shared_ptr<std::vector<QVector3D>> vertices(new std::vector<QVector3D>());
    std::shared_ptr<std::vector<QVector3D>> normals(new std::vector<QVector3D>());
    createGeometry(*vertices.get(), *normals.get());

    object->attributes.push_back(
                std::unique_ptr<drawing_data::IAttribute>(
                    new drawing_data::Attribute<QVector3D>{ "vertex", vertices }
                    )
                );
    object->attributes.push_back(
                std::unique_ptr<drawing_data::IAttribute>(
                    new drawing_data::Attribute<QVector3D>{ "normal", normals }
                    )
                );

    QRandomGenerator gen;
    qreal fScale = 1;
    qreal fAngle = gen.generateDouble() * 360.0;
    std::shared_ptr<QMatrix4x4> modelview(new QMatrix4x4);
    modelview->rotate(fAngle, 0.0f, 1.0f, 0.0f);
    modelview->rotate(fAngle, 1.0f, 0.0f, 0.0f);
    modelview->rotate(fAngle, 0.0f, 0.0f, 1.0f);
    modelview->scale(fScale);
    modelview->translate(0.0f, -0.2f, 0.0f);

    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<QMatrix4x4>{ "matrix", modelview }
                ));
    data_.objects.push_back(std::move(object));
}


void drawing_data::TestMinimal2PassDrawQuizImageObject::initialize(
        QuizImageObjects &data_
        ) const
{
    std::unique_ptr<QuizImageObject> object1(new QuizImageObject());
    std::unique_ptr<QuizImageObject> object2(new QuizImageObject());

    const char *vsrc1 =
        "attribute highp vec4 vertex;\n"
        "attribute mediump vec3 normal;\n"
        "uniform mediump mat4 matrix;\n"
        "varying mediump vec4 color;\n"
        "void main(void)\n"
        "{\n"
        "    vec3 toLight = normalize(vec3(0.0, 0.3, 1.0));\n"
        "    float angle = max(dot(normal, toLight), 0.0);\n"
        "    vec3 col = vec3(0.40, 1.0, 0.0);\n"
        "    color = vec4(col * 0.2 + col * 0.8 * angle, 1.0);\n"
        "    color = clamp(color, 0.0, 1.0);\n"
        "    gl_Position = matrix * vertex;\n"
        "}\n";

    const char *fsrc1 =
        "varying mediump vec4 color;\n"
        "void main(void)\n"
        "{\n"
        "    gl_FragColor = color;\n"
        "}\n";

    QRandomGenerator gen;

    object1->vertexShader = vsrc1;
    object1->fragmentShader = fsrc1;
    object2->vertexShader = vsrc1;
    object2->fragmentShader = fsrc1;

    std::shared_ptr<std::vector<QVector3D>> vertices(new std::vector<QVector3D>());
    std::shared_ptr<std::vector<QVector3D>> normals(new std::vector<QVector3D>());
    createGeometry(*vertices.get(), *normals.get());

    object1->attributes.push_back(
                std::unique_ptr<drawing_data::IAttribute>(
                    new drawing_data::Attribute<QVector3D>{ "vertex", vertices }
                    )
                );
    object1->attributes.push_back(
                std::unique_ptr<drawing_data::IAttribute>(
                    new drawing_data::Attribute<QVector3D>{ "normal", normals }
                    )
                );
    object2->attributes.push_back(
                std::unique_ptr<drawing_data::IAttribute>(
                    new drawing_data::Attribute<QVector3D>{ "vertex", vertices }
                    )
                );
    object2->attributes.push_back(
                std::unique_ptr<drawing_data::IAttribute>(
                    new drawing_data::Attribute<QVector3D>{ "normal", normals }
                    )
                );

    qreal fScale = 1;
    qreal fAngle1 = gen.generateDouble() * 360.0;
    std::shared_ptr<QMatrix4x4> modelview1(new QMatrix4x4);
    modelview1->rotate(fAngle1, 0.0f, 1.0f, 0.0f);
    modelview1->rotate(fAngle1, 1.0f, 0.0f, 0.0f);
    modelview1->rotate(fAngle1, 0.0f, 0.0f, 1.0f);
    modelview1->scale(fScale);
    modelview1->translate(0.0f, -0.2f, 0.0f);

    qreal fAngle2 = gen.generateDouble() * 360.0;
    std::shared_ptr<QMatrix4x4> modelview2(new QMatrix4x4);
    modelview2->rotate(fAngle2, 0.0f, 1.0f, 0.0f);
    modelview2->rotate(fAngle2, 1.0f, 0.0f, 0.0f);
    modelview2->rotate(fAngle2, 0.0f, 0.0f, 1.0f);
    modelview2->scale(fScale);
    modelview2->translate(0.0f, -0.2f, 0.0f);

    object1->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<QMatrix4x4>{ "matrix", modelview1 }
                ));
    object2->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<QMatrix4x4>{ "matrix", modelview2 }
                ));

    data_.objects.push_back(std::move(object1));
    data_.objects.push_back(std::move(object2));
}


void drawing_data::Test0QuizImageObject::initialize(
        QuizImageObjects &data_
        ) const
{
    std::unique_ptr<QuizImageObject> object(new QuizImageObject());

    static QByteArray vertex = loadTextFile(":/Shaders/Shaders/test000/default.vsh").toUtf8();
    static QByteArray fragment = loadTextFile(":/Shaders/Shaders/test000/default.fsh").toUtf8();
    object->vertexShader = vertex.constData();
    object->fragmentShader = fragment.constData();

    object->textures = {
        {"renderFromImage", ":/Images/Images/test001/0001.png"},
        {"renderToImage", ":/Images/Images/test001/0002.png"},
    };

    std::shared_ptr<std::vector<QVector4D>> vertices(new std::vector<QVector4D>());
    std::shared_ptr<std::vector<QVector4D>> textures(new std::vector<QVector4D>());

    object->attributes.push_back(
                std::unique_ptr<drawing_data::IAttribute>(
                    new drawing_data::Attribute<QVector4D>{ "renderVertexAttribute", vertices }
                    )
                );
    object->attributes.push_back(
                std::unique_ptr<drawing_data::IAttribute>(
                    new drawing_data::Attribute<QVector4D>{ "renderTextureAttribute", textures }
                    )
                );

    std::shared_ptr<QMatrix4x4> renderMatrix(new QMatrix4x4);
    renderMatrix->ortho(QRectF(0, 0, 1.0, 1.0));
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<QMatrix4x4>{ "renderMatrix", renderMatrix }
                ));
    std::shared_ptr<QMatrix4x4> renderFromImageMatrix(new QMatrix4x4);
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<QMatrix4x4>{ "renderFromImageMatrix", renderFromImageMatrix }
                ));
    std::shared_ptr<QMatrix4x4> renderToImageMatrix(new QMatrix4x4);
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<QMatrix4x4>{ "renderToImageMatrix", renderToImageMatrix }
                ));
    std::shared_ptr<QVector2D> renderScreenRect(new QVector2D());
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<QVector2D>{ "renderScreenRect", renderScreenRect }
                ));

    std::shared_ptr<GLfloat> renderT(new GLfloat{0.5});
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<GLfloat>{ "renderT", renderT }
                ));

    std::shared_ptr<GLfloat> renderOpacity(new GLfloat{1.0});
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<GLfloat>{ "renderOpacity", renderOpacity }
                ));

    std::shared_ptr<QVector2D> renderFacedGeometryCoefs(new QVector2D(0.0, 0.0));
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<QVector2D>{ "renderFacedGeometryCoefs", renderFacedGeometryCoefs }
                ));

    std::shared_ptr<QVector2D> renderGeomertySize(new QVector2D(2.0, 2.0));
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<QVector2D>{ "renderGeomertySize", renderGeomertySize }
                ));

    std::shared_ptr<GLint> renderIsGeomertySolid(new GLint{1});
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<GLint>{ "renderIsGeomertySolid", renderIsGeomertySolid }
                ));

    data_.objects.push_back(std::move(object));
}

void drawing_data::Test1QuizImageObject::initialize(
        QuizImageObjects &data_
        ) const
{
    std::unique_ptr<QuizImageObject> object(new QuizImageObject());

    static QByteArray vertex = loadTextFile(":/Shaders/Shaders/test001/cube_rotation_horizontal_faced.vsh").toUtf8();
    static QByteArray fragment = loadTextFile(":/Shaders/Shaders/test001/default_view_two_images_mix_both.fsh").toUtf8();
    object->vertexShader = vertex.constData();
    object->fragmentShader = fragment.constData();

    object->textures = {
        {"renderFromImage", ":/Images/Images/test001/0001.png"},
        {"renderToImage", ":/Images/Images/test001/0002.png"},
    };

    std::shared_ptr<std::vector<QVector4D>> vertices(new std::vector<QVector4D>());
    std::shared_ptr<std::vector<QVector4D>> textures(new std::vector<QVector4D>());

    object->attributes.push_back(
                std::unique_ptr<drawing_data::IAttribute>(
                    new drawing_data::Attribute<QVector4D>{ "renderVertexAttribute", vertices }
                    )
                );
    object->attributes.push_back(
                std::unique_ptr<drawing_data::IAttribute>(
                    new drawing_data::Attribute<QVector4D>{ "renderTextureAttribute", textures }
                    )
                );

    std::shared_ptr<QMatrix4x4> renderMatrix(new QMatrix4x4);
    renderMatrix->ortho(QRectF(0, 0, 1.0, 1.0));
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<QMatrix4x4>{ "renderMatrix", renderMatrix }
                ));
    std::shared_ptr<QMatrix4x4> renderFromImageMatrix(new QMatrix4x4);
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<QMatrix4x4>{ "renderFromImageMatrix", renderFromImageMatrix }
                ));
    std::shared_ptr<QMatrix4x4> renderToImageMatrix(new QMatrix4x4);
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<QMatrix4x4>{ "renderToImageMatrix", renderToImageMatrix }
                ));
    std::shared_ptr<QVector2D> renderScreenRect(new QVector2D());
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<QVector2D>{ "renderScreenRect", renderScreenRect }
                ));

    QRandomGenerator gen;
    std::shared_ptr<QVector4D> faceRotateDir(new QVector4D());
    faceRotateDir->setX(gen.generateDouble());
    faceRotateDir->setY(gen.generateDouble());
    faceRotateDir->setZ(gen.generateDouble());
    faceRotateDir->setW(gen.generateDouble());
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<QVector4D>{ "faceRotateDir", faceRotateDir }
                ));

    std::shared_ptr<GLfloat> renderT(new GLfloat);
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<GLfloat>{ "renderT", renderT }
                ));

    std::shared_ptr<GLfloat> renderOpacity(new GLfloat{1.0});
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<GLfloat>{ "renderOpacity", renderOpacity }
                ));

    std::shared_ptr<QVector2D> renderFacedGeometryCoefs(new QVector2D(0.0, 1.0e-3));
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<QVector2D>{ "renderFacedGeometryCoefs", renderFacedGeometryCoefs }
                ));

    std::shared_ptr<QVector2D> renderGeomertySize(new QVector2D(2.0, 4.0));
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<QVector2D>{ "renderGeomertySize", renderGeomertySize }
                ));

    std::shared_ptr<GLint> renderIsGeomertySolid(new GLint);
    *renderIsGeomertySolid = 0;
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<GLint>{ "renderIsGeomertySolid", renderIsGeomertySolid }
                ));

    data_.objects.push_back(std::move(object));
}


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

void opengl_drawing::Texture::setUniform(QOpenGLShaderProgram *program_) const //, const QString &name_
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
    program->addCacheableShaderFromSourceCode(QOpenGLShader::Vertex, m_imageData->vertexShader.constData());
    program->addCacheableShaderFromSourceCode(QOpenGLShader::Fragment, m_imageData->fragmentShader.constData());
    program->link();

    for(const auto &attribute : m_imageData->attributes)
    {
        if(!attribute.operator bool())
        {
            continue;
        }
        attributes[attribute->name()] = program->attributeLocation(attribute->name());
    }

    for(const auto &uniform : m_imageData->uniforms)
    {
        if(!uniform.operator bool())
        {
            continue;
        }
        uniforms[uniform->name()] = program->uniformLocation(uniform->name());
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
        texture->setLocation(program->uniformLocation(texture_.name));
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
        if(attribute < 0) { continue; }
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
        if(attribute < 0) { continue; }
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
        if(attributeId < 0) { continue; }
        program->setAttributeArray(attributeId, attribute->constData(), attribute->tupleSize());
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
       return left_.operator bool() && right_.operator bool() && left_->size() < right_->size();
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

void opengl_drawing::Object::setTexture(const QString &name_, const QString& newFilename_)
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

bool opengl_drawing::Object::getTextureSize(const QString &name_, QSize &imageSize_) const
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

void opengl_drawing::Objects::init(std::unique_ptr<drawing_data::QuizImageObjects> &&imageData_)
{
    m_imageData = std::move(imageData_);
    m_objects.reserve(m_imageData->objects.size());
    for(const auto &object : m_imageData->objects)
    {
        m_objects.push_back(std::unique_ptr<opengl_drawing::Object>(new opengl_drawing::Object()));
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
    if(!m_openglData.operator bool()) { return {nullptr}; }
    return m_openglData->free();
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

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
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
        std::vector<GLfloat> &textureData_,
        std::vector<GLint> &indexesData_
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
                        vertexData_[offs0 + 2] = -0.1;
                    }
                    if(geometryVertexCoords_ >= 4)
                    {
                        vertexData_[offs0 + 3] = 1.0;
                    }

                    // texture coordinate
                    if(hasTextureCoords_)
                    {
                        textureData_[offs1 + 0] = (GLfloat)(x + coords[j][k][0])/(GLfloat)geomertyPointsWidth_;
                        textureData_[offs1 + 1] = (GLfloat)(y + coords[j][k][1])/(GLfloat)geometryPointsHeight_;
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


class QuizImageFboRenderer : public QQuickFramebufferObject::Renderer
{
public:
    QuizImageFboRenderer()
    {
    }

    void render() override
    {
        objectRenderer.render();
        //update();
    }

    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override
    {
        QOpenGLFramebufferObjectFormat format;
        format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
        format.setSamples(4);
        return new QOpenGLFramebufferObject(size, format);
    }

    virtual void synchronize(QQuickFramebufferObject *frameBufferObject_) override
    {
        QuizImage *quizImage = static_cast<QuizImage *>(frameBufferObject_);
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
            quizImage->setDataToFree(objectRenderer.releaseImageData());
            objectRenderer.setImageData(quizImage->getData());
        }

        objectRenderer.setUniform("renderScreenRect", m_windowSize);
        objectRenderer.setUniform("renderT", t);
        if(!(imageDataChanged || sizeChanged))
        {
            return;
        }

        GLint isSolid = 1;
        objectRenderer.getUniform("renderIsGeomertySolid", isSolid);
        QVector2D geometrySize(1.0, 1.0);
        objectRenderer.getUniform("renderGeomertySize", geometrySize);
        QVector2D geometryFacedCoef(1.0, 1.0);
        objectRenderer.getUniform("renderFacedGeometryCoefs", geometryFacedCoef);

        const int vertexAttributeTupleSize = objectRenderer.getAttributeTupleSize("renderVertexAttribute");
        const int textureAttributeTupleSize = objectRenderer.getAttributeTupleSize("renderTextureAttribute");
        if(vertexAttributeTupleSize <= 0)
        {
            return;
        }

        std::vector<GLfloat> vertexData;
        std::vector<GLfloat> textureData;
        std::vector<GLint> indexesData;

        makeGeometry(1.0, 1.0, //m_windowSize.x(), m_windowSize.y(),
                     (int)geometrySize.x(), (int)geometrySize.y(),
                     geometryFacedCoef.x(), geometryFacedCoef.y(),
                     vertexAttributeTupleSize,
                     textureAttributeTupleSize, textureAttributeTupleSize > 0,
                     0 != isSolid,
                     vertexData, textureData, indexesData);

        objectRenderer.setAttribute("renderVertexAttribute", vertexData, vertexAttributeTupleSize);
        if(textureAttributeTupleSize > 0)
        {
            objectRenderer.setAttribute("renderTextureAttribute", textureData, textureAttributeTupleSize);
        }

        QSize windowSize((int)m_windowSize.x(), (int)m_windowSize.y());
        QSize imageSize;
        if(objectRenderer.getTextureSize("renderFromImage", imageSize))
        {
            QMatrix4x4 imageTextureMatrix;
            calculatePreserveAspectFitTextureMatrix(imageTextureMatrix, imageSize, windowSize);
            objectRenderer.setUniform("renderFromImageMatrix", imageTextureMatrix);
        }
        if(objectRenderer.getTextureSize("renderToImage", imageSize))
        {
            QMatrix4x4 imageTextureMatrix;
            calculatePreserveAspectFitTextureMatrix(imageTextureMatrix, imageSize, windowSize);
            objectRenderer.setUniform("renderToImageMatrix", imageTextureMatrix);
        }
    }

    ObjectsRenderer objectRenderer;
    QVector2D m_windowSize;
};


QuizImage::QuizImage()
{
    initDefaultDrawingData();
}

QQuickFramebufferObject::Renderer *QuizImage::createRenderer() const
{
    return new QuizImageFboRenderer();
}

void QuizImage::updateState()
{
    update();
}

void QuizImage::updateProject()
{
    loadProject();
}

qreal QuizImage::t() const
{
    return m_t;
}

void QuizImage::setT(const qreal &t_)
{
    m_t = t_;

    updateState();
    emit tChanged();
}

QString QuizImage::fromImage() const
{
    return m_fromImage;
}

void QuizImage::setFromImage(const QString &fromImage_)
{
    m_fromImage = fromImage_;
    loadProject();

    updateState();
    emit fromImageChanged();
}

QString QuizImage::toImage() const
{
    return m_toImage;
}

void QuizImage::setToImage(const QString &toImage_)
{
    m_toImage = toImage_;
    loadProject();

    updateState();
    emit toImageChanged();
}

QString QuizImage::project() const
{
    return m_project;
}

void QuizImage::setProject(const QString &project_)
{
    m_project = project_;

    loadProject();
    updateState();
    emit projectChanged();
}

QString QuizImage::log() const
{
    return m_compilerLog;
}

void QuizImage::setLog(const QString &log_)
{
    m_compilerLog = log_;

    emit logChanged();
}

void QuizImage::freeProject()
{
    m_drawingData.reset(nullptr);
    m_drawingOldData.reset(nullptr);
}

void QuizImage::loadProject()
{
    freeProject();
    if(!m_project.isEmpty())
    {
        QUrl url(project());
        initDefaultDrawingData();
        //m_drawingData = OpenGLDrawingImageData::fromJson(
        //            QJsonDocument::fromJson(
        //                loadBinaryFile(url.toLocalFile())
        //                )
        //            );
    }
    setProjectFromImage();
    setProjectToImage();
}

void QuizImage::setProjectFromImage()
{
    if(nullptr == m_drawingData ||
            fromImage().isEmpty()
            ) { return; }
    // do not remove prev image
    QUrl url(fromImage());
    //m_drawingData->addRenderImage(url.toLocalFile(), true);
}

void QuizImage::setProjectToImage()
{
    if(nullptr == m_drawingData ||
            toImage().isEmpty()
            ) { return; }
    // do not remove prev image
    QUrl url(toImage());
    //m_drawingData->addRenderImage(url.toLocalFile(), false);
}

bool QuizImage::isImageDataUpdated() const
{
    return m_drawingData.operator bool();
}

std::unique_ptr<drawing_data::QuizImageObjects> QuizImage::getData()
{
    return std::move(m_drawingData);
}

void QuizImage::setDataToFree(std::unique_ptr<drawing_data::QuizImageObjects> &&old_)
{
    m_drawingOldData = std::move(old_);
}

void QuizImage::renderBuildError(const QString &compilerLog_)
{
    setLog(compilerLog_);
}

void QuizImage::initDefaultDrawingData()
{
    std::unique_ptr<drawing_data::QuizImageObjects> data(new drawing_data::QuizImageObjects());
    drawing_data::Test0QuizImageObject defaultData;
    defaultData.initialize(*data.get());
    m_drawingData = std::move(data);
}
