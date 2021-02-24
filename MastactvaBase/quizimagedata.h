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


/*
 *  container
*/
class ArgumentBase
{
public:
    ArgumentBase() = default;
    virtual ~ArgumentBase() = default;

    const QString &getName() const;
    void setName(const QString &name_);
    const QString &getStorage() const;
    void setStorage(const QString &storage_);
    const QString &getType() const;
    void setType(const QString &type_);
    const QString &getValue() const;
    void setValue(const QString &value_);
    const QString &getDefaultValue() const;
    void setDefaultValue(const QString &defaultValue_);

private:
    QString m_name;
    QString m_storage;
    QString m_type;
    QString m_value;
    QString m_defaultValue;
};


using ArgumentList = QList<ArgumentBase>;


class ArgumentValueData : public ArgumentBase
{
public:
    void initData();

    bool isIntType() const;
    bool isFloatType() const;
    bool isStyringType() const;

protected:
    void initStorage(const QString &storage_);
    void initType(const QString &type_);
    void setArray(const QString &value_);
    void setArray(const QVariantList &values_);

protected:
    bool m_isAttribute = false;
    bool m_isUniform = false;
    bool m_isIndex = false;

    bool m_intArrayType = false;
    bool m_floatArrayType = false;
    bool m_stringArrayType = false;
    bool m_matrixType = false;

    int m_arraySize = 0;
    QVector<GLint> m_intValues;
    QVector<GLfloat> m_floatValues;
    QVector<QString> m_stringValues;
};


class ArgumentValue: public ArgumentValueData
{
public:
    ArgumentValue() = default;
    ArgumentValue(const ArgumentValueData& argumentValueData_);
    virtual ~ArgumentValue() override = default;

    int getEffectArgumentId() const;
    void setEffectArgumentId(int effectArgumentId_);

private:
    int m_effectArgumentId = -1;
};


class DataTableValue
{
public:
    DataTableValue() = default;
    ~DataTableValue();

    ArgumentValueData *getArgumentData();
    void set(const ArgumentBase &argument_, int effectArgumentId_);
    void convertToArgument(const ArgumentBase &templateArgument_);

private:
    ArgumentValue *m_argument = nullptr;
    int *m_intValue = nullptr;
    float *m_floatValue = nullptr;
    QString *m_stringValue = nullptr;
    QHash<QString, DataTableValue> m_children;
};

class ArgumentDataTable
{
private:
    QHash<QString, DataTableValue> m_root;
};

class ArgumentsSet
{
public:
    void add(
            const QString &objectName_,
            int step_index,
            const ArgumentBase &argument_,
            int effectArgumentId_);
    void add(
            const ArgumentDataTable &data_);    // add all from data table
    void add(
            const ArgumentDataTable &data_,
            const ArgumentList &argumentList_); // add output of the list
    ArgumentValueData *find(
            const QString &objectName_,
            int step_index,
            const ArgumentBase &argument_);
    ArgumentBase *find(
            int effectArgumentId_);
    ArgumentDataTable* slice(
            const QString &objectName_,
            int step_index,
            const ArgumentList &argumentList_); // get input of the list
private:
    ArgumentDataTable m_data;
};


class OpenGLArgumentValue : public ArgumentValueData
{
public:
    OpenGLArgumentValue() = default;
    OpenGLArgumentValue(const ArgumentValueData &argumentValueData_);
    virtual ~OpenGLArgumentValue() override = default;

    void initShaderId(QOpenGLShaderProgram *program_);
    void setShaderUniformValue(QOpenGLShaderProgram *program_) const;

private:
    int m_id = 0;
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

private:
    int m_index = -1;
    QJsonDocument *m_artefactDocument = nullptr;
    QString *m_luaScript = nullptr;
    QImage *m_texture = nullptr;
    QString *m_vertexShader = nullptr;
    QString *m_fragmentShader = nullptr;
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
