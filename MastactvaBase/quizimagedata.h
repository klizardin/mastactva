#ifndef QUIZIMAGEDATA_H
#define QUIZIMAGEDATA_H


#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLTexture>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLShader>


#if QT_CONFIG(opengl)


class OpenGlQuizImage;
class Effect;
class EffectArgSet;


class ArgumentInfo
{
public:
    ArgumentInfo() = default;
    ~ArgumentInfo() = default;

    void setArgId(int argId_);
    int getArgId() const;
    void setName(const QString &name_);
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


class QuizImageData
{
public:
    QuizImageData();

    void setFromImageUrl(const QString &fromImageUrl_);
    void setToImageUrl(const QString &toImageUrl_);
    bool fromImageUrlChanged() const;
    void useNewFromImageUrl();
    bool toImageUrlChanged() const;
    void useNewToImageUrl();
    bool isSwapImages() const;
    void swapImages();
    void setEffectId(int effectId_);
    void setArgumentSetId(int argumentSetId_);
    bool effectChanged() const;
    void useNewEffect();
    void setShaders(const QString &vertexShader_, const QString &fragmentShader_);
    void setVertexShader(const QString &vertexShader_);
    void setFragmentShader(const QString &fragmentShader_);
    void initDefaultShaders();
    void setArguments(const QList<ArgumentInfo> &arguments_);
    void clearArguments();
    void appendArguments(const ArgumentInfo &argument_);
    void setArgumentValue(int argId_, const QString &value_);
    bool isFromImageIsUrl() const;
    bool isToImageIsUrl() const;
    const QString &getFromImageUrl() const;
    const QString &getToImageUrl() const;
    const QList<ArgumentInfo> &getArguments() const;
    QList<ArgumentInfo> &getArgumentsNC();
    const QString &getVertexShader() const;
    const QString &getFragmentShader() const;

    void extractArguments(const Effect *effect_, const EffectArgSet *argumentSet_);
protected:

protected:
    QString m_newFromImageUrl;
    QString m_newToImageUrl;
    QString m_fromImageUrl;
    QString m_toImageUrl;
    int m_newEffectId = -1;
    int m_newArgumentSetId = -1;
    int m_effectId = -1;
    int m_argumentSetId = -1;
    QList<ArgumentInfo> m_arguments;
    QString m_vertexShader;
    QString m_fragmentShader;
};


#endif


#endif // QUIZIMAGEDATA_H
