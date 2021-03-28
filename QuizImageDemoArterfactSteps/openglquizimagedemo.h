#ifndef OPENGLQUIZIMAGEDEMO_H
#define OPENGLQUIZIMAGEDEMO_H


#include <QSGRenderNode>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLTexture>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLShader>
#include <QQuickItem>
#include <QByteArray>
#include <QImage>
#include <QMatrix4x4>
#include "../MastactvaBase/quizimagedata.h"
//#include "../MastactvaModels/effect.h"
//#include "../MastactvaModels/effectargset.h"


#if QT_CONFIG(opengl)


class OpenGlQuizImageDemo : public QSGRenderNode
{
public:
    OpenGlQuizImageDemo();
    virtual ~OpenGlQuizImageDemo() override;

    virtual void releaseResources() override;
    virtual void render(const RenderState *state_) override;
    virtual StateFlags changedStates() const override;
    virtual RenderingFlags flags() const override;
    virtual QRectF rect() const override;

    void sync(QQuickItem *item_);

private:
    void init(QOpenGLFunctions *f_);
    void paintGL(QOpenGLFunctions *f_, const RenderState *state_);
    void noPaintGL(QOpenGLFunctions *f_, const RenderState *state_);
    void updateRenderArguments();

private:
    OpenGLDrawingImageData *m_drawingData = nullptr;
    int m_left = 0;
    int m_top = 0;
    int m_width = 1;
    int m_height = 1;
    qreal m_t = 0.0;
    QString m_programBuildLog;
};


#endif // #if QT_CONFIG(opengl)
#endif // OPENGLQUIZIMAGEDEMO_H
