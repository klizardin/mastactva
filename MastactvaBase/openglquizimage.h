#ifndef OPENGLQUIZIMAGE_H
#define OPENGLQUIZIMAGE_H


#include <QSGRenderNode>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLTexture>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLShader>
#include <QQuickItem>


#if QT_CONFIG(opengl)


class OpenGlQuizImage : public QSGRenderNode
{
public:
    OpenGlQuizImage();
};


#endif // #if QT_CONFIG(opengl)
#endif // OPENGLQUIZIMAGE_H
