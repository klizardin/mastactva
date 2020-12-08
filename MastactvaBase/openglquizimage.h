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
#include "../MastactvaModels/effect.h"


#if QT_CONFIG(opengl)


class ArgumentInfo
{
public:
    ArgumentInfo() = default;
    ~ArgumentInfo() = default;

    void setName(const QString &name_);
    void setType(const QString &type_);
    void setValue(const QString &value_);
    void initId(QOpenGLShaderProgram *program_);
    void setValue(QOpenGLShaderProgram *program_) const;

private:
    QString m_name;
    int m_id = 0;
    bool m_floatType = true;
    bool m_matrixType = false;
    int m_size = 0;
    QVector<GLfloat> m_valueFloat;
    QVector<GLint> m_valueInt;
};


class OpenGlQuizImage : public QSGRenderNode
{
public:
    OpenGlQuizImage(QObject * parent_);
    virtual ~OpenGlQuizImage() override;

    virtual void releaseResources() override;
    virtual void render(const RenderState *state_) override;
    virtual StateFlags changedStates() const override;
    virtual RenderingFlags flags() const override;
    virtual QRectF rect() const override;

    void sync(QQuickItem *item_);

private:
    void makeObject();
    void createTextures();
    void init(QOpenGLFunctions *f_);
    void paintGL(QOpenGLFunctions *f_, const RenderState *state_);
    void extractArguments();
    void resetProgram();
    QString loadFile(const QString &filename_);
    QString loadFileByUrl(const QString &filenameUrl_, bool useServerFiles_ = true);

private:
    QObject *m_parent = nullptr;
    int m_width = 0;
    int m_height = 0;
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
    int m_matrixId = -1;
    int m_texMatrix1Id = -1;
    int m_texMatrix2Id = -1;
    int m_tId = -1;

    QColor m_clearColor = Qt::black;
    qreal m_t = 0.0;
    QString m_fromImageUrl;
    QString m_toImageUrl;
    QString m_fromImageUrlNew;
    QString m_toImageUrlNew;
    Effect *m_effect = nullptr;
    int m_oldEffectId = -1;
    QList<ArgumentInfo> m_arguments;
    QString m_vertexShader;
    QString m_fragmentShader;
    QByteArray m_vshaderBA;
    QByteArray m_fshaderBA;
};


#endif // #if QT_CONFIG(opengl)
#endif // OPENGLQUIZIMAGE_H
