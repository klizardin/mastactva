/*
    Copyright 2022

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
