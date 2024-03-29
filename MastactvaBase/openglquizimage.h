﻿/*
    Copyright 2021

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
#include <QByteArray>
#include <QImage>
#include <QMatrix4x4>
#include "../MastactvaBase/quizimagedata.h"
//#include "../MastactvaModels/effect.h"
//#include "../MastactvaModels/effectargset.h"


#if QT_CONFIG(opengl)


/*class OpenGlQuizImage : public QSGRenderNode
{
public:
    OpenGlQuizImage();
    virtual ~OpenGlQuizImage() override;

    virtual void releaseResources() override;
    virtual void render(const RenderState *state_) override;
    virtual StateFlags changedStates() const override;
    virtual RenderingFlags flags() const override;
    virtual QRectF rect() const override;

    void sync(QQuickItem *item_);

private:
    void makeObject();
    void makeTextureMatrixes();
    void calculatePreserveAspectFitTextureMatrix(
            QMatrix4x4 & textureMatrix_,
            const QSize &imageSize_,
            const QSize &rectSize_);
    void createTextures();
    void init(QOpenGLFunctions *f_);
    void paintGL(QOpenGLFunctions *f_, const RenderState *state_);
    void initDefaultShaders();
    void initGeometry();
    void resetProgram();
    //QString loadFile(const QString &filename_);
    //QString loadFileByUrl(const QString &filenameUrl_, bool useServerFiles_ = true);
    bool getRenderRectSize(QVariantList &values_);
    bool renderStateInitializeNone(QVariantList &values_);
    void freeDrawingData();

private:
    int m_left = 0;
    int m_top = 0;
    int m_width = 0;
    int m_height = 0;
    int m_geomertyPointsWidth = 3;
    int m_geometryPointsHeight = 3;
    bool m_geometrySolid = false;
    GLfloat m_facedGeometryXCoef = 1e-4;
    GLfloat m_facedGeometryYCoef = 1e-4;
    QVector<GLfloat> m_vertData;

    QOpenGLTexture *m_fromTexture = nullptr;
    QOpenGLTexture *m_toTexture = nullptr;

    QOpenGLShaderProgram *m_program = nullptr;
    QOpenGLShader *m_vshader = nullptr;
    QOpenGLShader *m_fshader = nullptr;
    QOpenGLBuffer *m_vbo = nullptr;

    int m_vertexAttrId = -1;
    int m_texCoordAttrId = -1;
    int m_fromTextureId = -1;
    int m_toTextureId = -1;
    int m_opacitiId = -1;
    int m_matrixId = -1;
    int m_texMatrix1Id = -1;
    int m_texMatrix2Id = -1;
    int m_tId = -1;

    //QuizImageData m_data;
    OpenGLDrawingImageData *m_drawingData = nullptr;
    qreal m_t = 0.0;
    QByteArray m_vshaderBA;
    QByteArray m_fshaderBA;
    QImage *m_fromImage = nullptr;
    QImage *m_toImage = nullptr;
    QMatrix4x4 m_texMatrix1;
    QMatrix4x4 m_texMatrix2;
    bool m_updateSize = false;

    QString m_programBuildLog;

    friend class ArgumentInfo;
};*/


class OpenGlQuizImage : public QSGRenderNode
{
public:
    OpenGlQuizImage();
    virtual ~OpenGlQuizImage() override;

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
#endif // OPENGLQUIZIMAGE_H
