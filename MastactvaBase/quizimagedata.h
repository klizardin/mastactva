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
#include "../MastactvaBase/utils.h"


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
    bool isInput() const;
    bool isOutput() const;
    void setInput(bool isInput_);

    ArgumentValueDataArray *createValueDataArray() const;

private:
    QString m_name;
    QString m_storage;
    QString m_type;
    QString m_value;
    QString m_defaultValue;
    bool m_isInput = false;
};


class ArgumentList: public QList<ArgumentBase>
{
    using base = QList<ArgumentBase>;
public:
    using base::base;

    bool containsByName(const QString &argumentName_, bool isAny = true, bool isInput_ = true) const;
};


class OpenGLArgumentValueBase;


class ArgumentValueDataArray : public ArgumentBase
{
public:
    ArgumentValueDataArray(const ArgumentBase &from_, int arraySize_, int tupleSize_);
    virtual ~ArgumentValueDataArray() = default;

    virtual void initData();
    virtual void setArray(const QVariantList &values_) = 0;
    virtual QVariantList variantValues() const = 0;
    virtual OpenGLArgumentValueBase *createOpenGlValue() = 0;

    int getArraySize() const;
    int getTupleSize() const;

protected:
    virtual void setArray(const QString &value_) = 0;

protected:
    void initStorage(const QString &storage_);

    template<typename Type_>
    void initDataT(QVector<Type_> &values_)
    {
        if(m_isAttribute || m_isIndex)
        {
            m_arraySize = -1;
        }
        else if(m_isUniform && m_arraySize > 0)
        {
            if(m_arraySize > 0)
            {
                values_.resize(m_arraySize);
            }
            else
            {
                values_.clear();
            }
        }
        ArgumentValueDataArray::initData();
    }

    template<typename Type_>
    void setArrayT(const QVariantList &varValues_, QVector<Type_> &values_)
    {
        extractValues(varValues_, values_, m_arraySize);
    }

    template<typename Type_>
    void setArrayT(const QString &value_, QVector<Type_> &values_)
    {
        if(value_.contains(g_randArgumentValueName))
        {
            QVector<Type_> args;
            if(m_isAttribute || m_isIndex)
            {
                args.resize(3); // DOC: use 3 arguments (min, max, items count)
            }
            else if(m_isUniform)
            {
                args.resize(2); // DOC: use 2 arguments (min, max)
            }
            extractValues(value_, args, args.size());

            if((m_isAttribute || m_isIndex) && args.size() > 2)
            {
                values_.resize(int(args[2]));  // TODO: WARNING: not exact type
            }
            generateUniformRands(args, values_);
        }
        else
        {
            if(m_isAttribute || m_isIndex)
            {
                extractValues(value_, values_, m_arraySize);
            }
            else if(m_isUniform)
            {
                extractValues(value_, values_, values_.size());
            }
        }
    }

    template<typename Type_>
    QVariantList variantValuesT(const QVector<Type_> &values_) const
    {
        QVariantList res;
        for(const Type_ &v_ : values_)
        {
            res.push_back(v_);
        }
        return res;
    }

protected:
    bool m_isAttribute = false;
    bool m_isUniform = false;
    bool m_isIndex = false;

    bool m_isFixedSize = true;
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

    virtual void initData() override
    {
        initDataT(m_values);
    }
    virtual void setArray(const QVariantList &varValues_) override
    {
        setArrayT(varValues_, m_values);
    }
    virtual QVariantList variantValues() const override
    {
        return variantValuesT(m_values);
    }
    virtual OpenGLArgumentValueBase *createOpenGlValue() override;

    const QVector<GLint> &getValues() const;
    bool isMatrixType() const;

protected:
    virtual void setArray(const QString &value_) override
    {
        setArrayT(value_, m_values);
    }

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

    virtual void initData() override
    {
        initDataT(m_values);
    }
    virtual void setArray(const QVariantList &varValues_) override
    {
        setArrayT(varValues_, m_values);
    }
    virtual QVariantList variantValues() const override
    {
        return variantValuesT(m_values);
    }
    virtual OpenGLArgumentValueBase *createOpenGlValue() override;

    const QVector<GLfloat> &getValues() const;
    bool isMatrixType() const;

protected:
    virtual void setArray(const QString &value_) override
    {
        setArrayT(value_, m_values);
    }

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

    virtual void initData() override
    {
        initDataT(m_values);
    }
    virtual void setArray(const QVariantList &varValues_) override
    {
        setArrayT(varValues_, m_values);
    }
    virtual QVariantList variantValues() const override
    {
        return variantValuesT(m_values);
    }
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
    ArgumentValue();
    ~ArgumentValue();

    bool hasDataArray() const;
    ArgumentValueDataArray *getDataArray() const;
    void setDataArray(ArgumentValueDataArray *argumentValueDataArray_);
    void freeDataArray();

    int getEffectArgumentId() const;
    void clearEffectArgumentId();
    void setEffectArgumentId(int effectArgumentId_);

private:
    int m_effectArgumentId = -1;
    ArgumentValueDataArray *m_argumentValueDataArray = nullptr;
};


class ArgumentDataTable;


class DataTableValue
{
public:
    DataTableValue() = default;
    ~DataTableValue();

    bool hasArgumentDataArray() const;
    ArgumentValueDataArray *getArgumentDataArray();
    const ArgumentValueDataArray *getArgumentDataArray() const;
    void set(const ArgumentBase &argument_, int effectArgumentId_);
    bool convertToArgument(const ArgumentBase &templateArgument_);
    bool hasValue() const;
    bool isArgument() const;
    bool isIntValue() const;
    bool isFloatValue() const;
    bool isStringValue() const;
    QVariant getValue() const;

protected:
    void free();

    bool hasChild(const QString &key_) const;
    DataTableValue *getChild(const QString &key_);
    void copyFrom(const DataTableValue &dataTableValue_);
    int getArgumentValueEffectArgumentId() const;
    void setIntValue(const QVariant &value_);
    void setFloatValue(const QVariant &value_);
    void setStringValue(const QVariant &value_);
    QVariantList getChilderenValues() const;
    void freeChilderenValues();
    QList<QString> getChildrenKeys() const;

private:
    ArgumentValue m_argument;
    int *m_intValue = nullptr;
    float *m_floatValue = nullptr;
    QString *m_stringValue = nullptr;
    QHash<QString, DataTableValue> m_children;

    friend class ArgumentDataTable;
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
    bool hasRootChild(const QString &key_) const;
    DataTableValue *getRootChild(const QString &key_);
    DataTableValue *getArgument(
            const QString &objectName_,
            const QString &stepIndexStr_,
            const QString &argumentName_);
    void addArgument(
            const QString &objectName_,
            const QString &stepIndexStr_,
            const QString &argumentName_,
            const DataTableValue &argumentValue);

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
    virtual int getArraySize() const = 0;
    virtual int getMaxIndex() const = 0;
    virtual int getVBOPartSize() const = 0;
    virtual void setVBOPartOffset(int offset_) = 0;
    virtual void writeVBOPart(QOpenGLBuffer *vbo_, int offset_, int sizeItems_) const = 0;
    virtual void use(QOpenGLShaderProgram *program_) const = 0;
    virtual void draw(QOpenGLFunctions *f_) const = 0;
    virtual void release(QOpenGLShaderProgram *program_) const = 0;

protected:
    void initAttribureValueId(QOpenGLShaderProgram *program_, const QString &name_);
    void useAttributeValue(QOpenGLShaderProgram *program_, GLenum type_, int offset_, int tupleSize_) const;
    void writeAttributeValue(QOpenGLBuffer *vbo_, int offset_, int sizeItems_, const QVector<GLint> &values_, int partSize_, int tupleSize_) const;
    void writeAttributeValue(QOpenGLBuffer *vbo_, int offset_, int sizeItems_, const QVector<GLfloat> &values_, int partSize_, int tupleSize_) const;
    void writeAttributeValue(QOpenGLBuffer *vbo_, int offset_, int sizeItems_, const QVector<QString> &values_, int partSize_, int tupleSize_) const;
    void releaseAttributeValue(QOpenGLShaderProgram *program_) const;

    void initUniformValueId(QOpenGLShaderProgram *program_, const QString &name_);
    void setUniformValue(QOpenGLShaderProgram *program_, const QVector<GLint> &values_, int arraySize_, bool isMatrixType) const;
    void setUniformValue(QOpenGLShaderProgram *program_, const QVector<GLfloat> &values_, int arraySize_, bool isMatrixType) const;
    void setUniformValue(QOpenGLShaderProgram *program_, const QVector<QString> &values_, int arraySize_, bool isMatrixType) const;

    void drawTrianlesArray(QOpenGLFunctions *f_, int size_) const;

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
        initUniformValueId(
                    program_,
                    arg().getName());
    }

    virtual int getArraySize() const override
    {
        const int size = value().getArraySize();
        Q_ASSERT(size > 0);
        return std::max(0, size);
    }

    virtual int getMaxIndex() const override
    {
        return 0;
    }

    virtual int getVBOPartSize() const override
    {
        return 0;
    }

    virtual void setVBOPartOffset(int offset_)
    {
        Q_UNUSED(offset_);
    }

    virtual void writeVBOPart(QOpenGLBuffer *vbo_, int offset_, int sizeItems_) const override
    {
        Q_UNUSED(vbo_);
        Q_UNUSED(offset_);
        Q_UNUSED(sizeItems_);
    }

    virtual void use(QOpenGLShaderProgram *program_) const override
    {
        setUniformValue(program_,
                        value().getValues(),
                        getArraySize(),
                        value().isMatrixType());
    }

    virtual void draw(QOpenGLFunctions *f_) const override
    {
        Q_UNUSED(f_);
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
        initAttribureValueId(
                    program_,
                    arg().getName());
    }

    virtual int getArraySize() const override
    {
        return value().getValues().size();
    }

    virtual int getMaxIndex() const override
    {
        return ((getArraySize() + value().getTupleSize() - 1) / value().getTupleSize());
    }

    virtual int getVBOPartSize() const override
    {
        return getArraySize();
    }

    virtual void setVBOPartOffset(int offset_)
    {
        m_offset = offset_;
    }

    virtual void writeVBOPart(QOpenGLBuffer *vbo_, int offset_, int sizeItems_) const override
    {
        writeAttributeValue(
                    vbo_,
                    offset_,
                    sizeItems_,
                    value().getValues(),
                    getVBOPartSize(),
                    value().getTupleSize());
    }

    virtual void use(QOpenGLShaderProgram *program_) const override
    {
        useAttributeValue(
                    program_,
                    TypeToGLTypeEnum<ItemType>::value,
                    m_offset,
                    value().getTupleSize());
    }

    virtual void draw(QOpenGLFunctions *f_) const override
    {
        Q_UNUSED(f_);
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


template<class ArgumentValueDataArrayType_>
class OpenGLArgumentIndexValueT :
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
    OpenGLArgumentIndexValueT(const ArgumentValueDataArrayType_ &argumentValueDataArray_)
        :ArgumentValueDataArrayType_(argumentValueDataArray_)
    {
        m_maxIndex = std::max_element(
                    std::begin(value().getValues()),
                    std::end(value().getValues())
                    );
    }

    virtual void create(QOpenGLShaderProgram *program_) override
    {
        Q_UNUSED(program_);
    }

    virtual int getArraySize() const override
    {
        return value().getValues().size();
    }

    virtual int getMaxIndex() const override
    {
        return std::max(0, m_maxIndex);
    }

    virtual int getVBOPartSize() const override
    {
        return 0;
    }

    virtual void setVBOPartOffset(int offset_)
    {
        Q_UNUSED(offset_);
    }

    virtual void writeVBOPart(QOpenGLBuffer *vbo_, int offset_, int sizeItems_) const override
    {
        Q_UNUSED(vbo_);
        Q_UNUSED(offset_);
        Q_UNUSED(sizeItems_);
    }

    virtual void use(QOpenGLShaderProgram *program_) const override
    {
        Q_UNUSED(program_);
    }

    virtual void draw(QOpenGLFunctions *f_) const override
    {
        drawTrianlesArray(f_, value().getValues().size());
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

private:
    int m_maxIndex = 0;
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
