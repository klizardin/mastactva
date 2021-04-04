#ifndef QUIZIMAGEDATA_H
#define QUIZIMAGEDATA_H


#include <type_traits>
#include <set>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLTexture>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLShader>
#include "../MastactvaBase/names.h"
#include "../MastactvaBase/utils.h"


#if QT_CONFIG(opengl)


class ArgumentValueDataArray;
class ArtefactArg;
class ArtefactArgStorageModel;
class ArtefactArgTypeModel;


class ArgumentBase
{
public:
    ArgumentBase() = default;
    virtual ~ArgumentBase() = default;

    bool set(
            const ArtefactArg *arg_,
            ArtefactArgStorageModel* argStorageModel_ = nullptr,
            ArtefactArgTypeModel *argTypeModel_ = nullptr
            );
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

    /* used for scripts arguments
     * isOutput() == true
     *      * ... // TODO
     * isInput() == true
     *      * ... // TODO
    */
    bool isInput() const;
    bool isOutput() const;
    void setInput(bool isInput_);

    ArgumentValueDataArray *createValueDataArray() const;
    void deepCopy();
    static ArgumentBase *fromJson(const QJsonDocument &obj_, bool isInput_ = true);
    static ArgumentBase *fromJson(const QJsonObject &obj_, bool isInput_ = true);

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

    bool containsByName(const QString &argumentName_, bool isAny_ = true, bool isInput_ = true) const;
    ArgumentBase *getByName(const QString &argumentName_, bool isAny_ = true, bool isInput_ = true);
    const ArgumentBase *getByName(const QString &argumentName_, bool isAny_ = true, bool isInput_ = true) const;
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
    virtual QVector<GLint> &intValues() = 0;
    virtual QVector<GLfloat> &floatValues() = 0;
    virtual QVector<QString> &stringValues() = 0;
    virtual ArgumentValueDataArray * copy() const = 0;

    int getArraySize() const;
    int getTupleSize() const;
    bool isInitialized() const;
    bool isDefaultValue() const;

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
        m_isInitialized = true;
        m_isDefault = false;
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
            m_isInitialized = true;
            m_isDefault = false;
        }
        else
        {
            if(m_isAttribute || m_isIndex)
            {
                extractValues(value_, values_, m_arraySize);
                m_isInitialized = true;
                m_isDefault = false;
            }
            else if(m_isUniform)
            {
                extractValues(value_, values_, values_.size());
                m_isInitialized = true;
                m_isDefault = false;
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
    bool m_isInitialized = false;
    bool m_isDefault = true;

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
    virtual void setArray(const QVariantList &varValues_) override;
    virtual QVariantList variantValues() const override;
    virtual OpenGLArgumentValueBase *createOpenGlValue() override;
    virtual QVector<GLint> &intValues() override;
    virtual QVector<GLfloat> &floatValues() override;
    virtual QVector<QString> &stringValues() override;
    virtual ArgumentValueDataArray *copy() const override;

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

    virtual void initData() override;
    virtual void setArray(const QVariantList &varValues_) override;
    virtual QVariantList variantValues() const override;
    virtual OpenGLArgumentValueBase *createOpenGlValue() override;
    virtual QVector<GLint> &intValues() override;
    virtual QVector<GLfloat> &floatValues() override;
    virtual QVector<QString> &stringValues() override;
    virtual ArgumentValueDataArray *copy() const override;

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
    ArgumentValueDataStringArray(
            const ArgumentBase &from_,
            int arraySize_,
            int tupleSize_,
            bool isTextureType_
            );
    virtual ~ArgumentValueDataStringArray() override = default;

    virtual void initData() override;
    virtual void setArray(const QVariantList &varValues_) override;
    virtual QVariantList variantValues() const override;
    virtual OpenGLArgumentValueBase *createOpenGlValue() override;
    virtual QVector<GLint> &intValues() override;
    virtual QVector<GLfloat> &floatValues() override;
    virtual QVector<QString> &stringValues() override;
    virtual ArgumentValueDataArray *copy() const override;

    const QVector<QString> &getValues() const;
    bool isMatrixType() const;

protected:
    virtual void setArray(const QString &value_) override;

private:
    QVector<QString> m_values;
    bool m_isTextureType = false;
};


class DataTableArgumentValue
{
public:
    DataTableArgumentValue();
    ~DataTableArgumentValue();

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
    DataTableValue *findChild(const QString &key_);
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
    DataTableArgumentValue m_argument;
    int *m_intValue = nullptr;      // TODO: maybe, use QVariant
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
            int stepIndex,
            const ArgumentBase &argument_,
            int effectArgumentId_);
    void add(
            const ArgumentDataTable &data_);    // add all from data table
    void add(
            const ArgumentDataTable &data_,
            const ArgumentList &argumentList_); // add output of the list
    ArgumentValueDataArray *find(
            const QString &objectName_,
            int stepIndex,
            const ArgumentBase &argument_);
    ArgumentBase *find(
            int effectArgumentId_);
    ArgumentDataTable* slice(
            const QString &objectName_,
            int stepIndex,
            const ArgumentList &argumentList_); // get input of the list

private:
    bool hasRootChild(const QString &key_) const;
    DataTableValue *findRootChild(const QString &key_);
    DataTableValue *getRootChild(const QString &key_);
    DataTableValue *findArgument(
            const QString &objectName_,
            const QString &stepIndexStr_,
            const QString &argumentName_);
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
            int stepIndex,
            const ArgumentBase &argument_,
            int effectArgumentId_);
    void add(
            const ArgumentDataTable &data_);    // add all from data table
    void add(
            const ArgumentDataTable &data_,
            const ArgumentList &argumentList_); // add output of the list
    ArgumentValueDataArray *find(
            const QString &objectName_,
            int stepIndex,
            const ArgumentBase &argument_);
    ArgumentBase *find(
            int effectArgumentId_);
    ArgumentDataTable* slice(
            const QString &objectName_,
            int stepIndex,
            const ArgumentList &argumentList_); // get input of the list
private:
    ArgumentDataTable m_data;
};


class OpenGLArgumentValueBase
{
public:
    virtual ~OpenGLArgumentValueBase() = default;

    virtual bool valueOf(const ArgumentValueDataArray *valueDataArray_) const = 0;
    virtual void create(QOpenGLShaderProgram *program_) = 0;
    virtual void bind(QOpenGLShaderProgram *program_) = 0;
    virtual bool isTexture() const;         // to count textures
    virtual QString getTextureName() const;    // to find texture in the drawing image data
    virtual void setTextureIndex(int textureIndex_);    // to setup texture indexes, in revers order -- last 0
    virtual void createTexture(const QImage &image_);
    virtual int getArraySize() const = 0;       // internaly used (do not know the purpose of this function)
    virtual int getMaxIndex() const = 0;        // max index value to allocate data
    virtual void setMaxIndex(int maxIndex_);
    virtual int getVBOPartSize() const = 0;     // vbo size of this part
    virtual void setVBOPartOffset(int offset_) = 0; // set offset of vbo data (data follows in chain)
    virtual void writeVBOPart(QOpenGLBuffer *vbo_, int offset_, int sizeItems_) const = 0;  // write vbo part to draing buffer
    virtual void use(QOpenGLShaderProgram *program_) const = 0;
    virtual void bindTexture(QOpenGLFunctions *f_) const;
    virtual void draw(QOpenGLFunctions *f_) const = 0;
    virtual void release(QOpenGLShaderProgram *program_) const = 0;

protected:
    void initAttribureValueId(QOpenGLShaderProgram *program_, const QString &name_);
    void bindAttribureValueId(QOpenGLShaderProgram *program_, const QString &name_);
    void useAttributeValue(QOpenGLShaderProgram *program_, GLenum type_, int offset_, int tupleSize_) const;
    void writeAttributeValue(QOpenGLBuffer *vbo_, int offset_, int sizeItems_, const QVector<GLint> &values_, int partSize_, int tupleSize_) const;
    void writeAttributeValue(QOpenGLBuffer *vbo_, int offset_, int sizeItems_, const QVector<GLfloat> &values_, int partSize_, int tupleSize_) const;
    void writeAttributeValue(QOpenGLBuffer *vbo_, int offset_, int sizeItems_, const QVector<QString> &values_, int partSize_, int tupleSize_) const;
    void releaseAttributeValue(QOpenGLShaderProgram *program_) const;

    void initUniformValueId(QOpenGLShaderProgram *program_, const QString &name_);
    void setUniformValue(QOpenGLShaderProgram *program_, const QVector<GLint> &values_, int arraySize_, bool isMatrixType) const;
    void setUniformValue(QOpenGLShaderProgram *program_, const QVector<GLfloat> &values_, int arraySize_, bool isMatrixType) const;
    void setUniformValue(QOpenGLShaderProgram *program_, const QVector<QString> &values_, int arraySize_, bool isMatrixType) const;

    void drawTrianlesArray(QOpenGLFunctions *f_, int offset_, int size_) const;

    void createTextureFromImage(QOpenGLTexture *&texture_, const QImage &image_);
    void bindTexture(QOpenGLFunctions *f_, QOpenGLTexture *texture_, int textureIndex_) const;
private:
    int m_id = -1;
};


template<class ArgumentValueDataArrayType_>
class OpenGLArgumentUniformValueT :
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
    OpenGLArgumentUniformValueT(const ArgumentValueDataArrayType_ *argumentValueDataArray_)
        :m_valueDataArray(argumentValueDataArray_)
    {
    }

    virtual bool valueOf(const ArgumentValueDataArray *valueDataArray_) const
    {
        return static_cast<const ArgumentValueDataArray *>(m_valueDataArray) == valueDataArray_;
    }

    virtual void create(QOpenGLShaderProgram *program_) override
    {
        OpenGLArgumentValueBase::initUniformValueId(
                    program_,
                    arg().getName());
    }

    virtual void bind(QOpenGLShaderProgram *program_) override
    {
        Q_UNUSED(program_);
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
        OpenGLArgumentValueBase::setUniformValue(program_,
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
        return static_cast<const ArgumentBase&>(*m_valueDataArray);
    }

    const ArgumentValueDataArrayType_ &value() const
    {
        return static_cast<const ArgumentValueDataArrayType_&>(*m_valueDataArray);
    }

private:
    const ArgumentValueDataArrayType_ *m_valueDataArray = nullptr;
};


template<class ArgumentValueDataArrayType_>
class OpenGLArgumentTextureValueT :
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
    OpenGLArgumentTextureValueT(const ArgumentValueDataArrayType_ *argumentValueDataArray_)
        :m_valueDataArray(argumentValueDataArray_)
    {
    }

    virtual ~OpenGLArgumentTextureValueT() override
    {
        delete m_texture;
        m_texture = nullptr;
    }

    virtual bool valueOf(const ArgumentValueDataArray *valueDataArray_) const
    {
        return static_cast<const ArgumentValueDataArray *>(m_valueDataArray) == valueDataArray_;
    }

    virtual void create(QOpenGLShaderProgram *program_) override
    {
        OpenGLArgumentValueBase::initUniformValueId(
                    program_,
                    arg().getName());
    }

    virtual void bind(QOpenGLShaderProgram *program_) override
    {
        Q_UNUSED(program_);
    }

    virtual bool isTexture() const override
    {
        return true && value().getValues().size() > 0;
    }

    virtual QString getTextureName() const override
    {
        if(value().getValues().size() > 0)
        {
            return value().getValues()[0];
        }
        else
        {
            return QString();
        }
    }

    virtual void setTextureIndex(int textureIndex_)
    {
        m_textureIndex = textureIndex_;
    }

    virtual void createTexture(const QImage &image_) override
    {
        delete m_texture;
        m_texture = nullptr;
        createTextureFromImage(m_texture, image_);
    }

    virtual int getArraySize() const override
    {
        return 0;
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
        if(m_textureIndex < 0) { return; }
        OpenGLArgumentValueBase::setUniformValue(program_,
                    QVector<GLint>({m_textureIndex, }),
                    1,
                    false);
    }

    virtual void bindTexture(QOpenGLFunctions *f_) const override
    {
        OpenGLArgumentValueBase::bindTexture(f_, m_texture, m_textureIndex);
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
        return static_cast<const ArgumentBase&>(*m_valueDataArray);
    }

    const ArgumentValueDataArrayType_ &value() const
    {
        return static_cast<const ArgumentValueDataArrayType_&>(*m_valueDataArray);
    }

private:
    int m_textureIndex = -1;
    QOpenGLTexture *m_texture = nullptr;
    const ArgumentValueDataArrayType_ *m_valueDataArray = nullptr;
};


template<typename Type_>
struct TypeToGLTypeEnum
{
    constexpr static GLenum value = GL_BYTE;
    constexpr static GLenum sizeOfType = 1;
};

template<>
struct TypeToGLTypeEnum<GLint>
{
    constexpr static GLenum value = GL_INT;
    constexpr static GLenum sizeOfType = sizeof(GLint);
};

template<>
struct TypeToGLTypeEnum<GLfloat>
{
    constexpr static GLenum value = GL_FLOAT;
    constexpr static GLenum sizeOfType = sizeof(GLfloat);
};


template<class ArgumentValueDataArrayType_>
class OpenGLArgumentAttributeValueT :
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
    OpenGLArgumentAttributeValueT(const ArgumentValueDataArrayType_ *argumentValueDataArray_)
        :m_valueDataArray(argumentValueDataArray_)
    {
    }

    virtual bool valueOf(const ArgumentValueDataArray *valueDataArray_) const
    {
        return static_cast<const ArgumentValueDataArray *>(m_valueDataArray) == valueDataArray_;
    }

    virtual void create(QOpenGLShaderProgram *program_) override
    {
        OpenGLArgumentValueBase::initAttribureValueId(
                    program_,
                    arg().getName());
    }

    virtual void bind(QOpenGLShaderProgram *program_) override
    {
        OpenGLArgumentValueBase::bindAttribureValueId(
                    program_,
                    arg().getName());
    }

    virtual int getArraySize() const override
    {
        return value().getValues().size();
    }

    virtual int getMaxIndex() const override
    {
        return ((getArraySize() + valueBase().getTupleSize() - 1) / valueBase().getTupleSize());
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
        OpenGLArgumentValueBase::writeAttributeValue(
                    vbo_,
                    offset_,
                    sizeItems_,
                    value().getValues(),
                    getVBOPartSize() * TypeToGLTypeEnum<ItemType>::sizeOfType,
                    valueBase().getTupleSize());
    }

    virtual void use(QOpenGLShaderProgram *program_) const override
    {
        OpenGLArgumentValueBase::useAttributeValue(
                    program_,
                    TypeToGLTypeEnum<ItemType>::value,
                    m_offset,
                    valueBase().getTupleSize());
    }

    virtual void draw(QOpenGLFunctions *f_) const override
    {
        Q_UNUSED(f_);
    }

    virtual void release(QOpenGLShaderProgram *program_) const override
    {
        OpenGLArgumentValueBase::releaseAttributeValue(program_);
    }

private:
    const ArgumentBase &arg() const
    {
        return static_cast<const ArgumentBase&>(*m_valueDataArray);
    }

    const ArgumentValueDataArrayType_ &value() const
    {
        return static_cast<const ArgumentValueDataArrayType_&>(*m_valueDataArray);
    }

    const ArgumentValueDataArray &valueBase() const
    {
        return static_cast<const ArgumentValueDataArray&>(value());
    }

private:
    int m_offset = -1;
    const ArgumentValueDataArrayType_ *m_valueDataArray = nullptr;
};


template<class ArgumentValueDataArrayType_>
class OpenGLArgumentIndexValueT :
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
    OpenGLArgumentIndexValueT(const ArgumentValueDataArrayType_ *argumentValueDataArray_)
        :m_valueDataArray(argumentValueDataArray_)
    {
    }

    virtual bool valueOf(const ArgumentValueDataArray *valueDataArray_) const
    {
        return static_cast<const ArgumentValueDataArray *>(m_valueDataArray) == valueDataArray_;
    }

    virtual void create(QOpenGLShaderProgram *program_) override
    {
        Q_UNUSED(program_);
    }

    virtual void bind(QOpenGLShaderProgram *program_) override
    {
        Q_UNUSED(program_);
    }

    virtual int getArraySize() const override
    {
        return 0;
    }

    virtual int getMaxIndex() const override
    {
        return 0;
    }

    virtual void setMaxIndex(int maxIndex_) override
    {
        m_maxIndex = maxIndex_;
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
        if(value().getValues().size() == 0) { return; }
        const int elements = value().getValues()[0];
        for(int i = 0; i < elements; i++)
        {
            OpenGLArgumentValueBase::drawTrianlesArray(
                        f_,
                        (m_maxIndex * (i + 0)) / elements,
                        (m_maxIndex * (i + 1)) / elements
                        );
        }
    }

    virtual void release(QOpenGLShaderProgram *program_) const override
    {
        Q_UNUSED(program_);
    }

private:
    const ArgumentBase &arg() const
    {
        return static_cast<const ArgumentBase&>(*m_valueDataArray);
    }

    const ArgumentValueDataArrayType_ &value() const
    {
        return static_cast<const ArgumentValueDataArrayType_&>(*m_valueDataArray);
    }

private:
    int m_maxIndex = 0;
    const ArgumentValueDataArrayType_ *m_valueDataArray = nullptr;
};


class Effect;
class EffectArgSet;
class Artefact;
class ArtefactArgModel;


class IQuizImageDataArtefact
{
public:
    IQuizImageDataArtefact() = default;
    virtual ~IQuizImageDataArtefact() = default;

    bool setArtefact(const Artefact *artefact_, int stepIndex_);
    int getStepIndex() const;
    const ArgumentList &getArguments() const;

    static IQuizImageDataArtefact *create(const Artefact *artefact_, int stepIndex_);

    int getId() const;
    const QString &filename() const;
    virtual bool isVertexShader() const;
    virtual bool isFragmentShader() const;
    bool isTexture() const;
    virtual QString getVertexShader() const;
    virtual QString getFragmentShader() const;
    virtual const QImage *getTexture() const;

protected:
    virtual bool setData(const QByteArray &data_) = 0;

protected:
    bool setArguments(const ArtefactArgModel *args_);
    bool setArguments(const QString &shaderCode_);

private:
    int m_id = -1;
    int m_stepIndex = 0;
    QString m_filename;
    ArgumentList m_arguments;
};

class QuizImageDataVertexArtefact : public IQuizImageDataArtefact
{
public:
    QuizImageDataVertexArtefact() = default;

    virtual bool isVertexShader() const override;
    virtual QString getVertexShader() const override;

protected:
    virtual bool setData(const QByteArray &data_) override;

private:
    QString m_vertexShader;
};


class QuizImageDataFragmentArtefact : public IQuizImageDataArtefact
{
public:
    QuizImageDataFragmentArtefact() = default;

    virtual bool isFragmentShader() const override;
    virtual QString getFragmentShader() const override;

protected:
    virtual bool setData(const QByteArray &data_) override;

private:
    QString m_fragmentShader;
};


class QuizImageDataTexture1DArtefact : public IQuizImageDataArtefact
{
public:
    QuizImageDataTexture1DArtefact() = default;

    virtual const QImage *getTexture() const override;

protected:
    virtual bool setData(const QByteArray &data_) override;

private:
    QImage m_texture1D;
};


class QuizImageDataTexture2DArtefact : public IQuizImageDataArtefact
{
public:
    QuizImageDataTexture2DArtefact() = default;

    virtual const QImage *getTexture() const override;

protected:
    virtual bool setData(const QByteArray &data_) override;

private:
    QImage m_texture2D;
};


class QuizImageDataTexture3DArtefact : public IQuizImageDataArtefact
{
public:
    QuizImageDataTexture3DArtefact() = default;

    virtual const QImage *getTexture() const override;

protected:
    virtual bool setData(const QByteArray &data_) override;

private:
    QImage m_texture3D;
};


class QuizImageDataJsonArtefact : public IQuizImageDataArtefact
{
public:
    QuizImageDataJsonArtefact() = default;

protected:
    virtual bool setData(const QByteArray &data_) override;

private:
    QJsonDocument m_document;
};


class QuizImageData3DOBJArtefact : public IQuizImageDataArtefact
{
public:
    QuizImageData3DOBJArtefact() = default;

protected:
    virtual bool setData(const QByteArray &data_) override;

private:
    QJsonDocument m_document;
};


class QuizImageDataConvertJsonArtefact : public QuizImageDataJsonArtefact
{
public:
    QuizImageDataConvertJsonArtefact() = default;
};


class QuizImageDataScriptLuaArtefact : public IQuizImageDataArtefact
{
public:
    QuizImageDataScriptLuaArtefact() = default;

protected:
    virtual bool setData(const QByteArray &data_) override;

private:
    QString m_script;
};


class EffectObjects;


class QuizImageDataObject
{
public:
    QuizImageDataObject() = default;
    ~QuizImageDataObject();

    static QuizImageDataObject *create(const EffectObjects *effectObject_);
    const QString &getProgrammerName() const;
    const QVector<IQuizImageDataArtefact *> &getArtefacts() const;

protected:
    void sortArtefacts();
    void free();

protected:
    QString m_programmerName;
    QVector<IQuizImageDataArtefact *> m_artefacts;
};


class DrawingImageData;
class QuizImageData;


class DrawingArtefact
{
public:
    DrawingArtefact() = default;

    bool operator == (const DrawingArtefact &drawingArtefact_) const;
    bool operator < (const DrawingArtefact &drawingArtefact_) const;
    int getId() const;

private:
    void setId(int id_);

private:
    int m_id = -1;

    friend class DrawingImageData;
    friend class QuizImageData;
};


class DrawingImageData;
class OpenGLDrawingImageData;


class DrawingTextureArtefact : public DrawingArtefact
{
public:
    DrawingTextureArtefact() = default;

    bool operator == (const DrawingTextureArtefact &drawingArtefact_) const;
    bool operator < (const DrawingTextureArtefact &drawingArtefact_) const;
    void deepCopy();
    const QString &getFilename() const;
    const QImage &getImage() const;

    static DrawingTextureArtefact *fromJson(const QJsonDocument &obj_);
    static DrawingTextureArtefact *fromJson(const QJsonObject &obj_);

private:
    void setFilename(const QString &name_);
    void setTexture(const QImage &image_);
    bool loadImage();

private:
    QString m_filename;
    QImage m_image;

    friend class DrawingImageData;
    friend class QuizImageData;
    template<class JsonType>
    friend DrawingTextureArtefact *fromJsonT(const JsonType &obj_, const DrawingTextureArtefact *);
    friend class OpenGLDrawingImageData;
};

class DrawingShaderArtefact : public DrawingArtefact
{
public:
    DrawingShaderArtefact() = default;

    bool operator == (const DrawingShaderArtefact &drawingArtefact_) const;
    bool operator < (const DrawingShaderArtefact &drawingArtefact_) const;
    void deepCopy();
    const QString &getShaderCode() const;

    static DrawingShaderArtefact *fromJson(const QJsonDocument &obj_);
    static DrawingShaderArtefact *fromJson(const QJsonObject &obj_);
    static void toJson(const DrawingShaderArtefact *artefact_, QJsonObject &obj_);

private:
    void setShader(const QString &shaderCode_);
    void setFilename(const QString &filename_);
    const QString &getFilename() const;
    bool loadFile();

private:
    QString m_filename;
    QString m_shaderCode;

    friend class DrawingImageData;
    friend class QuizImageData;
    template<class JsonType>
    friend DrawingShaderArtefact *fromJsonT(const JsonType &obj_, const DrawingShaderArtefact *);
    friend class OpenGLDrawingImageData;
};


class DrawingArgument
{
public:
    DrawingArgument(
            ArgumentValueDataArray *valueDataArray_ = nullptr,
            int position_ = 0);
    ~DrawingArgument();
    QString getArgumentName() const;
    void initData();
    void setValues(const QVector<GLfloat> &values_, int size_);
    void setValues(const QVector<GLint> &values_, int size_);
    void getValues(QVector<GLfloat> &values_) const;
    void getValues(QVector<GLint> &values_) const;
    void setValue(const QString &value_);
    int getTupleSize() const;
    bool isInitialized() const;
    bool operator == (const DrawingArgument &argument_) const;
    bool operator < (const DrawingArgument &argument_) const;
    DrawingArgument * copy() const;

private:
    const QVector<GLint> &intValues() const;
    const QVector<GLfloat> &floatValues() const;
    const QVector<QString> &stringValues() const;
    bool doesValueEqual(const DrawingArgument &argument_) const;
    OpenGLArgumentValueBase *createOpenglValue();
    const ArgumentValueDataArray *getValueDataArray() const;

private:
    ArgumentValueDataArray *m_valueDataArray = nullptr;
    int m_position = 0;
    friend class OpenGLDrawingImageData;
};


class OpenGLDrawingStepImageData
{
public:
    OpenGLDrawingStepImageData() = default;
    ~OpenGLDrawingStepImageData();

    // init api
    //void clearArgumentIds();
    bool buildProgram(QString &errorLog_);
    bool isProgramBuilded() const;
    void createArguments();
    void createTextures();
    void bind();
    void buildVBO();
    void writeVBO();

    // draw api
    void bindProgramAndVBO();
    void useArguments() const;
    void bindTextures(QOpenGLFunctions *f_) const;
    void draw(QOpenGLFunctions *f_) const;
    void release() const;

private:
    void free();

private:
    const DrawingShaderArtefact *m_vertexArtefact = nullptr;
    const DrawingShaderArtefact *m_fragmentArtefact = nullptr;
    QVector<OpenGLArgumentValueBase *> m_programArguments;
    QVector<const DrawingTextureArtefact *> m_argumentTextures;       // for every argument we set or nullptr
                                                                // or texture artefact for it
                                                                // associate textures in object creation time
    QByteArray m_vertexDataBA;
    QByteArray m_fragmentDataBA;
    QOpenGLShaderProgram *m_program = nullptr;
    QOpenGLShader *m_vertexShader = nullptr;
    QOpenGLShader *m_fragmentShader = nullptr;
    QOpenGLBuffer *m_vbo = nullptr;
    QVector<GLfloat> m_vboData;

    friend class OpenGLDrawingImageData;
};


class OpenGLDrawingImageData
{
public:
    OpenGLDrawingImageData() = default;
    ~OpenGLDrawingImageData();

    bool isInitialized() const;
    void setRenderArgumentValue(const QString &argumentName_, const QVector<GLfloat> & values_, int size_);
    void setRenderArgumentValue(const QString &argumentName_, const QVector<GLint> & values_, int size_);
    void getArgumentValue(const QString &argumentName_, QVector<GLfloat> & values_) const;
    void getArgumentValue(const QString &argumentName_, QVector<GLint> & values_) const;
    int getTupleSize(const QString &argumentName_) const;
    bool isArgumentInitialized(const QString &argumentName_) const;
    bool getTextureSize(const QString &argumentName_, QSize &size_) const;
    void addRenderImage(const QString &filename_, bool fromImage_);

    int stepCount() const;
    //void clearStepArgumentIds(int stepIndex_);
    bool buildStepProgram(int stepIndex_, QString &errorLog_);
    bool isStepProgramBuilded(int stepIndex_) const;
    void createStepArgument(int stepIndex_);
    void createStepTextures(int stepIndex_);
    void bindStep(int stepIndex_);
    void buildStepVBO(int stepIndex_);

    void bindStepProgramAndVBO(int stepIndex_);
    void writeStepVBO(int stepIndex_) const;
    void useStepArguments(int stepIndex_) const;
    void bindStepTexture(int stepIndex_, QOpenGLFunctions *f_) const;
    void drawStep(int stepIndex_, QOpenGLFunctions *f_) const;
    void releaseStep(int stepIndex_) const;

    static OpenGLDrawingImageData *fromJson(const QJsonDocument &doc_);

protected:
    void findArgumentsRange(
            const QString &argumentName_,
            QVector<DrawingArgument *>::iterator &itb_
            );
    void findArgumentsRange(
            const QString &argumentName_,
            QVector<DrawingArgument *>::const_iterator &itb_
            ) const;
    void free();

private:
    QVector<OpenGLDrawingStepImageData *> m_steps;
    QVector<DrawingArgument *> m_arguments;
    QVector<const DrawingTextureArtefact *> m_textures;
    QVector<const DrawingShaderArtefact *> m_vertexShaders;
    QVector<const DrawingShaderArtefact *> m_fragmentShaders;
    bool m_initialized = false;

private:
    friend class DrawingImageData;
};


class DrawingImageData
{
public:
    DrawingImageData() = default;

    OpenGLDrawingImageData *copy();

private:
    void setObjects(const QVector<QuizImageDataObject *> &objects_);
    void setArtefacts(const QVector<QuizImageDataObject *> &objects_);
    //void addTexture(const DrawingTextureArtefact &argtefact_);
    void setAllArgumentValues(ArgumentValueDataArray *argumentValueDataArray_);

private:
    // for drawing data creator thread
    std::multiset<DrawingArgument> m_argumentsSet;
    std::set<DrawingTextureArtefact> m_texturesSet;
    std::set<DrawingShaderArtefact> m_vertexShadersSet;
    std::set<DrawingShaderArtefact> m_fragmentShadersSet;

private:
    friend class QuizImageData;
};

class QuizImageData
{
public:
    QuizImageData();
    ~QuizImageData();

    void setFromImageUrl(const QString &fromImageUrl_);
    void setToImageUrl(const QString &toImageUrl_);
    void swapImages();
    void setEffect(const Effect *effect_);
    void setArgumentSet(const EffectArgSet *argumentSet_);
    bool isEffectChanged() const;
    void prepareDrawingData();
    OpenGLDrawingImageData *getDrawingData();

protected:
    bool fromImageUrlChanged() const;
    void useNewFromImageUrl();
    bool toImageUrlChanged() const;
    void useNewToImageUrl();

    bool isSwapImages() const;

    const QString &getFromImageUrl() const;
    const QString &getToImageUrl() const;

    void useNewEffect();

    bool canUpdateEffect(const Effect *effect_) const;
    bool differentEffect(const Effect *effect_) const;
    void clearEffect();
    void free();
    void freeObjects();
    void setImagesToDrawingData();

protected:
    QString m_newFromImageUrl;
    QString m_newToImageUrl;
    QString m_fromImageUrl;
    QString m_toImageUrl;
    bool m_effectIsChanged = false;
    int m_oldEffectId = -1;
    QVector<QuizImageDataObject *> m_objects;
    DrawingImageData m_drawingData;
};


#endif


#endif // QUIZIMAGEDATA_H
