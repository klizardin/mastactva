﻿/*
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
#include "../MastactvaModels/effect.h"
#include "../MastactvaModels/effectargset.h"


#if QT_CONFIG(opengl)


class OpenGlQuizImage;
class QuizImage;


class ArgumentInfo
{
public:
    ArgumentInfo() = default;
    ~ArgumentInfo() = default;

    void setArgId(int argId_);
    int getArgId() const;
    void setName(const QString &name_);
    const QString &getName() const;
    void setType(const QString &type_);
    void setValue(const QString &value_);
    void setValue(const QVariantList &values_);
    void initValueFromRenderState(OpenGlQuizImage *render_);
    void initId(QOpenGLShaderProgram *program_);
    void setValue(QOpenGLShaderProgram *program_) const;

private:
    QString m_name;
    bool (OpenGlQuizImage::*m_renderStateInitializeFunc)(QVariantList &values_) = nullptr;
    int m_id = 0;
    int m_argId = -1;
    bool m_floatType = true;
    bool m_matrixType = false;
    int m_size = 0;
    QVector<GLfloat> m_valueFloat;
    QVector<GLint> m_valueInt;
};


class OpenGlQuizImage : public QSGRenderNode
{
public:
    OpenGlQuizImage(QObject *parent_);
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
    void calculatePreserveAspectFitTextureMatrix(QMatrix4x4 & textureMatrix_, const QSize &imageSize_, const QSize &rectSize_);
    void createTextures();
    void init(QOpenGLFunctions *f_);
    void paintGL(QOpenGLFunctions *f_, const RenderState *state_);
    void extractArguments();
    void initDefaultShaders();
    void initGeometry();
    void resetProgram();
    QString loadFile(const QString &filename_);
    bool getRenderRectSize(QVariantList &values_);
    bool renderStateInitializeNone(QVariantList &values_);
    int getGeometryItemSize() const;

private:
    QObject *m_parent = nullptr;
    int m_left = 0;
    int m_top = 0;
    int m_width = 0;
    int m_height = 0;
    int m_geomertyPointsWidth = 3;
    int m_geometryPointsHeight = 3;
    bool m_geometrySolid = false;
    GLfloat m_facedGeometryXCoef = 1e-3;
    GLfloat m_facedGeometryYCoef = 1e-3;
    QVector<GLfloat> m_vertData;

    QOpenGLTexture *m_fromTexture = nullptr;
    QOpenGLTexture *m_toTexture = nullptr;

    QOpenGLShaderProgram *m_program = nullptr;
    QOpenGLShader *m_vshader = nullptr;
    QOpenGLShader *m_fshader = nullptr;
    QOpenGLBuffer *m_vbo = nullptr;

    int m_vertexAttrId = -1;
    int m_texCoordAttrId = -1;
    int m_colorAttrId = -1;
    int m_fromTextureId = -1;
    int m_toTextureId = -1;
    int m_opacitiId = -1;
    int m_matrixId = -1;
    int m_texMatrix1Id = -1;
    int m_texMatrix2Id = -1;
    int m_tId = -1;

    qreal m_t = 0.0;
    QString m_fromImageUrl;
    QString m_toImageUrl;
    QString m_fromImageUrlNew;
    QString m_toImageUrlNew;
    QList<ArgumentInfo> m_arguments;
    QString m_vertexShader;
    QString m_fragmentShader;
    QByteArray m_vshaderBA;
    QByteArray m_fshaderBA;
    QImage *m_fromImage = nullptr;
    QImage *m_toImage = nullptr;
    QMatrix4x4 m_texMatrix1;
    QMatrix4x4 m_texMatrix2;
    bool m_updateSize = false;
    bool m_attributeColors = false;
    int m_geometryItemSize = 1;

    QString m_programBuildLog;

    friend class ArgumentInfo;
};


#endif // #if QT_CONFIG(opengl)
#endif // OPENGLQUIZIMAGE_H
