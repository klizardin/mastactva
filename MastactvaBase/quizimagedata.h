#ifndef QUIZIMAGEDATA_H
#define QUIZIMAGEDATA_H


#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLTexture>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLShader>
#include <QJsonDocument>


#if QT_CONFIG(opengl)


class ArgumentsSet;
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
    void initValueFromArtumentsSet(ArgumentsSet *arguments_);
    void initId(QOpenGLShaderProgram *program_);
    void setValue(QOpenGLShaderProgram *program_) const;

private:
    QString m_name;
    bool (ArgumentsSet::*m_argumentsSetInitfuctionFunc)(QVariantList &values_) = nullptr;
    int m_id = 0;
    int m_argId = -1;
    bool m_floatType = true;
    bool m_matrixType = false;
    int m_size = 0;
    QVector<GLfloat> m_valueFloat;
    QVector<GLint> m_valueInt;
};


class ArgumentsSet
{
public:
    void clear();
    void setValue(int argId_, const QString &value_);
    void appendArgument(const ArgumentInfo &argument_);
    void setArguments(const QList<ArgumentInfo> &arguments_);

    const QList<ArgumentInfo> &getArguments() const;
    QList<ArgumentInfo> &getArgumentsNC();

    // function to init argument from arguments set
public:
    bool nullInitialization(QVariantList &values_);
    bool getRenderRectSize(QVariantList &values_);

private:
    QList<ArgumentInfo> m_arguments;
};


class QuizImageDataArtefact
{
public:
    QuizImageDataArtefact() = default;

    int getIndex() const;
    void setIndex(int index_);

    bool isData() const;
    bool isLuaScript() const;
    bool isTexture() const;
    bool isVertexShader() const;
    bool isFragmentShader() const;

    const QJsonDocument &getData() const;
    void setJsonData(const QString &jsonData_);
    void setOBJData(const QString &objData_);

    const QString &getLuaScript() const;
    void setLuaScript(const QString &luaScript_);

    const QImage &getTextureImage() const;
    void setTextureImage(const QString &imageUrl_);

    const QString &getVertexShader() const;
    void setVertexShader(const QString &vertexShader_);

    const QString &getFragmentShader() const;
    void setFragmentShader(const QString &fragmentShader_);

    const ArgumentsSet &getArguments() const;
    void addNewArgument(const ArgumentInfo &ai_);

private:
    int m_index = -1;
    QJsonDocument *m_artefactDocument = nullptr;
    QString *m_luaScript = nullptr;
    QImage *m_texture = nullptr;
    QString *m_vertexShader = nullptr;
    QString *m_fragmentShader = nullptr;
    ArgumentsSet m_arguments;
};


class QuizImageDataObject
{
public:
    QuizImageDataObject(const QString &programmerName_);

    void setArtefact(const QString &artefactType_, const QString &artefactDataString_, int artefactIndex_);

    void setArtefactVertexShader(const QString &vertexShader_, int artefactIndex_);
    void setArtefactFragmentShader(const QString &fragmentShader_, int artefactIndex_);
    void setArtefactDataJson(const QString &jsonData_, int artefactIndex_);
    void setArtefactDataOBJ(const QString &objData_, int artefactIndex_);

    void initDefaultShaderArtefacts();

    const QList<QuizImageDataArtefact> &getArtefacts() const;

protected:
    void sortArtefacts();

protected:
    QString m_programmerName;
    QList<QuizImageDataArtefact> m_artefactData;
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

    void addNewObject(const QString &programmerName_);
    QuizImageDataObject &getCurrentObjectRef();
    const QList<QuizImageDataObject> &getObjects() const;

    bool isFromImageIsUrl() const;
    bool isToImageIsUrl() const;
    const QString &getFromImageUrl() const;
    const QString &getToImageUrl() const;

    const ArgumentsSet &getArguments() const;
    ArgumentsSet &getArgumentsNC();

    void extractObjects(const Effect *effect_);
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
    QList<QuizImageDataObject> m_objects;
    ArgumentsSet m_arguments;
};


#endif


#endif // QUIZIMAGEDATA_H
