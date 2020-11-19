#ifndef OPENGLQUIZIMAGE_H
#define OPENGLQUIZIMAGE_H

#include <qsgrendernode.h>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLTexture>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLShader>
#include <QQuickItem>

#if QT_CONFIG(opengl)

class OpenGLQuizImageV1 : public QSGRenderNode
{
public:
    OpenGLQuizImageV1(QObject * parent_);
    virtual ~OpenGLQuizImageV1() override;

    virtual void render(const RenderState *state) override;
    virtual void releaseResources() override;
    virtual StateFlags changedStates() const override;
    virtual RenderingFlags flags() const override;
    virtual QRectF rect() const override;

    void sync(QQuickItem *item);

private:
    void makeObject();
    void init(QOpenGLFunctions *f_);
    void paintGL(QOpenGLFunctions *f_);

    int m_width = 0;
    int m_height = 0;
    QObject *m_parent = nullptr;
    QVector<GLfloat> m_vertData;
    QOpenGLTexture *m_fromTexture = nullptr;
    QOpenGLTexture *m_toTexture = nullptr;
    QOpenGLShaderProgram *m_program = nullptr;
    QOpenGLShader *m_vshader = nullptr;
    QOpenGLShader *m_fshader = nullptr;
    QOpenGLBuffer *m_vbo = nullptr;
    QColor m_clearColor = Qt::blue;
    //QPoint m_lastPos;
    int m_xRot = 0;
    int m_yRot = 0;
    int m_zRot = 0;
    int m_vertexAttrId = -1;
    int m_texCoordAttrId = -1;
    int m_textureId = -1;
    int m_matrixId = -1;
};


/*class OpenGLQuizImage : public QSGRenderNode
{
public:
    OpenGLQuizImage(QObject * parent_);
    virtual ~OpenGLQuizImage() override;

    virtual void render(const RenderState *state) override;
    virtual StateFlags changedStates() const override;
    virtual RenderingFlags flags() const override;
    virtual QRectF rect() const override;

    void sync(QQuickItem *item);

protected:
    virtual void releaseResources() override;

private:
    void makeObject();
    void init(QOpenGLFunctions *f_);
    void paintGL(QOpenGLFunctions *f_);

    int m_width = 0;
    int m_height = 0;
    QObject *m_parent = nullptr;
    QVector<GLfloat> m_vertData;
    QOpenGLTexture *m_fromTexture = nullptr;
    QOpenGLTexture *m_toTexture = nullptr;
    QOpenGLShaderProgram *m_program = nullptr;
    QOpenGLShader *m_vshader = nullptr;
    QOpenGLShader *m_fshader = nullptr;
    QOpenGLBuffer *m_vbo = nullptr;
    QColor m_clearColor = Qt::blue;
    int m_xRot = 0;
    int m_yRot = 0;
    int m_zRot = 0;
    int m_vertexAttrId = -1;
    int m_texCoordAttrId = -1;
    int m_textureId = -1;
    int m_matrixId = -1;
};*/


#endif // QT_CONFIG(opengl)
#endif // OPENGLQUIZIMAGE_H
