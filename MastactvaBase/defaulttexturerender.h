#ifndef DEFAULTTEXTURERENDER_H
#define DEFAULTTEXTURERENDER_H


#include <QMatrix4x4>
#include <vector>
#include <memory>


QT_FORWARD_DECLARE_CLASS(QOpenGLContext)
QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)
QT_FORWARD_DECLARE_CLASS(QOpenGLBuffer)
QT_FORWARD_DECLARE_CLASS(QOpenGLVertexArrayObject)
QT_FORWARD_DECLARE_CLASS(QWindow)
QT_FORWARD_DECLARE_CLASS(QOffscreenSurface)


class DefaultTextureRender
{
public:
    DefaultTextureRender(QOffscreenSurface *offscreenSurface);
    ~DefaultTextureRender();

    void resize(int w, int h);
    void render(QWindow *w, QOpenGLContext *share, uint texture);

private:
    void init(QWindow *w, QOpenGLContext *share);
    void setupVertexAttribs();

    /* can be emulated by QWindow
     * https://doc.qt.io/qt-6/qoffscreensurface.html
    */
    QOffscreenSurface *m_offscreenSurface;
    std::unique_ptr<QOpenGLContext> m_context;
    std::unique_ptr<QOpenGLShaderProgram> m_program;
    std::unique_ptr<QOpenGLBuffer> m_vbo;
    std::unique_ptr<QOpenGLVertexArrayObject> m_vao;
    int m_matrixLoc = 0;
    QMatrix4x4 m_proj;
    bool m_last = false;
    std::vector<int> m_viewport;

};

#endif // DEFAULTTEXTURERENDER_H
