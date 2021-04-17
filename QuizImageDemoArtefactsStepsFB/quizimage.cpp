#include "quizimage.h"
#include <math.h>
#include <QtGui/qvector3d.h>
#include <QtGui/qmatrix4x4.h>
#include <QtGui/qopenglshaderprogram.h>
#include <QtGui/qopenglfunctions.h>
#include <QtGui/QOpenGLFramebufferObject>
#include <QtQuick/QQuickWindow>
#include <qsgsimpletexturenode.h>
#include <QTime>
#include <QVector>


template<typename TargetType_, typename SrcType_>
std::unique_ptr<TargetType_> unique_ptr_static_cast(std::unique_ptr<SrcType_> &&ptr_)
{
    return std::unique_ptr<TargetType_>{static_cast<TargetType_*>(ptr_.release())};
}


void quad(
        QVector<QVector3D> &vertices,
        QVector<QVector3D> &normals,
        qreal x1, qreal y1, qreal x2, qreal y2, qreal x3, qreal y3, qreal x4, qreal y4
        )
{
    vertices << QVector3D(x1, y1, -0.05f);
    vertices << QVector3D(x2, y2, -0.05f);
    vertices << QVector3D(x4, y4, -0.05f);

    vertices << QVector3D(x3, y3, -0.05f);
    vertices << QVector3D(x4, y4, -0.05f);
    vertices << QVector3D(x2, y2, -0.05f);

    QVector3D n = QVector3D::normal
        (QVector3D(x2 - x1, y2 - y1, 0.0f), QVector3D(x4 - x1, y4 - y1, 0.0f));

    normals << n;
    normals << n;
    normals << n;

    normals << n;
    normals << n;
    normals << n;

    vertices << QVector3D(x4, y4, 0.05f);
    vertices << QVector3D(x2, y2, 0.05f);
    vertices << QVector3D(x1, y1, 0.05f);

    vertices << QVector3D(x2, y2, 0.05f);
    vertices << QVector3D(x4, y4, 0.05f);
    vertices << QVector3D(x3, y3, 0.05f);

    n = QVector3D::normal
        (QVector3D(x2 - x4, y2 - y4, 0.0f), QVector3D(x1 - x4, y1 - y4, 0.0f));

    normals << n;
    normals << n;
    normals << n;

    normals << n;
    normals << n;
    normals << n;
}

void extrude(
        QVector<QVector3D> &vertices,
        QVector<QVector3D> &normals,
        qreal x1, qreal y1, qreal x2, qreal y2
        )
{
    vertices << QVector3D(x1, y1, +0.05f);
    vertices << QVector3D(x2, y2, +0.05f);
    vertices << QVector3D(x1, y1, -0.05f);

    vertices << QVector3D(x2, y2, -0.05f);
    vertices << QVector3D(x1, y1, -0.05f);
    vertices << QVector3D(x2, y2, +0.05f);

    QVector3D n = QVector3D::normal
        (QVector3D(x2 - x1, y2 - y1, 0.0f), QVector3D(0.0f, 0.0f, -0.1f));

    normals << n;
    normals << n;
    normals << n;

    normals << n;
    normals << n;
    normals << n;
}

void createGeometry(
        QVector<QVector3D> &vertices,
        QVector<QVector3D> &normals
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

    for (int i = 0; i < NumSectors; ++i) {
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

    for (int i = 0;i < vertices.size();i++)
        vertices[i] *= 2.0f;
}

void drawing_data::DefaultQuizImageObject::initialize()
{
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

    vertexShader = vsrc1;
    fragmentShader = fsrc1;

    QVector<QVector3D> vertices;
    QVector<QVector3D> normals;
    createGeometry(vertices, normals);

    attributes.append(
                {{"vertex", std::move(vertices) },
                 {"normal", std::move(normals) }
                });
}


namespace opengl_drawing
{
    class Object
    {
    public:
        void free();
        void init(const std::unique_ptr<drawing_data::QuizImageObject> &object_);
        void enableAttributes();
        void disableAttributes();
        void setAttributeArray(const std::unique_ptr<drawing_data::QuizImageObject> &object_);
        void drawTriangles(
                const std::unique_ptr<drawing_data::QuizImageObject> &object_,
                QOpenGLFunctions *f_
                );

        std::unique_ptr<QOpenGLShaderProgram> program;

    private:
        QHash<QString, int> attributes;
    };
}


void opengl_drawing::Object::free()
{
    program.reset();
}

void opengl_drawing::Object::init(
        const std::unique_ptr<drawing_data::QuizImageObject> &object_
        )
{
    free();
    program.reset(new QOpenGLShaderProgram);
    program->addCacheableShaderFromSourceCode(QOpenGLShader::Vertex, object_->vertexShader.constData());
    program->addCacheableShaderFromSourceCode(QOpenGLShader::Fragment, object_->fragmentShader.constData());
    program->link();

    for(const auto &attribute : object_->attributes)
    {
        attributes[attribute.name] = program->attributeLocation(attribute.name);
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

void opengl_drawing::Object::setAttributeArray(
        const std::unique_ptr<drawing_data::QuizImageObject> &object_
        )
{
    if(!program.operator bool())
    {
        return;
    }
    for(const auto &attribute : object_->attributes)
    {
        const auto attributeId = attributes[attribute.name];
        if(attributeId < 0) { continue; }
        program->setAttributeArray(attributeId, attribute.data.constData());
    }
}

void opengl_drawing::Object::drawTriangles(
        const std::unique_ptr<drawing_data::QuizImageObject> &object_,
        QOpenGLFunctions *f_
        )
{
    if(!program.operator bool()
            || object_->attributes.isEmpty()
            || nullptr == f_
            )
    {
        return;
    }
    const auto fit = std::min_element(
                std::begin(object_->attributes),
                std::end(object_->attributes),
                [](const drawing_data::Attribute &left_, const drawing_data::Attribute &right_)->bool
    {
       return  left_.data.size() < right_.data.size();
    });
    f_->glDrawArrays(GL_TRIANGLES, 0, fit->data.size());
}


class ObjectsRenderer : protected QOpenGLFunctions
{
public:
    ObjectsRenderer();
    ~ObjectsRenderer();

    void render();
    void release();
    void initialize();
    void setImageData(std::unique_ptr<drawing_data::QuizImageObject> imageData_);
    std::unique_ptr<drawing_data::QuizImageObject> releaseImageData();

private:
    qreal   m_fAngle;
    qreal   m_fScale;

    void paintQtLogo();

    int matrixUniform1;

    std::unique_ptr<drawing_data::QuizImageObject> m_imageData;
    std::unique_ptr<opengl_drawing::Object> m_openglData;
};


ObjectsRenderer::ObjectsRenderer()
{
}

ObjectsRenderer::~ObjectsRenderer()
{
    release();
}

void ObjectsRenderer::setImageData(
        std::unique_ptr<drawing_data::QuizImageObject> imageData_
        )
{
    m_imageData = std::move(imageData_);
}

std::unique_ptr<drawing_data::QuizImageObject> ObjectsRenderer::releaseImageData()
{
    return std::move(m_imageData);
}

void ObjectsRenderer::release()
{
    if(!m_openglData.operator bool())
    {
        return;
    }
    m_openglData->free();
}

void ObjectsRenderer::initialize()
{
    initializeOpenGLFunctions();

    if(!m_imageData.operator bool())
    {
        return;
    }

    m_openglData.reset(new opengl_drawing::Object());
    m_openglData->init(m_imageData);

    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);

    matrixUniform1 = m_openglData->program->uniformLocation("matrix");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    m_fAngle = 0;
    m_fScale = 1;
}

void ObjectsRenderer::paintQtLogo()
{
    if(!m_openglData.operator bool())
    {
        return;
    }

    m_openglData->enableAttributes();
    m_openglData->setAttributeArray(m_imageData);
    m_openglData->drawTriangles(m_imageData, this);
    m_openglData->disableAttributes();
}

void ObjectsRenderer::render()
{
    glDepthMask(true);

    glClearColor(0.5f, 0.5f, 0.7f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    glFrontFace(GL_CW);
    glCullFace(GL_FRONT);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    QMatrix4x4 modelview;
    modelview.rotate(m_fAngle, 0.0f, 1.0f, 0.0f);
    modelview.rotate(m_fAngle, 1.0f, 0.0f, 0.0f);
    modelview.rotate(m_fAngle, 0.0f, 0.0f, 1.0f);
    modelview.scale(m_fScale);
    modelview.translate(0.0f, -0.2f, 0.0f);

    m_openglData->program->bind();
    m_openglData->program->setUniformValue(matrixUniform1, modelview);
    paintQtLogo();
    m_openglData->program->release();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    m_fAngle += 1.0f;
}


class QuizImageFboRenderer : public QQuickFramebufferObject::Renderer
{
public:
    QuizImageFboRenderer()
    {
        objectRenderer.initialize();
    }

    void render() override
    {
        objectRenderer.render();
        update();
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
        if(nullptr == quizImage) { return; }
        if(quizImage->isImageDataUpdated())
        {
            quizImage->setDataToFree(objectRenderer.releaseImageData());
            objectRenderer.setImageData(quizImage->getData());
            objectRenderer.release();
            objectRenderer.initialize();
        }
    }

    ObjectsRenderer objectRenderer;
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

std::unique_ptr<drawing_data::QuizImageObject> QuizImage::getData()
{
    return std::move(m_drawingData);
}

void QuizImage::setDataToFree(std::unique_ptr<drawing_data::QuizImageObject> &&old_)
{
    m_drawingOldData = std::move(old_);
}

void QuizImage::renderBuildError(const QString &compilerLog_)
{
    setLog(compilerLog_);
}

void QuizImage::initDefaultDrawingData()
{
    std::unique_ptr<drawing_data::DefaultQuizImageObject> defaultImageData(
                new drawing_data::DefaultQuizImageObject()
                );
    defaultImageData->initialize();
    m_drawingData = unique_ptr_static_cast<drawing_data::QuizImageObject>(std::move(defaultImageData));
}
