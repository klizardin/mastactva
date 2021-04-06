#include "quizimagedata.h"
#include <set>
#include "../MastactvaBase/serverfiles.h"
#include "../MastactvaBase/utils.h"
#include "../MastactvaModels/artefacttype.h"
#include "../MastactvaModels/artefactargstorage.h"
#include "../MastactvaModels/artefactargtype.h"
#include "../MastactvaModels/objectartefact.h"
#include "../MastactvaModels/effect.h"
#include "../MastactvaBase/wavefrontobj.h"
#include "../MastactvaBase/utils.h"


static const int g_baseTableIndex = 1;


bool ArgumentBase::set(
        const ArtefactArg *arg_,
        ArtefactArgStorageModel* argStorageModel_ /* = nullptr*/,
        ArtefactArgTypeModel *argTypeModel_ /* = nullptr*/
        )
{
    if(nullptr == arg_) { return false; }
    if(arg_->name().isEmpty()) { return false; }
    setName(arg_->name());

    if(nullptr == argStorageModel_)
    {
        argStorageModel_ =
            static_cast<ArtefactArgStorageModel*>(
                QMLObjectsBase::getInstance().getListModel(g_artefactArgStorageModel)
                );
    }
    if(nullptr == argStorageModel_) { return false; }
    ArtefactArgStorage *storage = argStorageModel_->findDataItemByAppIdImpl(arg_->argStorageId());
    if(nullptr == storage) { return false; }
    setStorage(storage->storage());
    if(nullptr == argTypeModel_)
    {
        argTypeModel_ =
            static_cast<ArtefactArgTypeModel*>(
                QMLObjectsBase::getInstance().getListModel(g_artefactArgTypeModel)
                );
    }
    if(nullptr == argTypeModel_) { return false; }
    ArtefactArgType *type = argTypeModel_->findDataItemByAppIdImpl(arg_->argTypeId());
    if(nullptr == type) { return false; }
    setType(type->type());
    setDefaultValue(arg_->defaultValue());
    return true;
}

const QString &ArgumentBase::getName() const
{
    return m_name;
}

void ArgumentBase::setName(const QString &name_)
{
    m_name = name_;
}

const QString &ArgumentBase::getStorage() const
{
    return m_storage;
}

void ArgumentBase::setStorage(const QString &storage_)
{
    m_storage = storage_;
}

const QString &ArgumentBase::getType() const
{
    return m_type;
}

void ArgumentBase::setType(const QString &type_)
{
    m_type = type_;
}

const QString &ArgumentBase::getValue() const
{
    return m_value;
}

void ArgumentBase::setValue(const QString &value_)
{
    m_value = value_;
}

const QString &ArgumentBase::getDefaultValue() const
{
    return m_defaultValue;
}

void ArgumentBase::setDefaultValue(const QString &defaultValue_)
{
    m_defaultValue = defaultValue_;
}

bool ArgumentBase::isInput() const
{
    return m_isInput;
}

bool ArgumentBase::isOutput() const
{
    return !m_isInput;
}

void ArgumentBase::setInput(bool isInput_)
{
    m_isInput = isInput_;
}

template<typename EnumType_>
constexpr std::underlying_type_t<EnumType_> toUnderlaying(EnumType_ ev_) noexcept
{
    return {static_cast<std::underlying_type_t<EnumType_>>(ev_)};
}

ArgumentValueDataArray *ArgumentBase::createValueDataArray() const
{
    enum class Fields : int {name, arraySize, isIntType, isFloatType, isMatrixType, isString, isTexture, tupleSize};
    using TypeInfo = std::tuple<const char *, int, bool, bool, bool, bool, bool, int>;
    static const TypeInfo typeInfos[] = {
        { g_intTypeName, 1,         true,  false, false, false, false, 1 },
        { g_floatTypeName, 1,       false, true,  false, false, false, 1 },
        { g_vec2TypeName, 2,        false, true,  false, false, false, 2 },
        { g_vec3TypeName, 3,        false, true,  false, false, false, 3 },
        { g_vec4TypeName, 4,        false, true,  false, false, false, 4 },
        { g_mat2TypeName, 4,        false, true,  true,  false, false, 4 },
        { g_mat3TypeName, 9,        false, true,  true,  false, false, 9 },
        { g_mat4TypeName, 16,       false, true,  true,  false, false, 16 },
        { g_stringsTypeName, -1,    false, false, false, true,  false, 1 },
        { g_sampler1DTypeName, 1,   false, false, false, true,  true,  1 },
        { g_sampler2DTypeName, 1,   false, false, false, true,  true,  1 },
        { g_sampler3DTypeName, 1,   false, false, false, true,  true,  1 },
    };

    const auto fit = std::find_if(
                std::begin(typeInfos),
                std::end(typeInfos),
                [this] (const TypeInfo &ti_) -> bool
    {
        return std::get<toUnderlaying(Fields::name)>(ti_) == getType();
    });

    Q_ASSERT(std::end(typeInfos) != fit);   // unknown type
    if(std::end(typeInfos) == fit) { return nullptr; }

    const int arraySize = std::get<toUnderlaying(Fields::arraySize)>(*fit);
    const bool isIntArrayType = std::get<toUnderlaying(Fields::isIntType)>(*fit);
    const bool isFloatArrayType = std::get<toUnderlaying(Fields::isFloatType)>(*fit);
    const bool isMatrixType = std::get<toUnderlaying(Fields::isMatrixType)>(*fit);
    const bool isStringArrayType = std::get<toUnderlaying(Fields::isString)>(*fit);
    const bool isTextureType = std::get<toUnderlaying(Fields::isTexture)>(*fit);
    const int tupleSize = std::get<toUnderlaying(Fields::tupleSize)>(*fit);

    if(isIntArrayType)
    {
        return new ArgumentValueDataIntArray(*this, arraySize, tupleSize);
    }
    else if(isFloatArrayType)
    {
        return new ArgumentValueDataFloatArray(*this, arraySize, tupleSize, isMatrixType);
    }
    else if(isStringArrayType)
    {
        return new ArgumentValueDataStringArray(*this, arraySize, tupleSize, isTextureType);
    }
    else
    {
        return nullptr;
    }
}

void ArgumentBase::deepCopy()
{
    m_name = QString(m_name.constData(), m_name.length());
    m_storage = QString(m_storage.constData(), m_storage.length());
    m_type = QString(m_type.constData(), m_type.length());
    m_value = QString(m_value.constData(), m_value.length());
    m_defaultValue = QString(m_defaultValue.constData(), m_defaultValue.length());
}

template<class JSonType_>
ArgumentBase *fromJsonT(const JSonType_ &obj_, bool isInput_, const ArgumentBase *)
{
    ArgumentBase *result = new ArgumentBase();
    result->setInput(isInput_);

    QJsonValue jsvName = obj_[g_argumentNameName];
    if(!jsvName.isUndefined() && jsvName.isString())
    {
        result->setName(jsvName.toString());
    }
    else
    {
        delete result;
        result = nullptr;
        return result;
    }

    QJsonValue jsvStorage = obj_[g_argumentStorageName];
    if(!jsvStorage.isUndefined() && jsvStorage.isString())
    {
        result->setStorage(jsvStorage.toString());
    }
    else
    {
        delete result;
        result = nullptr;
        return result;
    }

    QJsonValue jsvType = obj_[g_argumentTypeName];
    if(!jsvType.isUndefined() && jsvType.isString())
    {
        result->setType(jsvType.toString());
    }
    else
    {
        delete result;
        result = nullptr;
        return result;
    }

    QJsonValue jsvValue = obj_[g_argumentValueName];
    bool hasValue = false;
    if(!jsvValue.isUndefined() && jsvValue.isString())
    {
        result->setValue(jsvValue.toString());
        hasValue = true;
    }

    QJsonValue jsvDefaultValue = obj_[g_argumentDefaultValueName];
    if(!jsvDefaultValue.isUndefined() && jsvDefaultValue.isString())
    {
        result->setDefaultValue(jsvDefaultValue.toString());
    }
    else if(!hasValue)
    {
        delete result;
        result = nullptr;
        return result;
    }

    return result;
}

ArgumentBase *ArgumentBase::fromJson(const QJsonDocument &obj_, bool isInput_ /*= true*/)
{
    return fromJsonT(obj_, isInput_, static_cast<const ArgumentBase *>(nullptr));
}

ArgumentBase *ArgumentBase::fromJson(const QJsonObject &obj_, bool isInput_ /*= true*/)
{
    return fromJsonT(obj_, isInput_, static_cast<const ArgumentBase *>(nullptr));
}


bool ArgumentList::containsByName(
        const QString &argumentName_,
        bool isAny_ /*= true*/,
        bool isInput_ /*= true*/) const
{
    return nullptr != getByName(argumentName_, isAny_, isInput_);
}

ArgumentBase *ArgumentList::getByName(
        const QString &argumentName_,
        bool isAny_ /*= true*/,
        bool isInput_ /*= true*/
        )
{
    auto fit = std::find_if(
                std::begin(*this),
                std::end(*this),
                [&argumentName_, isAny_, isInput_](const ArgumentBase &arg_) -> bool
    {
        return arg_.getName() == argumentName_ && (isAny_ || (!isAny_ && arg_.isInput() == isInput_));
    });
    return std::end(*this) != fit ? &*fit : nullptr;
}

const ArgumentBase *ArgumentList::getByName(
        const QString &argumentName_,
        bool isAny_ /*= true*/,
        bool isInput_ /*= true*/
        ) const
{
    return const_cast<ArgumentList *>(this)->getByName(argumentName_, isAny_, isInput_);
}


ArgumentValueDataArray::ArgumentValueDataArray(const ArgumentBase &from_, int arraySize_, int tupleSize_)
    : ArgumentBase(from_),
      m_arraySize(arraySize_),
      m_tupleSize(tupleSize_)
{
    initStorage(getStorage());
}

void ArgumentValueDataArray::initData()
{
    if(!getValue().trimmed().isEmpty())
    {
        setArray(getValue());
        m_isInitialized = true;
        m_isDefault = false;
    }
    else
    {
        setArray(getDefaultValue());
        m_isInitialized = true;
        m_isDefault = true;
    }
}

int ArgumentValueDataArray::getArraySize() const
{
    return m_arraySize;
}

int ArgumentValueDataArray::getTupleSize() const
{
    return m_tupleSize;
}

bool ArgumentValueDataArray::isInitialized() const
{
    return m_isInitialized;
}

bool ArgumentValueDataArray::isDefaultValue() const
{
    return m_isDefault;
}

void ArgumentValueDataArray::initStorage(const QString &storage_)
{
    using StorageInfo = std::tuple<const char *, bool, bool, bool>;
    static const StorageInfo storageInfos[] = {
        { g_attributeStorageName, true, false, false },
        { g_uniformStorageName, false, true, false },
        { g_indexesStorageName, false, false, true },
    };

    const auto fit = std::find_if(
                std::begin(storageInfos),
                std::end(storageInfos),
                [&storage_] (const StorageInfo &si_) -> bool
    {
        return std::get<0>(si_) == storage_;
    });

    Q_ASSERT(std::end(storageInfos) != fit);   // unknown type
    if(std::end(storageInfos) == fit) { return; }

    m_isAttribute = std::get<1>(*fit);
    m_isUniform = std::get<2>(*fit);
    m_isIndex = std::get<3>(*fit);
}


ArgumentValueDataIntArray::ArgumentValueDataIntArray(
        const ArgumentBase &from_,
        int arraySize_,
        int tupleSize_)
    :ArgumentValueDataArray(from_, arraySize_, tupleSize_)
{
}

void ArgumentValueDataIntArray::initData()
{
    initDataT(m_values);
}

void ArgumentValueDataIntArray::setArray(const QVariantList &varValues_)
{
    setArrayT(varValues_, m_values);
}

QVariantList ArgumentValueDataIntArray::variantValues() const
{
    return variantValuesT(m_values);
}

OpenGLArgumentValueBase *ArgumentValueDataIntArray::createOpenGlValue()
{
    if(m_isAttribute)
    {
        return new OpenGLArgumentAttributeValueT<ArgumentValueDataIntArray>(this);
    }
    else if(m_isUniform)
    {
        return new OpenGLArgumentUniformValueT<ArgumentValueDataIntArray>(this);
    }
    else if(m_isIndex)
    {
        return new OpenGLArgumentIndexValueT<ArgumentValueDataIntArray>(this);
    }
    else
    {
        return nullptr;
    }
}

QVector<GLint> &ArgumentValueDataIntArray::intValues()
{
    return valueOrFish(m_values, static_cast<const QVector<GLint> *>(nullptr));
}

QVector<GLfloat> &ArgumentValueDataIntArray::floatValues()
{
    return valueOrFish(m_values, static_cast<const QVector<GLfloat> *>(nullptr));
}

QVector<QString> &ArgumentValueDataIntArray::stringValues()
{
    return valueOrFish(m_values, static_cast<const QVector<QString> *>(nullptr));
}

ArgumentValueDataArray *ArgumentValueDataIntArray::copy() const
{
    ArgumentValueDataIntArray *result = new ArgumentValueDataIntArray(*this);
    result->deepCopy();
    return result;
}

const QVector<GLint> &ArgumentValueDataIntArray::getValues() const
{
    return m_values;
}

bool ArgumentValueDataIntArray::isMatrixType() const
{
    return false;
}


ArgumentValueDataFloatArray::ArgumentValueDataFloatArray(const ArgumentBase &from_, int arraySize_, int tupleSize_, bool isMatrixType_)
    : ArgumentValueDataArray(from_, arraySize_, tupleSize_),
      m_isMatrixType(isMatrixType_)
{
}

void ArgumentValueDataFloatArray::initData()
{
    initDataT(m_values);
}

void ArgumentValueDataFloatArray::setArray(const QVariantList &varValues_)
{
    setArrayT(varValues_, m_values);
}

QVariantList ArgumentValueDataFloatArray::variantValues() const
{
    return variantValuesT(m_values);
}

OpenGLArgumentValueBase *ArgumentValueDataFloatArray::createOpenGlValue()
{
    if(m_isAttribute)
    {
        return new OpenGLArgumentAttributeValueT<ArgumentValueDataFloatArray>(this);
    }
    else if(m_isUniform)
    {
        return new OpenGLArgumentUniformValueT<ArgumentValueDataFloatArray>(this);
    }
    else if(m_isIndex)
    {
        return new OpenGLArgumentIndexValueT<ArgumentValueDataFloatArray>(this);
    }
    else
    {
        return nullptr;
    }
}

QVector<GLint> &ArgumentValueDataFloatArray::intValues()
{
    return valueOrFish(m_values, static_cast<const QVector<GLint> *>(nullptr));
}

QVector<GLfloat> &ArgumentValueDataFloatArray::floatValues()
{
    return valueOrFish(m_values, static_cast<const QVector<GLfloat> *>(nullptr));
}

QVector<QString> &ArgumentValueDataFloatArray::stringValues()
{
    return valueOrFish(m_values, static_cast<const QVector<QString> *>(nullptr));
}

ArgumentValueDataArray *ArgumentValueDataFloatArray::copy() const
{
    ArgumentValueDataFloatArray *result = new ArgumentValueDataFloatArray(*this);
    result->deepCopy();
    return result;
}

const QVector<GLfloat> &ArgumentValueDataFloatArray::getValues() const
{
    return m_values;
}

bool ArgumentValueDataFloatArray::isMatrixType() const
{
    return m_isMatrixType;
}


ArgumentValueDataStringArray::ArgumentValueDataStringArray(
        const ArgumentBase &from_,
        int arraySize_,
        int tupleSize_,
        bool isTextureType_
        )
    : ArgumentValueDataArray(from_, arraySize_, tupleSize_)
    , m_isTextureType(isTextureType_)
{
}

void ArgumentValueDataStringArray::initData()
{
    initDataT(m_values);
}

void ArgumentValueDataStringArray::setArray(const QVariantList &varValues_)
{
    setArrayT(varValues_, m_values);
}

QVariantList ArgumentValueDataStringArray::variantValues() const
{
    return variantValuesT(m_values);
}

OpenGLArgumentValueBase *ArgumentValueDataStringArray::createOpenGlValue()
{
    if(m_isTextureType)
    {
        return new OpenGLArgumentTextureValueT<ArgumentValueDataStringArray>(this);
    }
    else if(m_isAttribute)
    {
        return new OpenGLArgumentAttributeValueT<ArgumentValueDataStringArray>(this);
    }
    else if(m_isUniform)
    {
        return new OpenGLArgumentUniformValueT<ArgumentValueDataStringArray>(this);
    }
    //else if(m_isIndex)
    //{
    //    return new OpenGLArgumentIndexValueT<ArgumentValueDataStringArray>(this);
    //}
    else
    {
        return nullptr;
    }
}

QVector<GLint> &ArgumentValueDataStringArray::intValues()
{
    return valueOrFish(m_values, static_cast<const QVector<GLint> *>(nullptr));
}

QVector<GLfloat> &ArgumentValueDataStringArray::floatValues()
{
    return valueOrFish(m_values, static_cast<const QVector<GLfloat> *>(nullptr));
}

QVector<QString> &ArgumentValueDataStringArray::stringValues()
{
    return valueOrFish(m_values, static_cast<const QVector<QString> *>(nullptr));
}

ArgumentValueDataArray *ArgumentValueDataStringArray::copy() const
{
    ArgumentValueDataStringArray *result = new ArgumentValueDataStringArray(*this);
    for(auto &str_ : result->m_values)
    {
        str_ = QString(str_.constData(), str_.length());
    }
    result->deepCopy();
    return result;
}

const QVector<QString> &ArgumentValueDataStringArray::getValues() const
{
    return m_values;
}

bool ArgumentValueDataStringArray::isMatrixType() const
{
    return false;
}

void ArgumentValueDataStringArray::setArray(const QString &value_)
{
    Q_ASSERT(m_values.size() <= 1);
    if(m_values.size() < 1) { m_values.resize(1); }
    m_values[0] = value_;
}


DataTableArgumentValue::DataTableArgumentValue()
{
    clearEffectArgumentId();
}

DataTableArgumentValue::~DataTableArgumentValue()
{
    freeDataArray();
}

bool DataTableArgumentValue::hasDataArray() const
{
    return nullptr != m_argumentValueDataArray;
}

ArgumentValueDataArray *DataTableArgumentValue::getDataArray() const
{
    return m_argumentValueDataArray;
}

void DataTableArgumentValue::setDataArray(ArgumentValueDataArray *argumentValueDataArray_)
{
    m_argumentValueDataArray = argumentValueDataArray_;
}

void DataTableArgumentValue::freeDataArray()
{
    delete m_argumentValueDataArray;
    m_argumentValueDataArray = nullptr;
}

int DataTableArgumentValue::getEffectArgumentId() const
{
    return m_effectArgumentId;
}

void DataTableArgumentValue::clearEffectArgumentId()
{
    m_effectArgumentId = -1;
}

void DataTableArgumentValue::setEffectArgumentId(int effectArgumentId_)
{
    m_effectArgumentId = effectArgumentId_;
}


DataTableValue::~DataTableValue()
{
    free();
}

bool DataTableValue::hasArgumentDataArray() const
{
    return m_argument.hasDataArray();
}

ArgumentValueDataArray *DataTableValue::getArgumentDataArray()
{
    return m_argument.getDataArray();
}

const ArgumentValueDataArray *DataTableValue::getArgumentDataArray() const
{
    return m_argument.getDataArray();
}

void DataTableValue::set(const ArgumentBase &argument_, int effectArgumentId_)
{
    if(m_argument.hasDataArray() &&
            m_argument.getEffectArgumentId() == effectArgumentId_)
    {
        return;
    }
    if(m_argument.hasDataArray())
    {
        m_argument.freeDataArray();
    }
    m_argument.setDataArray(argument_.createValueDataArray());
    if(m_argument.hasDataArray())
    {
        m_argument.setEffectArgumentId(effectArgumentId_);
    }
    else
    {
        m_argument.clearEffectArgumentId();
    }
}

bool DataTableValue::convertToArgument(const ArgumentBase &templateArgument_)
{
    if(hasValue())
    {
        if(isArgument())
        {
            return m_argument.getDataArray()->getName() == templateArgument_.getName();
        }
        return false;
    }
    m_argument.setDataArray(templateArgument_.createValueDataArray());
    m_argument.getDataArray()->setArray(getChilderenValues());
    return true;
}

bool DataTableValue::hasValue() const
{
    return isArgument() ||
            isIntValue() ||
            isFloatValue() ||
            isStringValue()
            ;
}

bool DataTableValue::isArgument() const
{
    return m_argument.hasDataArray();
}

bool DataTableValue::isIntValue() const
{
    return nullptr != m_intValue;
}

bool DataTableValue::isFloatValue() const
{
    return nullptr != m_floatValue;
}

bool DataTableValue::isStringValue() const
{
    return nullptr != m_stringValue;
}

QVariant DataTableValue::getValue() const
{
    if(isIntValue())
    {
        return QVariant::fromValue(*m_intValue);
    }
    else if(isFloatValue())
    {
        return QVariant::fromValue(*m_floatValue);
    }
    else if(isStringValue())
    {
        return QVariant::fromValue(*m_stringValue);
    }
    else
    {
        return QVariant();
    }
}

void DataTableValue::free()
{
    m_argument.freeDataArray();
    delete m_intValue;
    m_intValue = nullptr;
    delete m_floatValue;
    m_floatValue = nullptr;
    delete m_stringValue;
    m_stringValue = nullptr;
    m_children.clear();
}

bool DataTableValue::hasChild(const QString &key_) const
{
    return m_children.contains(key_);
}

DataTableValue *DataTableValue::findChild(const QString &key_)
{
    auto fit = std::end(m_children);
    if(hasChild(key_))
    {
        fit = m_children.find(key_);
    }
    return std::end(m_children) != fit ? &(fit.value()) : nullptr;
}

DataTableValue *DataTableValue::getChild(const QString &key_)
{
    auto fit = std::end(m_children);
    if(hasChild(key_))
    {
        fit = m_children.find(key_);
    }
    else
    {
        fit = m_children.insert(key_, DataTableValue());
    }
    return std::end(m_children) != fit ? &(fit.value()) : nullptr;
}

void DataTableValue::copyFrom(const DataTableValue &dataTableValue_)
{
    if(dataTableValue_.isArgument())
    {
        ArgumentValueDataArray *valueDataArray = getArgumentDataArray();
        if(nullptr == valueDataArray)
        {
            set(*(dataTableValue_.getArgumentDataArray()),
                dataTableValue_.getArgumentValueEffectArgumentId());
        }
        m_argument.getDataArray()->setArray(
                    dataTableValue_.m_argument.getDataArray()->variantValues());
        return;
    }
    if(dataTableValue_.hasValue())
    {
        if(dataTableValue_.isIntValue())
        {
            setIntValue(dataTableValue_.getValue());
        }
        else if(dataTableValue_.isFloatValue())
        {
            setFloatValue(dataTableValue_.getValue());
        }
        else if(dataTableValue_.isStringValue())
        {
            setStringValue(dataTableValue_.getValue());
        }
    }
}

int DataTableValue::getArgumentValueEffectArgumentId() const
{
    return m_argument.getEffectArgumentId();
}

void DataTableValue::setIntValue(const QVariant &value_)
{
    if(isArgument())
    {
        m_argument.getDataArray()->setArray(QVariantList({value_,}));
    }
    else if(value_.isValid())
    {
        bool ok = false;
        if(nullptr == m_intValue)
        {
            m_intValue = new int{0};
        }
        delete m_floatValue;
        m_floatValue = nullptr;
        delete m_stringValue;
        m_stringValue = nullptr;

        *m_intValue = value_.toInt(&ok);
        if(!ok)
        {
            delete m_intValue;
            m_intValue = nullptr;
        }
    }
}

void DataTableValue::setFloatValue(const QVariant &value_)
{
    if(isArgument())
    {
        m_argument.getDataArray()->setArray(QVariantList({value_,}));
    }
    else if(value_.isValid())
    {
        bool ok = false;
        if(nullptr == m_floatValue)
        {
            m_floatValue = new float{0.0};
        }
        delete m_intValue;
        m_intValue = nullptr;
        delete m_stringValue;
        m_stringValue = nullptr;

        *m_floatValue = value_.toDouble(&ok);
        if(!ok)
        {
            delete m_floatValue;
            m_floatValue = nullptr;
        }
    }
}

void DataTableValue::setStringValue(const QVariant &value_)
{
    if(isArgument())
    {
        m_argument.getDataArray()->setArray(QVariantList({value_,}));
    }
    else if(value_.isValid())
    {
        if(nullptr == m_stringValue)
        {
            m_stringValue = new QString();
        }
        delete m_intValue;
        m_intValue = nullptr;
        delete m_floatValue;
        m_floatValue = nullptr;

        *m_stringValue = value_.toString();
    }
}

QVariantList DataTableValue::getChilderenValues() const
{
    QVariantList res;
    for(int i = g_baseTableIndex;; ++i)
    {
        const QString key = QString::number(i);
        if(!m_children.contains(key)) { break; }
        res.push_back(m_children.value(key).getValue());
    }
    return res;
}

void DataTableValue::freeChilderenValues()
{
    for(int i = g_baseTableIndex;; ++i)
    {
        const QString key = QString::number(i);
        if(!m_children.contains(key)) { break; }
        m_children.remove(key);
    }
}

QList<QString> DataTableValue::getChildrenKeys() const
{
    return m_children.keys();
}


void ArgumentDataTable::add(
        const QString &objectName_,
        int stepIndex,
        const ArgumentBase &argument_,
        int effectArgumentId_)
{
    if(argument_.getName().isEmpty()) { return; }
    DataTableValue *objectValue = getRootChild(objectName_);
    if(nullptr == objectValue) { return; }
    DataTableValue *indexValue = objectValue->getChild(QString::number(stepIndex));
    if(nullptr == indexValue) { return; }
    DataTableValue *argumentValue = indexValue->getChild(argument_.getName());
    argumentValue->set(argument_, effectArgumentId_);
}

void ArgumentDataTable::add(
        const ArgumentDataTable &data_)    // add all from data table
{
    // TODO: check
    QList<QString> rootKeys = data_.m_root.keys();
    for(const auto &objectNameKey_ : qAsConst(rootKeys))
    {
        DataTableValue *objectValue = getRootChild(objectNameKey_);
        if(nullptr == objectValue) { continue; }
        QList<QString> indexNamesKeys = objectValue->getChildrenKeys();
        for(const auto &indexNameKey_ : qAsConst(indexNamesKeys))
        {
            DataTableValue *indexValue = objectValue->getChild(indexNameKey_);
            if(nullptr == indexValue) { continue; }
            QList<QString> argumentNamesKeys = indexValue->getChildrenKeys();
            for(const auto &argumentNameKey_ : qAsConst(argumentNamesKeys))
            {
                DataTableValue *argumentValue = indexValue->getChild(argumentNameKey_);
                if(nullptr == argumentValue) { continue; }
                addArgument(objectNameKey_, indexNameKey_, argumentNameKey_, *argumentValue);
            }
        }
    }
}

void ArgumentDataTable::add(
        const ArgumentDataTable &data_,
        const ArgumentList &argumentList_) // add output of the list
{
    QList<QString> rootKeys = data_.m_root.keys();
    for(const auto &objectNameKey_ : qAsConst(rootKeys))
    {
        DataTableValue *objectValue = getRootChild(objectNameKey_);
        if(nullptr == objectValue) { continue; }
        QList<QString> indexNamesKeys = objectValue->getChildrenKeys();
        for(const auto &indexNameKey_ : qAsConst(indexNamesKeys))
        {
            DataTableValue *indexValue = objectValue->getChild(indexNameKey_);
            if(nullptr == indexValue) { continue; }
            QList<QString> argumentNamesKeys = indexValue->getChildrenKeys();
            for(const auto &argumentNameKey_ : qAsConst(argumentNamesKeys))
            {
                DataTableValue *argumentValue = indexValue->getChild(argumentNameKey_);
                if(nullptr == argumentValue) { continue; }
                if(!argumentList_.containsByName(argumentNameKey_, false, false)) { continue; }
                addArgument(objectNameKey_, indexNameKey_, argumentNameKey_, *argumentValue);
            }
        }
    }
}

ArgumentValueDataArray *ArgumentDataTable::find(
        const QString &objectName_,
        int stepIndex,
        const ArgumentBase &argument_)
{
    DataTableValue *dataTableValue = findArgument(objectName_, QString::number(stepIndex), argument_.getName());
    if(nullptr == dataTableValue) { return nullptr; }
    dataTableValue->convertToArgument(argument_);
    return dataTableValue->getArgumentDataArray();
}

ArgumentBase *ArgumentDataTable::find(
        int effectArgumentId_)
{
    QList<QString> rootKeys = m_root.keys();
    for(const auto &objectNameKey_ : qAsConst(rootKeys))
    {
        DataTableValue *objectValue = getRootChild(objectNameKey_);
        if(nullptr == objectValue) { continue; }
        QList<QString> indexNamesKeys = objectValue->getChildrenKeys();
        for(const auto &indexNameKey_ : qAsConst(indexNamesKeys))
        {
            DataTableValue *indexValue = objectValue->getChild(indexNameKey_);
            if(nullptr == indexValue) { continue; }
            QList<QString> argumentNamesKeys = indexValue->getChildrenKeys();
            for(const auto &argumentNameKey_ : qAsConst(argumentNamesKeys))
            {
                DataTableValue *argumentValue = indexValue->getChild(argumentNameKey_);
                if(nullptr == argumentValue) { continue; }
                if(argumentValue->getArgumentValueEffectArgumentId() == effectArgumentId_)
                {
                    return argumentValue->getArgumentDataArray();
                }
            }
        }
    }
    return nullptr;
}

ArgumentDataTable* ArgumentDataTable::slice(
        const QString &objectName_,
        int stepIndex,
        const ArgumentList &argumentList_) // get input of the list
{
    ArgumentDataTable *res = new ArgumentDataTable();
    DataTableValue *objectValue = findRootChild(objectName_);
    if(nullptr == objectValue) { return res; }
    const QString stepIndexName = QString::number(stepIndex);
    DataTableValue *stepIndexValue = objectValue->findChild(stepIndexName);
    if(nullptr == stepIndexValue) { return res; }
    QList<QString> argumentNamesKeys = stepIndexValue->getChildrenKeys();
    for(const auto &argumentNameKey_ : qAsConst(argumentNamesKeys))
    {
        DataTableValue *argumentValue = stepIndexValue->getChild(argumentNameKey_);
        if(nullptr == argumentValue) { continue; }
        if(!argumentList_.containsByName(argumentNameKey_, false, true)) { continue; }
        res->addArgument(objectName_, stepIndexName, argumentNameKey_, *argumentValue);
    }
    return res;
}

bool ArgumentDataTable::hasRootChild(const QString &key_) const
{
    return m_root.contains(key_);
}

DataTableValue *ArgumentDataTable::findRootChild(const QString &key_)
{
    auto fit = std::end(m_root);
    if(hasRootChild(key_))
    {
        fit = m_root.find(key_);
    }
    return std::end(m_root) != fit ? &(fit.value()) : nullptr;
}

DataTableValue *ArgumentDataTable::getRootChild(const QString &key_)
{
    auto fit = std::end(m_root);
    if(hasRootChild(key_))
    {
        fit = m_root.find(key_);
    }
    else
    {
        fit = m_root.insert(key_, DataTableValue());
    }
    return std::end(m_root) != fit ? &(fit.value()) : nullptr;
}

DataTableValue *ArgumentDataTable::findArgument(
        const QString &objectName_,
        const QString &stepIndexStr_,
        const QString &argumentName_)
{
    DataTableValue *objectValue = findRootChild(objectName_);
    if(nullptr == objectValue) { return nullptr; }
    DataTableValue *indexValue = objectValue->findChild(stepIndexStr_);
    if(nullptr == indexValue) { return nullptr; }
    DataTableValue *argumentValue = indexValue->findChild(argumentName_);
    return argumentValue;
}

DataTableValue *ArgumentDataTable::getArgument(
        const QString &objectName_,
        const QString &stepIndexStr_,
        const QString &argumentName_)
{
    DataTableValue *objectValue = getRootChild(objectName_);
    if(nullptr == objectValue) { return nullptr; }
    DataTableValue *indexValue = objectValue->getChild(stepIndexStr_);
    if(nullptr == indexValue) { return nullptr; }
    DataTableValue *argumentValue = indexValue->getChild(argumentName_);
    return argumentValue;
}

void ArgumentDataTable::addArgument(
        const QString &objectName_,
        const QString &stepIndexStr_,
        const QString &argumentName_,
        const DataTableValue &argumentValue_)
{
    DataTableValue *argumentValue = getArgument(objectName_, stepIndexStr_, argumentName_);
    if(nullptr != argumentValue)
    {
        argumentValue->copyFrom(argumentValue_);
    }
}


void ArgumentsSet::add(
        const QString &objectName_,
        int stepIndex,
        const ArgumentBase &argument_,
        int effectArgumentId_)             // add one element from ArgumentBase and effectArgumentId_
{
    m_data.add(objectName_, stepIndex, argument_, effectArgumentId_);
}

void ArgumentsSet::add(
        const ArgumentDataTable &data_)    // add all from data table
{
    m_data.add(data_);
}

void ArgumentsSet::add(
        const ArgumentDataTable &data_,
        const ArgumentList &argumentList_) // add output of the list
{
    m_data.add(data_, argumentList_);
}

ArgumentValueDataArray *ArgumentsSet::find(
        const QString &objectName_,
        int stepIndex,
        const ArgumentBase &argument_)
{
    return m_data.find(objectName_, stepIndex, argument_);
}

ArgumentBase *ArgumentsSet::find(
        int effectArgumentId_)
{
    return m_data.find(effectArgumentId_);
}

ArgumentDataTable* ArgumentsSet::slice(
        const QString &objectName_,
        int stepIndex,
        const ArgumentList &argumentList_) // get input of the list
{
    return m_data.slice(objectName_, stepIndex, argumentList_);
}



bool OpenGLArgumentValueBase::isTexture() const
{
    return false;
}

QString OpenGLArgumentValueBase::getTextureName() const
{
    return QString();
}

void OpenGLArgumentValueBase::setTextureIndex(int textureIndex_)
{
    Q_UNUSED(textureIndex_);
}

void OpenGLArgumentValueBase::createTexture(const QImage &image_)
{
    Q_UNUSED(image_);
}

void OpenGLArgumentValueBase::setMaxIndex(int maxIndex_)
{
    Q_UNUSED(maxIndex_);
}

void OpenGLArgumentValueBase::use2(QOpenGLShaderProgram *program_) const
{
    Q_UNUSED(program_);
}

void OpenGLArgumentValueBase::bindTexture(QOpenGLFunctions *f_) const
{
    Q_UNUSED(f_);
}

void OpenGLArgumentValueBase::initAttribureValueId(QOpenGLShaderProgram *program_, const QString &name_)
{
    if(nullptr == program_) { return; }
    m_id = program_->attributeLocation(name_);
    //bindAttribureValueId(program_, name_);
    qDebug() << "program_->attributeLocation(" << name_ << " ) = " << m_id;
}

void OpenGLArgumentValueBase::bindAttribureValueId(QOpenGLShaderProgram *program_, const QString &name_)
{
    if(nullptr == program_ ||
            m_id < 0 ||
            name_.isEmpty()) { return; }
    program_->bindAttributeLocation(name_, m_id);
    qDebug() << "program_->bindAttributeLocation(" << name_  << m_id << " )";
}

void OpenGLArgumentValueBase::useAttributeValue(
        QOpenGLShaderProgram *program_,
        GLenum type_,
        int offset_,
        int tupleSize_) const
{
    if(nullptr == program_ ||
            m_id < 0
            ) { return; }
    program_->setAttributeBuffer(m_id, type_, offset_, tupleSize_, 0);
    program_->enableAttributeArray(m_id);
    qDebug() << "program_->setAttributeBuffer(" << m_id << type_ << offset_ << tupleSize_ << 0 << " )";
    qDebug() << "program_->enableAttributeArray(" << m_id << " )";
}

void OpenGLArgumentValueBase::writeAttributeValue(QOpenGLBuffer *vbo_, int offset_, int sizeItems_, const QVector<GLint> &values_, int partSize_, int tupleSize_) const
{
    if(nullptr == vbo_) { return; }
    if(values_.size() > 0 &&
            values_.size() >= sizeItems_ * tupleSize_ &&
            partSize_ <= (int)(tupleSize_ * sizeItems_ * sizeof(GLint))
            )
    {
        vbo_->write(
                    offset_,
                    reinterpret_cast<const void*>(&values_[0]),
                    partSize_
                    );
        qDebug() << "vbo_->write(" << offset_ << "QVector<GLint>" << partSize_ << ")";
    }
}

void OpenGLArgumentValueBase::writeAttributeValue(QOpenGLBuffer *vbo_, int offset_, int sizeItems_, const QVector<GLfloat> &values_, int partSize_, int tupleSize_) const
{
    if(nullptr == vbo_) { return; }
    if(values_.size() > 0 &&
            values_.size() >= sizeItems_ * tupleSize_  &&
            partSize_ <= (int)(tupleSize_ * sizeItems_ * sizeof(GLfloat))
            )
    {
        vbo_->write(
                   offset_,
                   reinterpret_cast<const void*>(&values_[0]),
                   partSize_
                   );
        qDebug() << "vbo_->write(" << offset_ << "QVector<GLfloat>" << partSize_ << ")";
    }
}

void OpenGLArgumentValueBase::writeAttributeValue(QOpenGLBuffer *vbo_, int offset_, int sizeItems_, const QVector<QString> &values_, int partSize_, int tupleSize_) const
{
    Q_UNUSED(vbo_);
    Q_UNUSED(offset_);
    Q_UNUSED(sizeItems_);
    Q_UNUSED(values_);
    Q_UNUSED(partSize_);
    Q_UNUSED(tupleSize_);
}

void OpenGLArgumentValueBase::releaseAttributeValue(QOpenGLShaderProgram *program_) const
{
    if(nullptr == program_ ||
            m_id < 0
            ) { return; }
    program_->disableAttributeArray(m_id);
    qDebug() << "program_->disableAttributeArray(" << m_id << " )";
}

void OpenGLArgumentValueBase::initUniformValueId(QOpenGLShaderProgram *program_, const QString &name_)
{
    if(nullptr == program_) { return; }
    m_id = program_->uniformLocation(name_);
    qDebug() << "program_->uniformLocation(" << name_ << " ) = " << m_id;
}

void OpenGLArgumentValueBase::setUniformValue(
        QOpenGLShaderProgram *program_,
        const QVector<GLint> &values_,
        int arraySize_,
        bool isMatrixType) const
{
    if(nullptr == program_ ||
            m_id < 0
            ) { return; }
    Q_UNUSED(isMatrixType);
    if(arraySize_ >= 1 && values_.size() >= 1)
    {
        program_->setUniformValue(m_id, values_[0]);
        qDebug() << "program_->setUniformValue(" << m_id << values_[0] << " )";
    }
}

void OpenGLArgumentValueBase::setUniformValue(
        QOpenGLShaderProgram *program_,
        const QVector<GLfloat> &values_,
        int arraySize_,
        bool isMatrixType) const
{
    if(nullptr == program_ ||
            m_id < 0
            ) { return; }
    if(1 == arraySize_)
    {
        if(values_.size() >= 1)
        {
            program_->setUniformValue(m_id, values_[0]);
            qDebug() << "program_->setUniformValue(" << m_id << values_[0] << " )";
        }
    }
    else if(2 == arraySize_)
    {
        if(values_.size() >= 2)
        {
            program_->setUniformValue(m_id, values_[0],
                    values_[1]);
            qDebug() << "program_->setUniformValue(" << m_id << values_[0] << values_[1] << " )";
        }
    }
    else if(3 == arraySize_)
    {
        if(values_.size() >= 3)
        {
            program_->setUniformValue(m_id, values_[0],
                    values_[1], values_[2]);
            qDebug() << "program_->setUniformValue(" << m_id << values_[0] << values_[1] << values_[2] << " )";
        }
    }
    else if(4 == arraySize_ && !isMatrixType)
    {
        if(values_.size() >= 4)
        {
            program_->setUniformValue(m_id, values_[0],
                    values_[1], values_[2], values_[3]);
            qDebug() << "program_->setUniformValue(" << m_id << values_[0] << values_[1] << values_[2] << values_[3] << " )";
        }
    }
    else if(4 == arraySize_ && isMatrixType)
    {
        if(values_.size() >= 4)
        {
            QMatrix2x2 m(&values_[0]);
            program_->setUniformValue(m_id, m);
            qDebug() << "program_->setUniformValue(" << m_id << values_[0] << values_[1] << values_[2] << values_[3] << " )";
        }
    }
    else if(9 == arraySize_ && isMatrixType)
    {
        if(values_.size() >= 9)
        {
            QMatrix3x3 m(&values_[0]);
            program_->setUniformValue(m_id, m);
            qDebug() << "program_->setUniformValue(" << m_id
                     << values_[0] << values_[1] << values_[2]
                     << values_[3] << values_[4] << values_[5]
                     << values_[6] << values_[7] << values_[8]
                     << " )";
        }
    }
    else if(16 == arraySize_ && isMatrixType)
    {
        if(values_.size() >= 16)
        {
            QMatrix4x4 m(&values_[0]);
            program_->setUniformValue(m_id, m);
            qDebug() << "program_->setUniformValue(" << m_id
                     << values_[0] << values_[1] << values_[2] << values_[3]
                     << values_[4] << values_[5] << values_[6] << values_[7]
                     << values_[8] << values_[9] << values_[10] << values_[11]
                     << values_[12] << values_[13] << values_[14] << values_[15]
                     << " )";
        }
    }
    else
    {
        Q_ASSERT(false); // unsupported case
    }
}

void OpenGLArgumentValueBase::setUniformValue(
        QOpenGLShaderProgram *program_,
        const QVector<QString> &values_,
        int arraySize_,
        bool isMatrixType) const
{
    Q_UNUSED(program_);
    Q_UNUSED(values_);
    Q_UNUSED(arraySize_);
    Q_UNUSED(isMatrixType);
}

void OpenGLArgumentValueBase::drawTrianlesArray(QOpenGLFunctions *f_, int offset_, int size_) const
{
    if(nullptr == f_ ||
            size_ <= 0 ||
            offset_ < 0
            ) { return; }
    f_->glDrawArrays(GL_TRIANGLES, (offset_ / 3) * 3, (size_ / 3) * 3);
    qDebug() << "glDrawArrays( GL_TRIANGLES, " << (offset_ / 3) * 3 << (size_ / 3) * 3 << ")";
}

void OpenGLArgumentValueBase::createTextureFromImage(QOpenGLTexture *&texture_, const QImage &image_)
{
    if(nullptr != texture_)
    {
        delete texture_;
        texture_ = nullptr;
    }
    if(image_.isNull()) { return; }
    texture_ = new QOpenGLTexture(image_.mirrored(), QOpenGLTexture::GenerateMipMaps);
    qDebug() << "texture_ = new QOpenGLTexture(image_.mirrored(), QOpenGLTexture::GenerateMipMaps) " << texture_ << image_.size().width() << image_.size().height();
    texture_->setMagnificationFilter(QOpenGLTexture::Filter::LinearMipMapLinear);
    texture_->setWrapMode(QOpenGLTexture::WrapMode::ClampToBorder);
    texture_->setBorderColor(1, 1, 1, 0);
}

void OpenGLArgumentValueBase::bindTexture(QOpenGLFunctions *f_, QOpenGLTexture *texture_, int textureIndex_) const
{
    if(nullptr == f_ || nullptr == texture_) { return; }
    if(textureIndex_ < 0) { return; }
    f_->glActiveTexture(GL_TEXTURE0 + textureIndex_);
    texture_->bind();
    qDebug() << "glActiveTexture ( GL_TEXTURE0 + " << textureIndex_ << ")";
    qDebug() << "texture_->bind() " << texture_;
}


bool IQuizImageDataArtefact::setArtefact(const Artefact *artefact_, int stepIndex_)
{
    if(nullptr == artefact_ || !artefact_->isObjectLoaded()) { return false; }
    // before set data as setData can add arguments, so setup default arguments
    if(!setArguments(artefact_->getArtefactArg())) { return false; }
    if(!setData(loadBinaryFileByUrl(artefact_->filename()))) { return false; }
    m_id = artefact_->id();
    m_stepIndex = stepIndex_;
    m_filename = artefact_->filename();
    return true;
}

int IQuizImageDataArtefact::getStepIndex() const
{
    return m_stepIndex;
}

const ArgumentList &IQuizImageDataArtefact::getArguments() const
{
    return m_arguments;
}

IQuizImageDataArtefact *IQuizImageDataArtefact::create(const Artefact *artefact_, int stepIndex_)
{
    IQuizImageDataArtefact *artefact = nullptr;
    if(nullptr == artefact_ || !artefact_->isObjectLoaded()) { return artefact; }

    ArtefactTypeModel *artefactTypeModel = static_cast<ArtefactTypeModel *>(
                QMLObjectsBase::getInstance().getListModel(g_artefactTypeModel)
                );
    if(nullptr == artefactTypeModel) { return artefact; }
    const ArtefactType *type = artefactTypeModel->findDataItemByAppIdImpl(artefact_->type());
    if(nullptr == type) { return artefact; }
    switch(type->id())
    {
    case to_underlying(ArtefactTypeEn::Vertex):
        artefact = new QuizImageDataVertexArtefact();
        break;
    case to_underlying(ArtefactTypeEn::Fragment):
        artefact = new QuizImageDataFragmentArtefact();
        break;
    case to_underlying(ArtefactTypeEn::Texture1D):
        artefact = new QuizImageDataTexture1DArtefact();
        break;
    case to_underlying(ArtefactTypeEn::Texture2D):
        artefact = new QuizImageDataTexture2DArtefact();
        break;
    case to_underlying(ArtefactTypeEn::Texture3D):
        artefact = new QuizImageDataTexture3DArtefact();
        break;
    case to_underlying(ArtefactTypeEn::DataJson):
        artefact = new QuizImageDataJsonArtefact();
        break;
    case to_underlying(ArtefactTypeEn::DataObj):
        artefact = new QuizImageData3DOBJArtefact();
        break;
    case to_underlying(ArtefactTypeEn::ConvertJson):
        artefact = new QuizImageDataConvertJsonArtefact();
        break;
    case to_underlying(ArtefactTypeEn::ScriptLua):
        artefact = new QuizImageDataScriptLuaArtefact();
        break;
    default:
        break;
    }

    if(!artefact->setArtefact(artefact_, stepIndex_))
    {
        delete artefact;
        artefact = nullptr;
    }

    return artefact;
}

int IQuizImageDataArtefact::getId() const
{
    return m_id;
}

const QString &IQuizImageDataArtefact::filename() const
{
    return m_filename;
}

bool IQuizImageDataArtefact::isVertexShader() const
{
    return false;
}

bool IQuizImageDataArtefact::isFragmentShader() const
{
    return false;
}

bool IQuizImageDataArtefact::isTexture() const
{
    return nullptr != getTexture();
}

QString IQuizImageDataArtefact::getVertexShader() const
{
    return QString();
}

QString IQuizImageDataArtefact::getFragmentShader() const
{
    return QString();
}

const QImage *IQuizImageDataArtefact::getTexture() const
{
    return nullptr;
}

bool IQuizImageDataArtefact::setArguments(const ArtefactArgModel *args_)
{
    if(nullptr == args_ || !args_->isListLoaded()) { return false; }
    for(int i = 0; i < args_->sizeImpl(); ++i)
    {
        const ArtefactArg *artefactArg = args_->dataItemAtImpl(i);
        if(nullptr == artefactArg) { continue; }
        ArgumentBase arg;
        if(!arg.set(artefactArg)) { continue; }
        m_arguments.push_back(arg);
    }
    return true;
}

static bool checkRequeredFields(const Comment &comment_)
{
    return comment_.values().contains(g_argumentName) &&
        comment_.values().contains(g_nameName) &&
        comment_.values().contains(g_typeName) &&
        comment_.values().contains(g_storageName)
        ;
}

bool IQuizImageDataArtefact::setArguments(const QString &shaderCode_)
{
    if(shaderCode_.trimmed().isEmpty()) { return false; }

    ArtefactArgTypeModel *argTypesModel = static_cast<ArtefactArgTypeModel *>(
                QMLObjectsBase::getInstance().getListModel(g_artefactArgTypeModel)
                );
    if(nullptr == argTypesModel) { return false; }
    ArtefactArgStorageModel *argStoragesModel = static_cast<ArtefactArgStorageModel *>(
                QMLObjectsBase::getInstance().getListModel(g_artefactArgStorageModel)
                );
    if(nullptr == argStoragesModel) { return false; }

    QVector<Comment> comments;
    getShaderComments(shaderCode_, comments);

    for(const auto &comment: qAsConst(comments))
    {
        if(!checkRequeredFields(comment)) { continue; }

        const QString argName = comment.values().value(g_nameName).trimmed();
        const QString argTypeStr = comment.values().value(g_typeName).trimmed();
        const QString argStorageStr = comment.values().value(g_storageName).trimmed();
        const QString argDefaultValue = comment.values().value(g_defaultValueName, QString()).trimmed();
        //qDebug() << "argName : " << argName << " argTypeStr : " << argTypeStr << " argDefaultValue : " << argDefaultValue;

        const ArtefactArgType *artefactArgType = argTypesModel->findDataItemByFieldValueImpl(
                    "artefactArgTypeType",
                    QVariant::fromValue(argTypeStr)
                    );
        if(nullptr == artefactArgType) { continue; };

        const ArtefactArgStorage *artefactArgStorage = argStoragesModel->findDataItemByFieldValueImpl(
                    "artefactArgStorageStorage",
                    QVariant::fromValue(argStorageStr)
                    );
        if(nullptr == artefactArgStorage) { continue; }

        ArgumentBase *argument = m_arguments.getByName(argName);
        const bool doesArgumentExist = nullptr != argument;

        ArgumentBase newArgument;
        if(!doesArgumentExist)
        {
            newArgument.setName(argName);
            argument = &newArgument;
        }
        argument->setStorage(argStorageStr);
        argument->setType(argTypeStr);
        argument->setDefaultValue(argDefaultValue);
        argument->setInput(true);   // shaders declare only input arguments
        if(!doesArgumentExist)
        {
            m_arguments.push_back(*argument);
        }
    }

    return true;
}


bool QuizImageDataVertexArtefact::isVertexShader() const
{
    return true;
}

QString QuizImageDataVertexArtefact::getVertexShader() const
{
    return m_vertexShader;
}

bool QuizImageDataVertexArtefact::setData(const QByteArray &data_)
{
    m_vertexShader = getTextFromBinaryData(data_);
    if(m_vertexShader.isEmpty())
    {
        m_vertexShader = ::loadTextFile(":/default.vert");
    }
    setArguments(m_vertexShader);
    return !m_vertexShader.isEmpty();
}


bool QuizImageDataFragmentArtefact::isFragmentShader() const
{
    return true;
}

QString QuizImageDataFragmentArtefact::getFragmentShader() const
{
    return m_fragmentShader;
}

bool QuizImageDataFragmentArtefact::setData(const QByteArray &data_)
{
    m_fragmentShader = getTextFromBinaryData(data_);
    if(m_fragmentShader.isEmpty())
    {
        m_fragmentShader = ::loadTextFile(":/default.frag");
    }
    setArguments(m_fragmentShader);
    return !m_fragmentShader.isEmpty();
}


const QImage *QuizImageDataTexture1DArtefact::getTexture() const
{
    return &m_texture1D;
}

bool QuizImageDataTexture1DArtefact::setData(const QByteArray &data_)
{
    return m_texture1D.loadFromData(data_);
}


const QImage *QuizImageDataTexture2DArtefact::getTexture() const
{
    return &m_texture2D;
}

bool QuizImageDataTexture2DArtefact::setData(const QByteArray &data_)
{
    return m_texture2D.loadFromData(data_);
}


const QImage *QuizImageDataTexture3DArtefact::getTexture() const
{
    return &m_texture3D;
}

bool QuizImageDataTexture3DArtefact::setData(const QByteArray &data_)
{
    return m_texture3D.loadFromData(data_);
}


bool QuizImageDataJsonArtefact::setData(const QByteArray &data_)
{
    m_document = QJsonDocument::fromJson(data_);
    return !m_document.isEmpty();
}


bool QuizImageData3DOBJArtefact::setData(const QByteArray &data_)
{
    m_document = graphicsOBJtoJson(data_);
    return !m_document.isEmpty();
}


bool QuizImageDataScriptLuaArtefact::setData(const QByteArray &data_)
{
    m_script = getTextFromBinaryData(data_);
    return !m_script.isEmpty();
}


QuizImageDataObject::~QuizImageDataObject()
{
    free();
}

QuizImageDataObject *QuizImageDataObject::create(const EffectObjects *effectObject_)
{
    if(nullptr == effectObject_ ||
            nullptr == effectObject_->getObjectInfoModel() ||
            !effectObject_->getObjectInfoModel()->isListLoaded() ||
            nullptr == effectObject_->getObjectInfoModel()->getCurrentDataItem() ||
            nullptr == effectObject_->getObjectArtefacts() ||
            !effectObject_->getObjectArtefacts()->isListLoaded()
            ) { return nullptr; }
    QuizImageDataObject *res = new QuizImageDataObject();
    res->m_programmerName = effectObject_->getObjectInfoModel()->getCurrentDataItem()->programmerName();
    for(int i = 0; i < effectObject_->getObjectArtefacts()->sizeImpl(); i++)
    {
        const ObjectArtefact *objectArtefact = effectObject_->getObjectArtefacts()->dataItemAtImpl(i);
        const ArtefactModel *artefactModel = objectArtefact->getArtefact();
        if(nullptr == artefactModel ||
                !artefactModel->isListLoaded()
                ) { continue; }
        for(int j = 0; j < artefactModel->sizeImpl(); j++)
        {
            const Artefact *artefact = artefactModel->dataItemAtImpl(j);
            IQuizImageDataArtefact *imageDataArtefact = IQuizImageDataArtefact::create(artefact, objectArtefact->stepIndex());
            if(nullptr != imageDataArtefact &&
                    nullptr != res)
            {
                res->m_artefacts.push_back(imageDataArtefact);
            }
        }
    }
    if(nullptr != res)
    {
        res->sortArtefacts();
    }
    return res;
}

const QString &QuizImageDataObject::getProgrammerName() const
{
    return m_programmerName;
}

const QVector<IQuizImageDataArtefact *> &QuizImageDataObject::getArtefacts() const
{
    return m_artefacts;
}

void QuizImageDataObject::sortArtefacts()
{
    std::sort(
        std::begin(m_artefacts),
        std::end(m_artefacts),
        [](const IQuizImageDataArtefact *a1_, const IQuizImageDataArtefact *a2_)->bool
    {
        return nullptr != a1_ &&
                nullptr != a2_ &&
                a1_->getStepIndex() < a2_->getStepIndex()
                ;
    });
}

void QuizImageDataObject::free()
{
    for(auto *&ptr_ : m_artefacts)
    {
        delete ptr_;
        ptr_ = nullptr;
    }
    m_artefacts.clear();
}


bool DrawingArtefact::operator == (const DrawingArtefact &drawingArtefact_) const
{
    return getId() == drawingArtefact_.getId();
}

bool DrawingArtefact::operator < (const DrawingArtefact &drawingArtefact_) const
{
    return getId() < drawingArtefact_.getId();
}

int DrawingArtefact::getId() const
{
    return m_id;
}

void DrawingArtefact::setId(int id_)
{
    m_id = id_;
}


bool DrawingTextureArtefact::operator == (const DrawingTextureArtefact &drawingArtefact_) const
{
    return getFilename() ==  drawingArtefact_.getFilename();
}

bool DrawingTextureArtefact::operator < (const DrawingTextureArtefact &drawingArtefact_) const
{
    return getFilename() <  drawingArtefact_.getFilename();
}

void DrawingTextureArtefact::deepCopy()
{
    m_filename = QString(m_filename.constData(), m_filename.length());
    m_image = m_image.copy();
}

const QString &DrawingTextureArtefact::getFilename() const
{
    return m_filename;
}

const QImage &DrawingTextureArtefact::getImage() const
{
    return m_image;
}

template<class JsonType>
DrawingTextureArtefact *fromJsonT(const JsonType &obj_, const DrawingTextureArtefact *)
{
    if(obj_.isEmpty()) { return nullptr; }
    DrawingTextureArtefact *result = new DrawingTextureArtefact();
    QJsonValue jsvFilename = obj_[g_textureArtefactFilenameName];
    if(!jsvFilename.isUndefined() && jsvFilename.isString())
    {
        result->setFilename(jsvFilename.toString());
    }
    if(!result->loadImage())
    {
        delete result;
        result = nullptr;
    }
    return result;
}

DrawingTextureArtefact *fromJson(const QJsonDocument &obj_)
{
    return fromJsonT(obj_, static_cast<const DrawingTextureArtefact *>(nullptr));
}

DrawingTextureArtefact *DrawingTextureArtefact::fromJson(const QJsonObject &obj_)
{
    return fromJsonT(obj_, static_cast<const DrawingTextureArtefact *>(nullptr));
}

void DrawingTextureArtefact::setFilename(const QString &name_)
{
    m_filename = name_;
}

void DrawingTextureArtefact::setTexture(const QImage &image_)
{
    m_image = image_;
}

bool DrawingTextureArtefact::loadImage()
{
    m_image.load(m_filename);
    return !m_image.isNull();
}


bool DrawingShaderArtefact::operator == (const DrawingShaderArtefact &drawingArtefact_) const
{
    return getFilename() == drawingArtefact_.getFilename();
}

bool DrawingShaderArtefact::operator < (const DrawingShaderArtefact &drawingArtefact_) const
{
    return getFilename() < drawingArtefact_.getFilename();
}

void DrawingShaderArtefact::deepCopy()
{
    m_shaderCode = QString(m_shaderCode.constData(), m_shaderCode.length());
}

const QString &DrawingShaderArtefact::getShaderCode() const
{
    return m_shaderCode;
}

template<class JsonType>
DrawingShaderArtefact *fromJsonT(const JsonType &obj_, const DrawingShaderArtefact *)
{
    if(obj_.isEmpty()) { return nullptr; }
    DrawingShaderArtefact *result = new DrawingShaderArtefact();
    QJsonValue jsvFilename = obj_[g_shaderArtefactFilenameName];
    if(!jsvFilename.isUndefined() && jsvFilename.isString())
    {
        result->setFilename(jsvFilename.toString());
    }
    if(!result->loadFile())
    {
        delete result;
        result = nullptr;
    }
    return result;
}

DrawingShaderArtefact *DrawingShaderArtefact::fromJson(const QJsonDocument &obj_)
{
    return fromJsonT(obj_, static_cast<const DrawingShaderArtefact *>(nullptr));
}

DrawingShaderArtefact *DrawingShaderArtefact::fromJson(const QJsonObject &obj_)
{
    return fromJsonT(obj_, static_cast<const DrawingShaderArtefact *>(nullptr));
}

void DrawingShaderArtefact::toJson(const DrawingShaderArtefact *artefact_, QJsonObject &obj_)
{
    if(nullptr == artefact_) { return; }
    obj_.insert(
                g_shaderArtefactFilenameName,
                QJsonValue::fromVariant(QVariant::fromValue(artefact_->getFilename()))
                );
}

void DrawingShaderArtefact::setShader(const QString &shaderCode_)
{
    m_shaderCode = shaderCode_;
}

void DrawingShaderArtefact::setFilename(const QString &filename_)
{
    m_filename = filename_;
}

const QString &DrawingShaderArtefact::getFilename() const
{
    return m_filename;
}

bool DrawingShaderArtefact::loadFile()
{
    setShader(loadTextFile(getFilename()));
    return !m_shaderCode.trimmed().isEmpty();
}


DrawingArgument::DrawingArgument(
        ArgumentValueDataArray *valueDataArray_ /*= nullptr*/,
        int position_ /*= 0*/)
    : m_valueDataArray(valueDataArray_),
    m_position(position_)
{
}

DrawingArgument::~DrawingArgument()
{
    delete m_valueDataArray;
    m_valueDataArray = nullptr;
}

QString DrawingArgument::getArgumentName() const
{
    return nullptr != m_valueDataArray ? m_valueDataArray->getName() : QString();
}

void DrawingArgument::initData()
{
    if(nullptr == m_valueDataArray) { return; }
    m_valueDataArray->initData();
}

void DrawingArgument::setValues(const QVector<GLfloat> &values_, int size_)
{
    if(nullptr == m_valueDataArray) { return; }
    const int cnt = size_ < 0
            ? std::min(values_.size(), m_valueDataArray->floatValues().size())
            : size_;
    if(size_ >= 0 &&
            m_valueDataArray->floatValues().size() != size_)
    {
        m_valueDataArray->floatValues().resize(size_);
    }
    for(int i = 0; i < cnt; ++i)
    {
        if(i < m_valueDataArray->floatValues().size())
        {
            m_valueDataArray->floatValues()[i] = values_[i];
        }
        else
        {
            break;
        }
    }
}

void DrawingArgument::setValues(const QVector<GLint> &values_, int size_)
{
    if(nullptr == m_valueDataArray) { return; }
    const int cnt = size_ < 0
            ? std::min(values_.size(), m_valueDataArray->intValues().size())
            : size_;
    if(size_ >= 0 &&
            m_valueDataArray->intValues().size() != size_)
    {
        m_valueDataArray->intValues().resize(size_);
    }
    for(int i = 0; i < cnt; ++i)
    {
        if(i < m_valueDataArray->intValues().size())
        {
            m_valueDataArray->intValues()[i] = values_[i];
        }
        else
        {
            break;
        }
    }
}

void DrawingArgument::getValues(QVector<GLfloat> &values_) const
{
    if(nullptr == m_valueDataArray) { return; }
    const int cnti = std::min(values_.size(), m_valueDataArray->intValues().size());
    for(int i = 0; i < cnti; ++i)
    {
        values_[i] = m_valueDataArray->intValues()[i];
    }
    const int cntf = std::min(values_.size(), m_valueDataArray->floatValues().size());
    for(int i = 0; i < cntf; ++i)
    {
        values_[i] = m_valueDataArray->floatValues()[i];
    }
}

void DrawingArgument::getValues(QVector<GLint> &values_) const
{
    if(nullptr == m_valueDataArray) { return; }
    const int cntf = std::min(values_.size(), m_valueDataArray->floatValues().size());
    for(int i = 0; i < cntf; ++i)
    {
        values_[i] = m_valueDataArray->floatValues()[i];
    }
    const int cnti = std::min(values_.size(), m_valueDataArray->intValues().size());
    for(int i = 0; i < cnti; ++i)
    {
        values_[i] = m_valueDataArray->intValues()[i];
    }
}

void DrawingArgument::setValue(const QString &value_)
{
    if(nullptr == m_valueDataArray) { return; }
    if(m_valueDataArray->stringValues().size() < 1) { return; }
    m_valueDataArray->stringValues()[0] = value_;
}

int DrawingArgument::getTupleSize() const
{
    if(nullptr == m_valueDataArray) { return 0; }
    return m_valueDataArray->getTupleSize();
}

bool DrawingArgument::isInitialized() const
{
    if(nullptr == m_valueDataArray) { return false; }
    return m_valueDataArray->isInitialized() &&
            !m_valueDataArray->isDefaultValue() &&
            (
                !m_valueDataArray->intValues().isEmpty() ||
                !m_valueDataArray->floatValues().isEmpty() ||
                !m_valueDataArray->stringValues().isEmpty()
                )
            ;
}

DrawingArgument *DrawingArgument::copy() const
{
    return new DrawingArgument(
                nullptr != m_valueDataArray ? m_valueDataArray->copy() : nullptr,
                m_position
                );
}

const QVector<GLint> &DrawingArgument::intValues() const
{
    static QVector<GLint> fish;
    return nullptr != m_valueDataArray ? m_valueDataArray->intValues() : fish;
}

const QVector<GLfloat> &DrawingArgument::floatValues() const
{
    static QVector<GLfloat> fish;
    return nullptr != m_valueDataArray ? m_valueDataArray->floatValues() : fish;
}

const QVector<QString> &DrawingArgument::stringValues() const
{
    static QVector<QString> fish;
    return nullptr != m_valueDataArray ? m_valueDataArray->stringValues() : fish;
}

bool DrawingArgument::operator == (const DrawingArgument &argument_) const
{
    return getArgumentName() == argument_.getArgumentName() &&
            m_position == argument_.m_position
            ;
}

bool DrawingArgument::operator < (const DrawingArgument &argument_) const
{
    if(getArgumentName() < argument_.getArgumentName()) { return false; }
    if(getArgumentName() > argument_.getArgumentName()) { return true; }
    return m_position < argument_.m_position;
}

bool DrawingArgument::doesValueEqual(const DrawingArgument &argument_) const
{
    return intValues() == argument_.intValues() &&
            floatValues() == argument_.floatValues() &&
            stringValues() == argument_.stringValues()
            ;
}

OpenGLArgumentValueBase *DrawingArgument::createOpenglValue()
{
    if(nullptr == m_valueDataArray) { return nullptr; }
    return m_valueDataArray->createOpenGlValue();
}

const ArgumentValueDataArray *DrawingArgument::getValueDataArray() const
{
    return m_valueDataArray;
}


OpenGLDrawingStepImageData::~OpenGLDrawingStepImageData()
{
    free();
}

bool OpenGLDrawingStepImageData::buildProgram(QString &errorLog_)
{
    if(nullptr == m_vertexShader &&
            nullptr != m_vertexArtefact)
    {
        m_vertexShader = new QOpenGLShader(QOpenGLShader::Vertex, nullptr);
        m_vertexDataBA = m_vertexArtefact->getShaderCode().toUtf8();
        m_vertexShader->compileSourceCode(m_vertexDataBA.constData());
        if(!m_vertexShader->isCompiled())
        {
            errorLog_ += m_vertexShader->log();
        }
    }
    if(nullptr == m_fragmentShader &&
            nullptr != m_fragmentArtefact)
    {
        m_fragmentShader = new QOpenGLShader(QOpenGLShader::Fragment, nullptr);
        m_fragmentDataBA = m_fragmentArtefact->getShaderCode().toUtf8();
        m_fragmentShader->compileSourceCode(m_fragmentDataBA.constData());
        if(!m_fragmentShader->isCompiled())
        {
            errorLog_ += m_fragmentShader->log();
        }
    }
    m_program = new QOpenGLShaderProgram();
    qDebug() << "m_program = new QOpenGLShaderProgram()";
    m_program->addShader(m_vertexShader);
    qDebug() << "m_program->addShader(m_vertexShader)";
    m_program->addShader(m_fragmentShader);
    qDebug() << "m_program->addShader(m_fragmentShader)";
    m_program->link();
    qDebug() << "m_program->link()";
    if(!m_program->isLinked())
    {
        errorLog_ += m_program->log();
        delete m_program;
        m_program = nullptr;
    }
    return isProgramBuilded();
}

bool OpenGLDrawingStepImageData::isProgramBuilded() const
{
    return nullptr != m_program;
}

void OpenGLDrawingStepImageData::createArguments()
{
    if(nullptr == m_program) { return; }
    for(auto *argument_: m_programArguments)
    {
        if(nullptr == argument_) { continue; }
        argument_->create(m_program);
    }
}

void OpenGLDrawingStepImageData::createTextures()
{
    int texturesCount = 0;
    for(auto *argument_: m_programArguments)
    {
        if(nullptr == argument_ ||
                !argument_->isTexture()) { continue; }
        ++texturesCount;
    }
    int textureIndex = texturesCount - 1;
    for(auto *argument_: m_programArguments)
    {
        if(nullptr == argument_ ||
                !argument_->isTexture()) { continue; }
        argument_->setTextureIndex(textureIndex);
        --textureIndex;
    }
    for(int i = 0; i < m_programArguments.size(); i++)
    {
        auto *argument = m_programArguments[i];
        if(nullptr == argument ||
                !argument->isTexture()) { continue; }
        if(i >= m_argumentTextures.size() ||
                nullptr == m_argumentTextures[i]) { continue; }
        argument->createTexture(m_argumentTextures[i]->getImage());
    }
}

void OpenGLDrawingStepImageData::bind()
{
    if(nullptr == m_program) { return; }
    for(auto *argument_ : m_programArguments)
    {
        argument_->bind(m_program);
    }
}

void OpenGLDrawingStepImageData::buildVBO()
{
    if(nullptr != m_vbo) { return; }
    int maxIndex = 0;
    for(auto *argument_: m_programArguments)
    {
        if(nullptr == argument_) { continue; }
        maxIndex = std::max(maxIndex, argument_->getMaxIndex());
    }
    for(auto *argument_: m_programArguments)
    {
        if(nullptr == argument_) { continue; }
        argument_->setMaxIndex(maxIndex);
    }
    int vboDataSize = 0;
    for(auto *argument_: m_programArguments)
    {
        if(nullptr == argument_) { continue; }
        if(0 < argument_->getVBOPartSize() &&
                0 < argument_->getMaxIndex())
        {
            const int offset = vboDataSize;
            vboDataSize += (argument_->getVBOPartSize() * maxIndex) / argument_->getMaxIndex();
            argument_->setVBOPartOffset(offset * sizeof(GLfloat));
        }
    }
    m_vboData.resize(vboDataSize);
    m_vbo = new QOpenGLBuffer();
    qDebug() << "m_vbo = new QOpenGLBuffer()";
    m_vbo->create();
    qDebug() << "m_vbo->create()";
    m_vbo->bind();
    qDebug() << "m_vbo->bind()";
    m_vbo->allocate(m_vboData.count() * sizeof(GLfloat));
    qDebug() << "m_vbo->allocate(" << m_vboData.count() * sizeof(GLfloat) << ")";
    writeVBO();
    m_vbo->release();
    qDebug() << "m_vbo->release()";
}

void OpenGLDrawingStepImageData::writeVBO()
{
    if(nullptr == m_vbo) { return; }
    int maxIndex = 0;
    for(auto *argument_: m_programArguments)
    {
        if(nullptr == argument_) { continue; }
        maxIndex = std::max(maxIndex, argument_->getMaxIndex());
    }
    int vboPartOffset = 0;
    for(auto *argument_: m_programArguments)
    {
        if(nullptr == argument_) { continue; }
        if(0 >= argument_->getVBOPartSize() ||
                0 >= argument_->getMaxIndex()
                ) { continue; }
        const int size = (argument_->getVBOPartSize() * maxIndex) / argument_->getMaxIndex();
        argument_->writeVBOPart(m_vbo, vboPartOffset, argument_->getMaxIndex());
        vboPartOffset += size * sizeof(GLfloat);
    }
}

void OpenGLDrawingStepImageData::bindProgramAndVBO()
{
    if(nullptr == m_program ||
            nullptr == m_vbo) { return; }
    m_program->bind();
    m_vbo->bind();
    qDebug() << "m_program->bind()";
    qDebug() << "m_vbo->bind()";
}

void OpenGLDrawingStepImageData::useArguments() const
{
    for(const auto *argument_: m_programArguments)
    {
        if(nullptr == argument_) { continue; }
        argument_->use(m_program);
    }
    for(const auto *argument_: m_programArguments)
    {
        if(nullptr == argument_) { continue; }
        argument_->use2(m_program);
    }
}

void OpenGLDrawingStepImageData::bindTextures(QOpenGLFunctions *f_) const
{
    for(const auto *argument_: m_programArguments)
    {
        if(nullptr == argument_) { continue; }
        argument_->bindTexture(f_);
    }
}

void OpenGLDrawingStepImageData::draw(QOpenGLFunctions *f_) const
{
    for(const auto *argument_: m_programArguments)
    {
        if(nullptr == argument_) { continue; }
        argument_->draw(f_);
    }
}

void OpenGLDrawingStepImageData::release() const
{
    if(nullptr == m_program) { return; }
    for(const auto *argument_: m_programArguments)
    {
        if(nullptr == argument_) { continue; }
        argument_->release(m_program);
    }
    if(nullptr != m_vbo)
    {
        m_vbo->release();
        qDebug() << "m_vbo->release()";
    }
    m_program->release();
    qDebug() << "m_program->release()";
}

void OpenGLDrawingStepImageData::free()
{
    m_vboData.clear();
    delete m_vbo;
    m_vbo = nullptr;
    delete m_program;
    m_program = nullptr;
    delete m_vertexShader;
    m_vertexShader = nullptr;
    delete m_fragmentShader;
    m_fragmentShader = nullptr;
}


OpenGLDrawingImageData::~OpenGLDrawingImageData()
{
    free();
}

bool OpenGLDrawingImageData::isInitialized() const
{
    return m_initialized;
}

void OpenGLDrawingImageData::findArgumentsRange(
        const QString &argumentName_,
        QVector<DrawingArgument *>::iterator &itb_
        )
{
    itb_ = std::lower_bound(
                std::begin(m_arguments),
                std::end(m_arguments),
                argumentName_,
                [](const DrawingArgument *arg_, const QString &name_)->bool
    {
        return nullptr != arg_ && arg_->getArgumentName() < name_;
    });
}

void OpenGLDrawingImageData::findArgumentsRange(
        const QString &argumentName_,
        QVector<DrawingArgument *>::const_iterator &itb_
        ) const
{
    itb_ = std::lower_bound(
                std::begin(m_arguments),
                std::end(m_arguments),
                argumentName_,
                [](const DrawingArgument *arg_, const QString &name_)->bool
    {
        return nullptr != arg_ && arg_->getArgumentName() < name_;
    });
}

void OpenGLDrawingImageData::setRenderArgumentValue(
        const QString &argumentName_,
        const QVector<GLfloat> & values_,
        int size_
        )
{
    auto itb = std::end(m_arguments);
    findArgumentsRange(argumentName_, itb);
    for(auto it = itb; it != std::end(m_arguments); ++it)
    {
        if(nullptr == *it) { continue; }
        if((*it)->getArgumentName() != argumentName_) { break; }
        (*it)->setValues(values_, size_);
    }
}

void OpenGLDrawingImageData::setRenderArgumentValue(
        const QString &argumentName_,
        const QVector<GLint> & values_,
        int size_
        )
{
    auto itb = std::end(m_arguments);
    findArgumentsRange(argumentName_, itb);
    for(auto it = itb; it != std::end(m_arguments); ++it)
    {
        if(nullptr == *it) { continue; }
        if((*it)->getArgumentName() != argumentName_) { break; }
        (*it)->setValues(values_, size_);
    }
}

void OpenGLDrawingImageData::getArgumentValue(
        const QString &argumentName_,
        QVector<GLfloat> & values_
        ) const
{
    auto itb = std::end(m_arguments);
    findArgumentsRange(argumentName_, itb);
    for(auto it = itb; it != std::end(m_arguments); ++it)
    {
        if(nullptr == *it) { continue; }
        if((*it)->getArgumentName() != argumentName_) { break; }
        (*it)->getValues(values_);
        break;
    }
}

void OpenGLDrawingImageData::getArgumentValue(
        const QString &argumentName_,
        QVector<GLint> & values_
        ) const
{
    auto itb = std::end(m_arguments);
    findArgumentsRange(argumentName_, itb);
    for(auto it = itb; it != std::end(m_arguments); ++it)
    {
        if(nullptr == *it) { continue; }
        if((*it)->getArgumentName() != argumentName_) { break; }
        (*it)->getValues(values_);
        break;
    }
}

int OpenGLDrawingImageData::getTupleSize(const QString &argumentName_) const
{
    auto itb = std::end(m_arguments);
    findArgumentsRange(argumentName_, itb);
    for(auto it = itb; it != std::end(m_arguments); ++it)
    {
        if(nullptr == *it) { continue; }
        if((*it)->getArgumentName() != argumentName_) { break; }
        return (*it)->getTupleSize();
    }
    return 0;
}

bool OpenGLDrawingImageData::isArgumentInitialized(const QString &argumentName_) const
{
    auto itb = std::end(m_arguments);
    findArgumentsRange(argumentName_, itb);
    for(auto it = itb; it != std::end(m_arguments); ++it)
    {
        if(nullptr == *it) { continue; }
        if((*it)->getArgumentName() != argumentName_) { break; }
        return (*it)->isInitialized();
    }
    return false;
}

bool OpenGLDrawingImageData::getTextureSize(const QString &argumentName_, QSize &size_) const
{
    auto itb = std::end(m_arguments);
    findArgumentsRange(argumentName_, itb);
    for(auto it = itb; it != std::end(m_arguments); ++it)
    {
        if(nullptr == *it) { continue; }
        if((*it)->getArgumentName() != argumentName_) { break; }
        for(auto *step_ : m_steps)
        {
            const int cnt = std::min(step_->m_programArguments.size(), step_->m_argumentTextures.size());
            for(int i1 = 0; i1 < cnt; ++i1)
            {
                if(!step_->m_programArguments[i1]->valueOf((*it)->getValueDataArray())) { continue; }
                if(nullptr != step_->m_argumentTextures[i1])
                {
                    size_ = step_->m_argumentTextures[i1]->getImage().size();
                    return true;
                }
            }
        }
    }
    return false;
}

void OpenGLDrawingImageData::addRenderImage(const QString &filename_, bool fromImage_)
{
    std::unique_ptr<DrawingTextureArtefact> fromTexture(new DrawingTextureArtefact());
    fromTexture->setFilename(filename_);
    if(!fromTexture->loadImage()) { return; }
    auto fTexIt = std::find_if(
                std::begin(m_textures),
                std::end(m_textures),
                [&fromTexture](const DrawingTextureArtefact *t_)->bool
    {
        return nullptr != t_ &&
                nullptr != fromTexture.get() &&
                *(fromTexture.get()) == *t_
                ;
    });
    if(std::end(m_textures) == fTexIt)
    {
        m_textures.push_back(fromTexture.release());
        fTexIt = std::end(m_textures);
        --fTexIt;
    }

    const QString argumentName = fromImage_ ? g_renderFromImageName : g_renderToImageName;
    auto itb = std::end(m_arguments);
    findArgumentsRange(argumentName, itb);

    for(auto it = itb; it != std::end(m_arguments); ++it)
    {
        if(nullptr == *it) { continue; }
        if((*it)->getArgumentName() != argumentName) { break; }
        (*it)->setValue(filename_);
        for(auto *step_ : m_steps)
        {
            if(nullptr == step_) { continue; }
            const int cnt = std::min(step_->m_programArguments.size(), step_->m_argumentTextures.size());
            for(int i1 = 0; i1 < cnt; ++i1)
            {
                if(!step_->m_programArguments[i1]->valueOf((*it)->getValueDataArray())) { continue; }
                step_->m_argumentTextures[i1] = *fTexIt;
            }
        }
    }
}

int OpenGLDrawingImageData::stepCount() const
{
    return m_steps.size();
}

bool OpenGLDrawingImageData::buildStepProgram(int stepIndex_, QString &errorLog_)
{
    if(stepIndex_ < 0 ||
            stepIndex_ >= stepCount() ||
            nullptr == m_steps[stepIndex_]
            ) { return false; }
    const bool res = m_steps[stepIndex_]->buildProgram(errorLog_);
    m_initialized |= res;
    return res;
}

bool OpenGLDrawingImageData::isStepProgramBuilded(int stepIndex_) const
{
    if(stepIndex_ < 0 ||
            stepIndex_ >= stepCount() ||
            nullptr == m_steps[stepIndex_]
            ) { return false; }
    return m_steps[stepIndex_]->isProgramBuilded();
}

void OpenGLDrawingImageData::createStepArgument(int stepIndex_)
{
    if(!isStepProgramBuilded(stepIndex_) ||
            stepIndex_ < 0 ||
            stepIndex_ >= stepCount() ||
            nullptr == m_steps[stepIndex_]
            ) { return; }
    m_steps[stepIndex_]->createArguments();
}

void OpenGLDrawingImageData::createStepTextures(int stepIndex_)
{
    if(!isStepProgramBuilded(stepIndex_) ||
            stepIndex_ < 0 ||
            stepIndex_ >= stepCount() ||
            nullptr == m_steps[stepIndex_]
            ) { return; }
    m_steps[stepIndex_]->createTextures();
}

void OpenGLDrawingImageData::bindStep(int stepIndex_)
{
    if(!isStepProgramBuilded(stepIndex_) ||
            stepIndex_ < 0 ||
            stepIndex_ >= stepCount() ||
            nullptr == m_steps[stepIndex_]
            ) { return; }
    m_steps[stepIndex_]->bind();
}

void OpenGLDrawingImageData::buildStepVBO(int stepIndex_)
{
    if(!isStepProgramBuilded(stepIndex_) ||
            stepIndex_ < 0 ||
            stepIndex_ >= stepCount() ||
            nullptr == m_steps[stepIndex_]
            ) { return; }
    m_steps[stepIndex_]->buildVBO();
}

void OpenGLDrawingImageData::bindStepProgramAndVBO(int stepIndex_)
{
    if(!isStepProgramBuilded(stepIndex_) ||
            stepIndex_ < 0 ||
            stepIndex_ >= stepCount() ||
            nullptr == m_steps[stepIndex_]
            ) { return; }
    m_steps[stepIndex_]->bindProgramAndVBO();
}

void OpenGLDrawingImageData::writeStepVBO(int stepIndex_) const
{
    if(!isStepProgramBuilded(stepIndex_) ||
            stepIndex_ < 0 ||
            stepIndex_ >= stepCount() ||
            nullptr == m_steps[stepIndex_]
            ) { return; }
    m_steps[stepIndex_]->writeVBO();
}

void OpenGLDrawingImageData::useStepArguments(int stepIndex_) const
{
    if(!isStepProgramBuilded(stepIndex_) ||
            stepIndex_ < 0 ||
            stepIndex_ >= stepCount() ||
            nullptr == m_steps[stepIndex_]
            ) { return; }
    m_steps[stepIndex_]->useArguments();
}

void OpenGLDrawingImageData::bindStepTexture(int stepIndex_, QOpenGLFunctions *f_) const
{
    if(!isStepProgramBuilded(stepIndex_) ||
            stepIndex_ < 0 ||
            stepIndex_ >= stepCount() ||
            nullptr == m_steps[stepIndex_]
            ) { return; }
    m_steps[stepIndex_]->bindTextures(f_);
}

void OpenGLDrawingImageData::drawStep(int stepIndex_, QOpenGLFunctions *f_) const
{
    if(!isStepProgramBuilded(stepIndex_) ||
            stepIndex_ < 0 ||
            stepIndex_ >= stepCount() ||
            nullptr == m_steps[stepIndex_]
            ) { return; }
    m_steps[stepIndex_]->draw(f_);
}

void OpenGLDrawingImageData::releaseStep(int stepIndex_) const
{
    if(!isStepProgramBuilded(stepIndex_) ||
            stepIndex_ < 0 ||
            stepIndex_ >= stepCount() ||
            nullptr == m_steps[stepIndex_]
            ) { return; }
    m_steps[stepIndex_]->release();
}

OpenGLDrawingImageData *OpenGLDrawingImageData::fromJson(const QJsonDocument &doc_)
{
    if(doc_.isEmpty()) { return nullptr; }
    OpenGLDrawingImageData *result = new OpenGLDrawingImageData();
    const QJsonValue jsvSteps = doc_[g_stepsName];
    if(jsvSteps.isUndefined() || !jsvSteps.isArray())
    {
        delete result;
        result = nullptr;
        return result;
    }
    const QJsonArray jsaSteps = jsvSteps.toArray();
    for(int i1 = 0; i1 < jsaSteps.size() ; ++i1)
    {
        const DrawingShaderArtefact *vertexShader = nullptr;
        const DrawingShaderArtefact *fragmentShader = nullptr;
        do {
            QJsonValue jsvStep = jsaSteps[i1];
            if(jsvStep.isUndefined() || !jsvStep.isObject()) { break; }
            const QJsonObject jsoStep = jsvStep.toObject();
            QJsonValue jsvStepVertex = jsoStep[g_vertexShaderArtefactForStepName];
            QJsonValue jsvStepFragment = jsoStep[g_fragmentShaderArtefactForStepName];
            if(jsvStepVertex.isUndefined() || !jsvStepVertex.isObject()) { break; }
            if(jsvStepFragment.isUndefined() || !jsvStepFragment.isObject()) { break; }
            vertexShader = DrawingShaderArtefact::fromJson(jsvStepVertex.toObject());
            fragmentShader = DrawingShaderArtefact::fromJson(jsvStepFragment.toObject());
            if(nullptr == vertexShader || fragmentShader == nullptr) { break; }
            QJsonValue jsvStepArguments = jsoStep[g_argumentsArtefactForStepName];
            if(jsvStepArguments.isUndefined() || !jsvStepArguments.isArray()) { break; }
            using ArgumentAndTexture = QPair<const ArgumentBase *, const DrawingTextureArtefact *>;
            QVector<ArgumentAndTexture> arguments;
            const QJsonArray jsaStepArguments = jsvStepArguments.toArray();
            for(int i2 = 0; i2 < jsaStepArguments.size(); ++i2)
            {
                const ArgumentBase *argument = nullptr;
                const DrawingTextureArtefact *texture = nullptr;
                do {
                    QJsonValue jsvStepArgument = jsaStepArguments[i2];
                    if(jsvStepArgument.isUndefined() || !jsvStepArgument.isObject()) { break; }
                    const QJsonObject jsoStepArgument = jsvStepArgument.toObject();
                    QJsonValue jsvArgument = jsoStepArgument[g_argumentArtefactForStepName];
                    QJsonValue jsvTexture = jsoStepArgument[g_textureArtefactForStepName];
                    if(jsvArgument.isUndefined() || !jsvArgument.isObject()) { break; }
                    argument = ArgumentBase::fromJson(jsvArgument.toObject());
                    texture = DrawingTextureArtefact::fromJson(jsvTexture.toObject());
                    if(nullptr == argument) { break; }
                    arguments.push_back({argument, texture});
                    argument = nullptr;
                    texture = nullptr;
                } while(false);
                delete argument;
                argument = nullptr;
                delete texture;
                texture = nullptr;
            }
            result->m_steps.push_back(new OpenGLDrawingStepImageData());
            for(auto &arg_ :arguments)
            {
                if(nullptr == arg_.first) { continue; }
                ArgumentValueDataArray *valueDataArray = arg_.first->createValueDataArray();
                DrawingArgument *argument = new DrawingArgument(valueDataArray, result->m_steps.size());
                result->m_arguments.push_back(argument);
                result->m_steps.back()->m_programArguments.push_back(
                            argument->createOpenglValue()
                            );
                if(nullptr != arg_.second)
                {
                    result->m_textures.push_back(arg_.second);
                    result->m_steps.back()->m_argumentTextures.push_back(arg_.second);
                }
                else
                {
                    result->m_steps.back()->m_argumentTextures.push_back(nullptr);
                }
                arg_.second = nullptr;
            }
            result->m_vertexShaders.push_back(vertexShader);
            result->m_fragmentShaders.push_back(fragmentShader);
            result->m_steps.back()->m_vertexArtefact = vertexShader;
            result->m_steps.back()->m_fragmentArtefact = fragmentShader;
            vertexShader = nullptr;
            fragmentShader = nullptr;
            for(auto &arg_ :arguments)
            {
                delete arg_.first;
                arg_.first = nullptr;
                delete arg_.second;
                arg_.second = nullptr;
            }
        } while(false);
        delete vertexShader;
        vertexShader = nullptr;
        delete fragmentShader;
        fragmentShader = nullptr;
    }
    if(nullptr != result)
    {
        std::sort(
            std::begin(result->m_arguments),
            std::end(result->m_arguments),
            [](const DrawingArgument *a1_, const DrawingArgument *a2_)->bool
        {
            return nullptr != a1_ &&
                    nullptr != a2_ &&
                    a1_->getArgumentName() < a2_->getArgumentName()
                    ;
        });
        std::sort(
            std::begin(result->m_textures),
            std::end(result->m_textures),
            [](const DrawingTextureArtefact *t1_, const DrawingTextureArtefact *t2_)->bool
        {
            return nullptr != t1_ &&
                    nullptr != t2_ &&
                    *t1_ < *t2_
                    ;
        });
        std::sort(
            std::begin(result->m_vertexShaders),
            std::end(result->m_vertexShaders),
            [](const DrawingShaderArtefact *s1_, const DrawingShaderArtefact *s2_)->bool
        {
            return nullptr != s1_ &&
                    nullptr != s2_ &&
                    *s1_ < *s2_
                    ;
        });
        std::sort(
            std::begin(result->m_fragmentShaders),
            std::end(result->m_fragmentShaders),
            [](const DrawingShaderArtefact *s1_, const DrawingShaderArtefact *s2_)->bool
        {
            return nullptr != s1_ &&
                    nullptr != s2_ &&
                    *s1_ < *s2_
                    ;
        });
        for(auto *a_ : result->m_arguments)
        {
            if(nullptr == a_) { continue; }
            a_->initData();
        }
    }

    return result;
}

void OpenGLDrawingImageData::free()
{
    for(auto *&ptr_ : m_steps)
    {
        delete ptr_;
        ptr_ = nullptr;
    }
    m_steps.clear();
    for(auto *&ptr_ : m_arguments)
    {
        delete ptr_;
        ptr_ = nullptr;
    }
    m_arguments.clear();
    for(auto *&ptr_ : m_textures)
    {
        delete ptr_;
        ptr_ = nullptr;
    }
    m_textures.clear();
    for(auto *&ptr_ : m_vertexShaders)
    {
        delete ptr_;
        ptr_ = nullptr;
    }
    m_vertexShaders.clear();
    for(auto *&ptr_ : m_fragmentShaders)
    {
        delete ptr_;
        ptr_ = nullptr;
    }
    m_fragmentShaders.clear();
}


OpenGLDrawingImageData *DrawingImageData::copy()
{
    OpenGLDrawingImageData *result = new OpenGLDrawingImageData();

/*    result->m_textures.reserve(m_texturesSet.size());
    for(const DrawingTextureArtefact &artefact_ : m_texturesSet)
    {
        result->m_textures.push_back(artefact_);
    }
    result->m_vertexShaders.reserve(m_vertexShadersSet.size());
    for(const DrawingShaderArtefact &artefact_ : m_vertexShadersSet)
    {
        result->m_vertexShaders.push_back(artefact_);
    }
    result->m_fragmentShaders.reserve(m_fragmentShadersSet.size());
    for(const DrawingShaderArtefact &artefact_ : m_fragmentShadersSet)
    {
        result->m_fragmentShaders.push_back(artefact_);
    }
    result->m_arguments.reserve(m_argumentsSet.size());
    for(const DrawingArgument &argument_ : m_argumentsSet)
    {
        result->m_arguments.push_back(argument_.copy());
    }
    for(DrawingTextureArtefact &artefact_ : result->m_textures)
    {
        artefact_.deepCopy();
    }
    for(DrawingShaderArtefact &artefact_ : result->m_vertexShaders)
    {
        artefact_.deepCopy();
    }
    for(DrawingShaderArtefact &artefact_ : result->m_fragmentShaders)
    {
        artefact_.deepCopy();
    }*/
    return result;
}

void DrawingImageData::setAllArgumentValues(ArgumentValueDataArray *argumentValueDataArray_)
{
    DrawingArgument arg(argumentValueDataArray_);
    auto itb = m_argumentsSet.lower_bound(arg);
    auto ite = m_argumentsSet.upper_bound(arg);
    for(auto it = itb;
        it != ite;
        ++it)
    {
        m_argumentsSet.erase(it);
    }
    m_argumentsSet.insert(arg);
}

void DrawingImageData::setObjects(const QVector<QuizImageDataObject *> &objects_)
{
    setArtefacts(objects_);
}

void DrawingImageData::setArtefacts(const QVector<QuizImageDataObject *> &objects_)
{
    std::set<DrawingTextureArtefact> textures;
    std::set<DrawingShaderArtefact> vertexShaders;
    std::set<DrawingShaderArtefact> fragmentShaders;
    for(const auto *obj_ : objects_)
    {
        if(nullptr == obj_) { continue; }
        for(const auto *artefact_: obj_->getArtefacts())
        {
            if(nullptr == artefact_) { continue; }

            if(artefact_->isVertexShader())
            {
                DrawingShaderArtefact drawingArtefact;
                drawingArtefact.setId(artefact_->getId());
                drawingArtefact.setFilename(artefact_->filename());
                drawingArtefact.setShader(artefact_->getVertexShader());
                vertexShaders.insert(drawingArtefact);
            }
            else if(artefact_->isFragmentShader())
            {
                DrawingShaderArtefact drawingArtefact;
                drawingArtefact.setId(artefact_->getId());
                drawingArtefact.setFilename(artefact_->filename());
                drawingArtefact.setShader(artefact_->getFragmentShader());
                fragmentShaders.insert(drawingArtefact);
            }
            else if(artefact_->isTexture() && nullptr != artefact_->getTexture())
            {
                DrawingTextureArtefact drawingArtefact;
                drawingArtefact.setId(artefact_->getId());
                drawingArtefact.setTexture(*artefact_->getTexture());
                drawingArtefact.setFilename(artefact_->filename());
                textures.insert(drawingArtefact);
            }
        }
    }
}


QuizImageData::QuizImageData()
{
    m_newFromImageUrl = setDefaultImageIfEmpty(QString());
    m_newToImageUrl = setDefaultImageIfEmpty(QString());
    clearEffect();
}

QuizImageData::~QuizImageData()
{
    free();
}

void QuizImageData::setFromImageUrl(const QString &fromImageUrl_)
{
    m_newFromImageUrl = setDefaultImageIfEmpty(fromImageUrl_);
}

void QuizImageData::setToImageUrl(const QString &toImageUrl_)
{
    m_newToImageUrl = setDefaultImageIfEmpty(toImageUrl_);
}

bool QuizImageData::fromImageUrlChanged() const
{
    return m_newFromImageUrl != m_fromImageUrl;
}

void QuizImageData::useNewFromImageUrl()
{
    m_fromImageUrl = m_newFromImageUrl;
}

bool QuizImageData::toImageUrlChanged() const
{
    return m_newToImageUrl != m_toImageUrl;
}

void QuizImageData::useNewToImageUrl()
{
    m_toImageUrl = m_newToImageUrl;
}

bool QuizImageData::isSwapImages() const
{
    return m_newFromImageUrl != m_fromImageUrl &&
            m_newToImageUrl != m_toImageUrl &&
            m_newFromImageUrl == m_toImageUrl &&
            m_newToImageUrl == m_fromImageUrl
            ;
}

void QuizImageData::swapImages()
{
    if(!isSwapImages()) { return; }
    useNewFromImageUrl();
    useNewToImageUrl();
}

const QString &QuizImageData::getFromImageUrl() const
{
    return m_fromImageUrl;
}

const QString &QuizImageData::getToImageUrl() const
{
    return m_toImageUrl;
}

bool QuizImageData::isEffectChanged() const
{
    return m_effectIsChanged;
}

void QuizImageData::useNewEffect()
{
    m_effectIsChanged = false;
}

void QuizImageData::setEffect(const Effect *effect_)
{
    if(!canUpdateEffect(effect_))
    {
        if(differentEffect(effect_))
        {
            clearEffect();
        }
        return;
    }

    clearEffect();

    const EffectObjectsModel *effectObjectModel = effect_->getEffectObjects();
    Q_ASSERT(nullptr != effectObjectModel);
    Q_ASSERT(effectObjectModel->isListLoaded());

    for(int i = 0; i < effectObjectModel->sizeImpl(); i++)
    {
        const EffectObjects *effectObjects = effectObjectModel->dataItemAtImpl(i);
        if(nullptr == effectObjects) { continue; }
        QuizImageDataObject *dataObj = QuizImageDataObject::create(effectObjects);
        if(nullptr == dataObj) { continue; }
        m_objects.push_back(dataObj);
    }

    m_effectIsChanged = true;
}

bool QuizImageData::canUpdateEffect(const Effect *effect_) const
{
    if(nullptr == effect_ ||
            effect_->id() == m_oldEffectId ||
            !effect_->isChildrenLoaded() ||
            nullptr == effect_->getEffectObjects() ||
            !effect_->getEffectObjects()->isListLoaded()
            ) { return false; }
    return true;
}

bool QuizImageData::differentEffect(const Effect *effect_) const
{
    if(nullptr == effect_ ||
            effect_->id() != m_oldEffectId
            ) { return true; }
    return false;
}

void QuizImageData::setArgumentSet(const EffectArgSet *argumentSet_)
{
}

void QuizImageData::clearEffect()
{
    // TODO: add implementation
    freeObjects();
    m_oldEffectId = -1;
    m_effectIsChanged = true;
}

void QuizImageData::free()
{
    freeObjects();
}

void QuizImageData::freeObjects()
{
    for(auto *&ptr_ : m_objects)
    {
        delete ptr_;
        ptr_ = nullptr;
    }
    m_objects.clear();
}

void QuizImageData::prepareDrawingData()
{
    setImagesToDrawingData();
}

void QuizImageData::setImagesToDrawingData()
{
    // set from image
    DrawingTextureArtefact fromTexture;
    fromTexture.setFilename(getFromImageUrl());
    fromTexture.setTexture(QImage(getFromImageUrl()));
    //m_drawingData.addTexture(fromTexture);

    ArgumentBase fromArg;
    fromArg.setName(g_renderFromImageName);
    fromArg.setType(g_sampler2DTypeName);
    fromArg.setStorage(g_uniformStorageName);
    fromArg.setInput(true);
    fromArg.setValue(getFromImageUrl());
    ArgumentValueDataArray *valueDataArray = fromArg.createValueDataArray();
    if(nullptr != valueDataArray)
    {
        valueDataArray->initData();
        m_drawingData.setAllArgumentValues(valueDataArray);
    }
    valueDataArray = nullptr;
    useNewFromImageUrl();

    // set to image
    DrawingTextureArtefact toTexture;
    toTexture.setFilename(getToImageUrl());
    toTexture.setTexture(QImage(getToImageUrl()));
    //m_drawingData.addTexture(toTexture);

    ArgumentBase toArg;
    toArg.setName(g_renderToImageName);
    toArg.setType(g_sampler2DTypeName);
    toArg.setStorage(g_uniformStorageName);
    toArg.setInput(true);
    toArg.setValue(getToImageUrl());
    valueDataArray = toArg.createValueDataArray();
    if(nullptr != valueDataArray)
    {
        valueDataArray->initData();
        m_drawingData.setAllArgumentValues(valueDataArray);
    }
    valueDataArray = nullptr;
    useNewToImageUrl();
}


OpenGLDrawingImageData *QuizImageData::getDrawingData()
{
    return m_drawingData.copy();
}
