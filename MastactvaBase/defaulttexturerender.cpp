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

#include "defaulttexturerender.h"
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOffscreenSurface>
#include <QWindow>


DefaultTextureRender::DefaultTextureRender(QOffscreenSurface *offscreenSurface)
    : m_offscreenSurface(offscreenSurface)
{
}

DefaultTextureRender::~DefaultTextureRender()
{
    // Use a temporary offscreen surface to do the cleanup.
    // There may not be a native window surface available anymore at this stage.
    m_context->makeCurrent(m_offscreenSurface);

    m_program.reset();
    m_vbo.reset();
    m_vao.reset();

    m_context->doneCurrent();
    m_context.reset();
}

// data
// default vertex shader
static const char *g_vertexShaderSource =
    "attribute highp vec4 vertex;\n"
    "attribute lowp vec2 coord;\n"
    "varying lowp vec2 v_coord;\n"
    "uniform highp mat4 matrix;\n"
    "void main() {\n"
    "   v_coord = coord;\n"
    "   gl_Position = matrix * vertex;\n"
    "}\n";
// default fragment shader
static const char *g_fragmentShaderSource =
    "varying lowp vec2 v_coord;\n"
    "uniform sampler2D sampler;\n"
    "void main() {\n"
    "   gl_FragColor = vec4(texture2D(sampler, v_coord).rgb, 1.0);\n"
    "}\n";

// vertexes and texture coordinates
const int g_vertexCount = 6;
const GLfloat g_vertexes[g_vertexCount * 3] = {
    -1.0, 1.0, 1.0,     1.0,-1.0,1.0,       -1.0,-1.0,1.0,
    1.0, -1.0, 1.0,     -1.0,1.0,1.0,       1.0,1.0,1.0,
};
const GLfloat g_textureCoords[g_vertexCount * 2] = {
    0.0f, 0.0f,         1.0f, 1.0f,         1.0f, 0.0f,
    1.0f, 1.0f,         0.0f, 0.0f,         0.0f, 1.0f,
};

void DefaultTextureRender::init(QWindow *w, QOpenGLContext *share)
{
    m_context = std::make_unique<QOpenGLContext>();
    m_context->setShareContext(share);
    m_context->setFormat(w->requestedFormat());
    m_context->create();
    if (!m_context->makeCurrent(w))
    {
        return;
    }

    QOpenGLFunctions *f = m_context->functions();
    f->glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    m_viewport.push_back(0);
    m_viewport.push_back(0);
    m_viewport.push_back(w->width() * w->devicePixelRatio());
    m_viewport.push_back(w->height() * w->devicePixelRatio());

    m_program = std::make_unique<QOpenGLShaderProgram>();
    m_program->addCacheableShaderFromSourceCode(QOpenGLShader::Vertex, g_vertexShaderSource);
    m_program->addCacheableShaderFromSourceCode(QOpenGLShader::Fragment, g_fragmentShaderSource);
    m_program->bindAttributeLocation("vertex", 0);
    m_program->bindAttributeLocation("coord", 1);
    m_program->link();
    m_matrixLoc = m_program->uniformLocation("matrix");

    m_vao = std::make_unique<QOpenGLVertexArrayObject>();
    m_vao->create();
    QOpenGLVertexArrayObject::Binder vaoBinder(m_vao.get());

    m_vbo = std::make_unique<QOpenGLBuffer>();
    m_vbo->create();
    m_vbo->bind();

    m_vbo->allocate(sizeof(GLfloat) * g_vertexCount * 5);
    m_vbo->write(0, g_vertexes, sizeof(GLfloat) * g_vertexCount * 3);
    m_vbo->write(sizeof(GLfloat) * g_vertexCount * 3, g_textureCoords, sizeof(GLfloat) * g_vertexCount * 2);
    m_vbo->release();

    if (m_vao->isCreated())
    {
        setupVertexAttribs();
    }
}

void DefaultTextureRender::resize(int w, int h)
{
    m_proj.setToIdentity();
    m_proj.perspective(45, w / float(h), 0.01f, 100.0f);
}

void DefaultTextureRender::setupVertexAttribs()
{
    m_vbo->bind();
    m_program->enableAttributeArray(0);
    m_program->enableAttributeArray(1);
    m_context->functions()->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    m_context->functions()->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0,
                                                  (const void *)(g_vertexCount * 3 * sizeof(GLfloat)));
    m_vbo->release();
}

void DefaultTextureRender::render(QWindow *w, QOpenGLContext *share, const std::vector<uint> &textures)
{
    if (!m_context)
    {
        init(w, share);
    }

    if (!m_context->makeCurrent(w))
    {
        return;
    }

    QOpenGLFunctions *f = m_context->functions();
    f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!textures.empty() && textures[0])
    {
        f->glBindTexture(GL_TEXTURE_2D, textures[0]);
        f->glFrontFace(GL_CW); // because of vertex data
        f->glEnable(GL_CULL_FACE);
        f->glEnable(GL_DEPTH_TEST);

        m_program->bind();
        QOpenGLVertexArrayObject::Binder vaoBinder(m_vao.get());
        // If VAOs are not supported, set the vertex attributes every time.
        if (!m_vao->isCreated())
        {
            setupVertexAttribs();
        }

        QMatrix4x4 m;
        m.translate(0, 0, -2);
        m_program->setUniformValue(m_matrixLoc, m_proj * m);

        // Draw the cube.
        if (m_viewport.size()>=4)
        {
            f->glViewport(m_viewport[0], m_viewport[1], m_viewport[2] ,m_viewport[3]);
            f->glDrawArrays(GL_TRIANGLES, 0, g_vertexCount);
        }
    }

    m_context->swapBuffers(w);
}
