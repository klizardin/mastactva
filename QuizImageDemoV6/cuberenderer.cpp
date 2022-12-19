/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "cuberenderer.h"
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOffscreenSurface>
#include <QWindow>

DefaultRenderer::DefaultRenderer(QOffscreenSurface *offscreenSurface)
    : m_offscreenSurface(offscreenSurface)
{
}

DefaultRenderer::~DefaultRenderer()
{
    // Use a temporary offscreen surface to do the cleanup.
    // There may not be a native window surface available anymore at this stage.
    m_context->makeCurrent(m_offscreenSurface);

    m_program.reset(nullptr);
    m_vbo.reset(nullptr);
    m_vao.reset(nullptr);

    m_context->doneCurrent();
    m_context.reset(nullptr);
}

const int vertexCount = 6;//36;

/*
 * we need QWindow and QOpenGlContext to init drawing content
*/
void DefaultRenderer::init(QWindow *w, QOpenGLContext *share)
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
    f->glClearColor(0.0f, 0.1f, 0.25f, 1.0f);

    m_viewport.push_back(0);
    m_viewport.push_back(0);
    m_viewport.push_back(w->width() * w->devicePixelRatio() * 0.5);
    m_viewport.push_back(w->height() * w->devicePixelRatio());

    m_viewport.push_back(w->width() * w->devicePixelRatio() * 0.5);
    m_viewport.push_back(0);
    m_viewport.push_back(w->width() * w->devicePixelRatio());
    m_viewport.push_back(w->height() * w->devicePixelRatio());
    //if(m_viewport.size()>=4)
    //    f->glViewport(m_viewport[0], m_viewport[1], m_viewport[2] ,m_viewport[3]);

    static const char *vertexShaderSource =
        "attribute highp vec4 vertex;\n"
        "attribute lowp vec2 coord;\n"
        "varying lowp vec2 v_coord;\n"
        "uniform highp mat4 matrix;\n"
        "void main() {\n"
        "   v_coord = coord;\n"
        "   gl_Position = matrix * vertex;\n"
        "}\n";
    static const char *fragmentShaderSource =
        "varying lowp vec2 v_coord;\n"
        "uniform sampler2D sampler;\n"
        "void main() {\n"
        "   gl_FragColor = vec4(texture2D(sampler, v_coord).rgb, 1.0);\n"
        "}\n";
    m_program = std::make_unique<QOpenGLShaderProgram>();
    m_program->addCacheableShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addCacheableShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
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

    GLfloat v[] = {
        -1.0, 1.0, 1.0,     1.0,-1.0,1.0,       -1.0,-1.0,1.0,
        1.0, -1.0, 1.0,     -1.0,1.0,1.0,       1.0,1.0,1.0,
    };
    GLfloat texCoords[] = {
        0.0f,0.0f,      1.0f,1.0f,      1.0f,0.0f,
        1.0f,1.0f,      0.0f,0.0f,      0.0f,1.0f,
    };

    m_vbo->allocate(sizeof(GLfloat) * vertexCount * 5);
    m_vbo->write(0, v, sizeof(GLfloat) * vertexCount * 3);
    m_vbo->write(sizeof(GLfloat) * vertexCount * 3, texCoords, sizeof(GLfloat) * vertexCount * 2);
    m_vbo->release();

    if (m_vao->isCreated())
        setupVertexAttribs();
}

void DefaultRenderer::resize(int width, int height)
{
    m_proj.setToIdentity();
    m_proj.perspective(45, width / float(height), 0.01f, 100.0f);
}

void DefaultRenderer::setupVertexAttribs()
{
    m_vbo->bind();
    m_program->enableAttributeArray(0);
    m_program->enableAttributeArray(1);
    m_context->functions()->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    m_context->functions()->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0,
                                                  (const void *)(vertexCount * 3 * sizeof(GLfloat)));
    m_vbo->release();
}

/*
 * TODO: add texture1, texture2
*/
void DefaultRenderer::render(QWindow *w, QOpenGLContext *share, uint texture1, uint texture2)
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

    if(texture1 || texture2)
    {
        f->glBindTexture(GL_TEXTURE_2D, texture1);
        f->glFrontFace(GL_CW); // because our cube's vertex data is such
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

        if (texture1 && m_viewport.size()>=4)
        {
            f->glViewport(m_viewport[0], m_viewport[1], m_viewport[2] ,m_viewport[3]);
            f->glDrawArrays(GL_TRIANGLES, 0, vertexCount);
        }
        if (texture2 && m_viewport.size()>=8)
        {
            f->glViewport(m_viewport[4], m_viewport[5], m_viewport[6] ,m_viewport[7]);
            f->glDrawArrays(GL_TRIANGLES, 0, vertexCount);
        }
    }

    m_context->swapBuffers(w);
}
