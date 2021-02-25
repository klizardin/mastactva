#ifndef QUIZIMAGEDATA_H
#define QUIZIMAGEDATA_H


#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLTexture>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLShader>
#include <QJsonDocument>
#include <type_traits>


#if QT_CONFIG(opengl)


class ArgumentValueDataArray;


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

    ArgumentValueDataArray *createValueDataArray() const;

private:
    QString m_name;
    QString m_storage;
    QString m_type;
    QString m_value;
    QString m_defaultValue;
};


using ArgumentList = QList<ArgumentBase>;


class OpenGLArgumentValueBase;


class ArgumentValueDataArray : public ArgumentBase
{
public:
    ArgumentValueDataArray(const ArgumentBase &from_, int arraySize_, int tupleSize_);
    virtual ~ArgumentValueDataArray() = default;

    virtual void initData();
    virtual void setArray(const QVariantList &values_) = 0;
    virtual OpenGLArgumentValueBase *createOpenGlValue() = 0;

    int getArraySize() const;
    int getTupleSize() const;

protected:
    virtual void setArray(const QString &value_) = 0;

protected:
    void initStorage(const QString &storage_);

protected:
    bool m_isAttribute = false;
    bool m_isUniform = false;
    bool m_isIndex = false;

    int m_arraySize = 0;
    int m_tupleSize = 1;

    friend class ArgumentBase;
};

class ArgumentValueDataIntArray : public ArgumentValueDataArray
{
public:
    using ItemType = GLint;

public:
    ArgumentValueDataIntArray(const ArgumentBase &from_, int arraySize_, int tupleSize_);
    virtual ~ArgumentValueDataIntArray() override = default;

    virtual void initData() override;
    virtual void setArray(const QVariantList &values_) override;
    virtual OpenGLArgumentValueBase *createOpenGlValue() override;

    const QVector<GLint> &getValues() const;
    bool isMatrixType() const;

protected:
    virtual void setArray(const QString &value_) override;

private:
    QVector<GLint> m_values;
};


class ArgumentValueDataFloatArray : public ArgumentValueDataArray
{
public:
    using ItemType = GLfloat;

public:
    ArgumentValueDataFloatArray(const ArgumentBase &from_, int arraySize_, int tupleSize_, bool isMatrixType_);
    virtual ~ArgumentValueDataFloatArray() override = default;

    virtual void initData() override;
    virtual void setArray(const QVariantList &values_) override;
    virtual OpenGLArgumentValueBase *createOpenGlValue() override;

    const QVector<GLfloat> &getValues() const;
    bool isMatrixType() const;

protected:
    virtual void setArray(const QString &value_) override;

private:
    QVector<GLfloat> m_values;
    bool m_isMatrixType = false;
};


class ArgumentValueDataStringArray : public ArgumentValueDataArray
{
public:
    using ItemType = QString;

public:
    ArgumentValueDataStringArray(const ArgumentBase &from_, int arraySize_, int tupleSize_);
    virtual ~ArgumentValueDataStringArray() override = default;

    virtual void initData() override;
    virtual void setArray(const QVariantList &values_) override;
    virtual OpenGLArgumentValueBase *createOpenGlValue() override;

    const QVector<QString> &getValues() const;
    bool isMatrixType() const;

protected:
    virtual void setArray(const QString &value_) override;

private:
    QVector<QString> m_values;
};


class ArgumentValue
{
public:
    ArgumentValue() = default;
    virtual ~ArgumentValue() = default;

    void setDataArray(ArgumentValueDataArray *argumentValueDataArray_);
    ArgumentValueDataArray *getDataArray() const;
    int getEffectArgumentId() const;
    void setEffectArgumentId(int effectArgumentId_);

private:
    int m_effectArgumentId = -1;
    ArgumentValueDataArray *m_argumentValueDataArray = nullptr;
};


class DataTableValue
{
public:
    DataTableValue() = default;
    ~DataTableValue();

    ArgumentValueDataArray *getArgumentDataArray();
    void set(const ArgumentBase &argument_, int effectArgumentId_);
    void convertToArgument(const ArgumentBase &templateArgument_);

private:
    ArgumentValue *m_argument = nullptr;
    int *m_intValue = nullptr;
    float *m_floatValue = nullptr;
    QString *m_stringValue = nullptr;
    QHash<QString, DataTableValue> m_children;
};


class ArgumentsSet;


class ArgumentDataTable
{
public:
    ArgumentDataTable() = default;
    ~ArgumentDataTable() = default;

protected:
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
    ArgumentValueDataArray *find(
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
    QHash<QString, DataTableValue> m_root;

    friend class ArgumentsSet;
};

class ArgumentsSet
{
public:
    ArgumentsSet() = default;
    ~ArgumentsSet() = default;

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
    ArgumentValueDataArray *find(
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


class OpenGLArgumentValueBase
{
public:
    virtual ~OpenGLArgumentValueBase() = default;

    virtual void create(QOpenGLShaderProgram *program_) = 0;
    virtual int getVBOPartSize() const = 0;
    virtual void writeVBOPart(QOpenGLBuffer *vbo_, int &offset_) = 0;
    virtual void use(QOpenGLShaderProgram *program_) const = 0;
    virtual void draw(QOpenGLShaderProgram *program_) const = 0;
    virtual void release(QOpenGLShaderProgram *program_) const = 0;

protected:
    void initAttribureValueId(QOpenGLShaderProgram *program_, const QString &name_);
    void initUniformValueId(QOpenGLShaderProgram *program_, const QString &name_);
    void initIndexArray(QOpenGLShaderProgram *program_);
    void setAttributeValue(QOpenGLShaderProgram *program_, GLenum type_, int offset_, int tupleSize_) const;
    void releaseAttributeValue(QOpenGLShaderProgram *program_) const;
    void setUniformValue(QOpenGLShaderProgram *program_, const QVector<GLint> &values_, int arraySize_, bool isMatrixType) const;
    void setUniformValue(QOpenGLShaderProgram *program_, const QVector<GLfloat> &values_, int arraySize_, bool isMatrixType) const;
    void setUniformValue(QOpenGLShaderProgram *program_, const QVector<QString> &values_, int arraySize_, bool isMatrixType) const;
    void setIndexArray(QOpenGLShaderProgram *program_) const;
    void drawIndexArray(QOpenGLShaderProgram *program_) const;

private:
    int m_id = -1;
};

template<class ArgumentValueDataArrayType_>
class OpenGLArgumentUniformValueT :
        public ArgumentValueDataArrayType_,
        public OpenGLArgumentValueBase
{
private:
    static_assert(
        std::is_base_of<ArgumentValueDataArray, ArgumentValueDataArrayType_>::value,
        "shoudl be ancestor of ArgumentValueDataArray");
    static_assert(
        std::is_base_of<ArgumentBase, ArgumentValueDataArrayType_>::value,
        "shoudl be ancestor of ArgumentBase");

public:
    OpenGLArgumentUniformValueT(const ArgumentValueDataArrayType_ &argumentValueDataArray_)
        :ArgumentValueDataArrayType_(argumentValueDataArray_)
    {
    }

    virtual void create(QOpenGLShaderProgram *program_) override
    {
        initUniformValueId(program_, arg().getName());
    }

    virtual int getVBOPartSize() const override
    {
        return 0;
    }

    virtual void writeVBOPart(QOpenGLBuffer *vbo_, int &offset_) override
    {
        Q_UNUSED(vbo_);
        Q_UNUSED(offset_);
    }

    virtual void use(QOpenGLShaderProgram *program_) const override
    {
        setUniformValue(program_, value().getValues(), value().getArraySize(), value().isMatrixType());
    }

    virtual void draw(QOpenGLShaderProgram *program_) const override
    {
        Q_UNUSED(program_);
    }

    virtual void release(QOpenGLShaderProgram *program_) const override
    {
        Q_UNUSED(program_);
    }

private:
    const ArgumentBase &arg() const
    {
        return static_cast<const ArgumentBase&>(*this);
    }

    const ArgumentValueDataArrayType_ &value() const
    {
        return static_cast<const ArgumentValueDataArrayType_&>(*this);
    }
};

template<typename Type_>
struct TypeToGLTypeEnum
{
    constexpr static GLenum value = GL_BYTE;
};

template<>
struct TypeToGLTypeEnum<GLint>
{
    constexpr static GLenum value = GL_INT;
};

template<>
struct TypeToGLTypeEnum<GLfloat>
{
    constexpr static GLenum value = GL_FLOAT;
};


template<class ArgumentValueDataArrayType_>
class OpenGLArgumentAttributeValueT :
        public ArgumentValueDataArrayType_,
        public OpenGLArgumentValueBase
{
private:
    static_assert(
        std::is_base_of<ArgumentValueDataArray, ArgumentValueDataArrayType_>::value,
        "shoudl be ancestor of ArgumentValueDataArray");
    static_assert(
        std::is_base_of<ArgumentBase, ArgumentValueDataArrayType_>::value,
        "shoudl be ancestor of ArgumentBase");

    using ItemType = typename ArgumentValueDataArrayType_::ItemType;

public:
    OpenGLArgumentAttributeValueT(const ArgumentValueDataArrayType_ &argumentValueDataArray_)
        :ArgumentValueDataArrayType_(argumentValueDataArray_)
    {
    }

    virtual void create(QOpenGLShaderProgram *program_) override
    {
        initAttribureValueId(program_, arg().getName());
    }

    virtual int getVBOPartSize() const override
    {
        return value().getArraySize();
    }

    virtual void writeVBOPart(QOpenGLBuffer *vbo_, int &offset_) override
    {
        m_offset = offset_;
        vbo_->write(
                    m_offset,
                    value().getValues(),
                    value().getArraySize() * sizeof(ItemType)
                    );
        offset_ += value().getArraySize() * sizeof(ItemType);
    }

    virtual void use(QOpenGLShaderProgram *program_) const override
    {
        setAttributeValue(program_, TypeToGLTypeEnum<ItemType>::value, m_offset, value().getTupleSize());
    }

    virtual void draw(QOpenGLShaderProgram *program_) const override
    {
        Q_UNUSED(program_);
    }

    virtual void release(QOpenGLShaderProgram *program_) const override
    {
        releaseAttributeValue(program_);
    }

private:
    const ArgumentBase &arg() const
    {
        return static_cast<const ArgumentBase&>(*this);
    }

    const ArgumentValueDataArrayType_ &value() const
    {
        return static_cast<const ArgumentValueDataArrayType_&>(*this);
    }
private:
    int m_offset = -1;
};


class Effect;
class EffectArgSet;


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
