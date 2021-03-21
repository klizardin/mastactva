#include "quizimagedata.h"
#include <set>
#include "../MastactvaBase/serverfiles.h"
#include "../MastactvaBase/utils.h"
#include "../MastactvaModels/artefacttype.h"
#include "../MastactvaModels/artefactargstorage.h"
#include "../MastactvaModels/artefactargtype.h"
#include "../MastactvaModels/objectartefact.h"
#include "../MastactvaModels/effect.h"
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

ArgumentValueDataArray *ArgumentBase::createValueDataArray() const
{
    using TypeInfo = std::tuple<const char *, int, bool, bool, bool, bool, int>;
    static const TypeInfo typeInfos[] = {
        { g_intTypeName, 1, false, false, false, false, 1 },
        { g_floatTypeName, 1, true, false, false, false, 1 },
        { g_vec2TypeName, 2, true, false, false, false, 2 },
        { g_vec3TypeName, 3, true, false, false, false, 3 },
        { g_vec4TypeName, 4, true, false, false, false, 4 },
        { g_mat2TypeName, 4, true, true, false, false, 4 },
        { g_mat3TypeName, 9, true, true, false, false, 9 },
        { g_mat4TypeName, 16, true, true, false, false, 16 },
        { g_stringsTypeName, -1, false, false, true, false, 1 },
        { g_sampler1DTypeName, 1, false, false, true, true, 1 },
        { g_sampler2DTypeName, 1, false, false, true, true, 1 },
        { g_sampler3DTypeName, 1, false, false, true, true, 1 },
    };

    const auto fit = std::find_if(
                std::begin(typeInfos),
                std::end(typeInfos),
                [this] (const TypeInfo &ti_) -> bool
    {
        return std::get<0>(ti_) == getType();
    });

    Q_ASSERT(std::end(typeInfos) != fit);   // unknown type
    if(std::end(typeInfos) == fit) { return nullptr; }

    const int arraySize = std::get<1>(*fit);
    const bool isIntArrayType = !std::get<2>(*fit);
    const bool isFloatArrayType = std::get<2>(*fit);
    const bool isMatrixType = std::get<3>(*fit);
    const bool isStringArrayType = std::get<4>(*fit);
    const bool isTextureType = std::get<5>(*fit);
    const int tupleSize = std::get<6>(*fit);

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
    }
    else
    {
        setArray(getDefaultValue());
    }
}

int ArgumentValueDataArray::getArraySize() const
{
    return m_arraySize;
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
        return new OpenGLArgumentAttributeValueT<ArgumentValueDataIntArray>(*this);
    }
    else if(m_isUniform)
    {
        return new OpenGLArgumentUniformValueT<ArgumentValueDataIntArray>(*this);
    }
    else if(m_isIndex)
    {
        return new OpenGLArgumentIndexValueT<ArgumentValueDataIntArray>(*this);
    }
    else
    {
        return nullptr;
    }
}

const QVector<GLint> &ArgumentValueDataIntArray::intValues() const
{
    return valueOrFish(getValues(), static_cast<const QVector<GLint> *>(nullptr));
}

const QVector<GLfloat> &ArgumentValueDataIntArray::floatValues() const
{
    return valueOrFish(getValues(), static_cast<const QVector<GLfloat> *>(nullptr));
}

const QVector<QString> &ArgumentValueDataIntArray::stringValues() const
{
    return valueOrFish(getValues(), static_cast<const QVector<QString> *>(nullptr));
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
        return new OpenGLArgumentAttributeValueT<ArgumentValueDataFloatArray>(*this);
    }
    else if(m_isUniform)
    {
        return new OpenGLArgumentUniformValueT<ArgumentValueDataFloatArray>(*this);
    }
    else if(m_isIndex)
    {
        return new OpenGLArgumentIndexValueT<ArgumentValueDataFloatArray>(*this);
    }
    else
    {
        return nullptr;
    }
}

const QVector<GLint> &ArgumentValueDataFloatArray::intValues() const
{
    return valueOrFish(getValues(), static_cast<const QVector<GLint> *>(nullptr));
}

const QVector<GLfloat> &ArgumentValueDataFloatArray::floatValues() const
{
    return valueOrFish(getValues(), static_cast<const QVector<GLfloat> *>(nullptr));
}

const QVector<QString> &ArgumentValueDataFloatArray::stringValues() const
{
    return valueOrFish(getValues(), static_cast<const QVector<QString> *>(nullptr));
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
        return new OpenGLArgumentTextureValueT<ArgumentValueDataStringArray>(*this);
    }
    else if(m_isAttribute)
    {
        return new OpenGLArgumentAttributeValueT<ArgumentValueDataStringArray>(*this);
    }
    else if(m_isUniform)
    {
        return new OpenGLArgumentUniformValueT<ArgumentValueDataStringArray>(*this);
    }
    else if(m_isIndex)
    {
        return new OpenGLArgumentIndexValueT<ArgumentValueDataStringArray>(*this);
    }
    else
    {
        return nullptr;
    }
}

const QVector<GLint> &ArgumentValueDataStringArray::intValues() const
{
    return valueOrFish(getValues(), static_cast<const QVector<GLint> *>(nullptr));
}

const QVector<GLfloat> &ArgumentValueDataStringArray::floatValues() const
{
    return valueOrFish(getValues(), static_cast<const QVector<GLfloat> *>(nullptr));
}

const QVector<QString> &ArgumentValueDataStringArray::stringValues() const
{
    return valueOrFish(getValues(), static_cast<const QVector<QString> *>(nullptr));
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
    for(const QString &objectNameKey_ : qAsConst(rootKeys))
    {
        DataTableValue *objectValue = getRootChild(objectNameKey_);
        if(nullptr == objectValue) { continue; }
        QList<QString> indexNamesKeys = objectValue->getChildrenKeys();
        for(const QString &indexNameKey_ : qAsConst(indexNamesKeys))
        {
            DataTableValue *indexValue = objectValue->getChild(indexNameKey_);
            if(nullptr == indexValue) { continue; }
            QList<QString> argumentNamesKeys = indexValue->getChildrenKeys();
            for(const QString &argumentNameKey_ : qAsConst(argumentNamesKeys))
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
    for(const QString &objectNameKey_ : qAsConst(rootKeys))
    {
        DataTableValue *objectValue = getRootChild(objectNameKey_);
        if(nullptr == objectValue) { continue; }
        QList<QString> indexNamesKeys = objectValue->getChildrenKeys();
        for(const QString &indexNameKey_ : qAsConst(indexNamesKeys))
        {
            DataTableValue *indexValue = objectValue->getChild(indexNameKey_);
            if(nullptr == indexValue) { continue; }
            QList<QString> argumentNamesKeys = indexValue->getChildrenKeys();
            for(const QString &argumentNameKey_ : qAsConst(argumentNamesKeys))
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
    for(const QString &objectNameKey_ : qAsConst(rootKeys))
    {
        DataTableValue *objectValue = getRootChild(objectNameKey_);
        if(nullptr == objectValue) { continue; }
        QList<QString> indexNamesKeys = objectValue->getChildrenKeys();
        for(const QString &indexNameKey_ : qAsConst(indexNamesKeys))
        {
            DataTableValue *indexValue = objectValue->getChild(indexNameKey_);
            if(nullptr == indexValue) { continue; }
            QList<QString> argumentNamesKeys = indexValue->getChildrenKeys();
            for(const QString &argumentNameKey_ : qAsConst(argumentNamesKeys))
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
    for(const QString &argumentNameKey_ : qAsConst(argumentNamesKeys))
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

void OpenGLArgumentValueBase::setTextureIndex(int textureIndex_)
{
    Q_UNUSED(textureIndex_);
}

QString OpenGLArgumentValueBase::getTextureName() const
{
    return QString();
}

void OpenGLArgumentValueBase::createTexture(QImage *image_)
{
    Q_UNUSED(image_);
}

void OpenGLArgumentValueBase::bindTexture(QOpenGLFunctions *f_)
{
    Q_UNUSED(f_);
}

void OpenGLArgumentValueBase::initAttribureValueId(QOpenGLShaderProgram *program_, const QString &name_)
{
    if(nullptr == program_) { return; }
    m_id = program_->attributeLocation(name_);
    program_->bindAttributeLocation(name_, m_id);
}

void OpenGLArgumentValueBase::useAttributeValue(
        QOpenGLShaderProgram *program_,
        GLenum type_,
        int offset_,
        int tupleSize_) const
{
    if(nullptr == program_) { return; }
    program_->setAttributeBuffer(m_id, type_, offset_, tupleSize_, 0);
    program_->enableAttributeArray(m_id);
}

void OpenGLArgumentValueBase::writeAttributeValue(QOpenGLBuffer *vbo_, int offset_, int sizeItems_, const QVector<GLint> &values_, int partSize_, int tupleSize_) const
{
    if(nullptr == vbo_) { return; }
    Q_ASSERT(partSize_ <= tupleSize_ * sizeItems_);
    vbo_->write(
                offset_,
                &values_[0],
                partSize_
                );
}

void OpenGLArgumentValueBase::writeAttributeValue(QOpenGLBuffer *vbo_, int offset_, int sizeItems_, const QVector<GLfloat> &values_, int partSize_, int tupleSize_) const
{
    if(nullptr == vbo_) { return; }
    Q_ASSERT(partSize_ <= tupleSize_ * sizeItems_);
    vbo_->write(
                offset_,
                &values_[0],
                partSize_
                );
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
    if(nullptr == program_) { return; }
    program_->disableAttributeArray(m_id);
}

void OpenGLArgumentValueBase::initUniformValueId(QOpenGLShaderProgram *program_, const QString &name_)
{
    if(nullptr == program_) { return; }
    m_id = program_->uniformLocation(name_);
}

void OpenGLArgumentValueBase::setUniformValue(
        QOpenGLShaderProgram *program_,
        const QVector<GLint> &values_,
        int arraySize_,
        bool isMatrixType) const
{
    if(nullptr == program_) { return; }
    Q_UNUSED(isMatrixType);
    if(arraySize_ >= 1 && values_.size() >= 1)
    {
        program_->setUniformValue(m_id, values_[0]);
    }
}

void OpenGLArgumentValueBase::setUniformValue(
        QOpenGLShaderProgram *program_,
        const QVector<GLfloat> &values_,
        int arraySize_,
        bool isMatrixType) const
{
    if(nullptr == program_) { return; }
    if(1 == arraySize_)
    {
        program_->setUniformValue(m_id, values_[0]);
    }
    else if(2 == arraySize_)
    {
        program_->setUniformValue(m_id, values_[0],
                values_[1]);
    }
    else if(3 == arraySize_)
    {
        program_->setUniformValue(m_id, values_[0],
                values_[1], values_[2]);
    }
    else if(4 == arraySize_ && !isMatrixType)
    {
        program_->setUniformValue(m_id, values_[0],
                values_[1], values_[2], values_[3]);
    }
    else if(4 == arraySize_ && isMatrixType)
    {
        QMatrix2x2 m(&values_[0]);
        program_->setUniformValue(m_id, m);
    }
    else if(9 == arraySize_ && isMatrixType)
    {
        QMatrix3x3 m(&values_[0]);
        program_->setUniformValue(m_id, m);
    }
    else if(16 == arraySize_ && isMatrixType)
    {
        QMatrix4x4 m(&values_[0]);
        program_->setUniformValue(m_id, m);
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

void OpenGLArgumentValueBase::drawTrianlesArray(QOpenGLFunctions *f_, int size_) const
{
    if(nullptr == f_) { return; }
    f_->glDrawArrays(GL_TRIANGLES, 0, (size_ / 3) * 3);
}

void OpenGLArgumentValueBase::createTextureFromImage(QOpenGLTexture *&texture_, QImage *image_)
{
    if(nullptr != texture_)
    {
        delete texture_;
        texture_ = nullptr;
    }
    if(nullptr == image_ || image_->isNull()) { return; }
    texture_ = new QOpenGLTexture(image_->mirrored(), QOpenGLTexture::GenerateMipMaps);
    texture_->setMagnificationFilter(QOpenGLTexture::Filter::LinearMipMapLinear);
    texture_->setWrapMode(QOpenGLTexture::WrapMode::ClampToBorder);
    texture_->setBorderColor(1, 1, 1, 0);
}

void OpenGLArgumentValueBase::bindTexture(QOpenGLFunctions *f_, QOpenGLTexture *texture_, int textureIndex_)
{
    if(nullptr == f_ || nullptr == texture_) { return; }
    if(textureIndex_ < 0) { return; }
    f_->glActiveTexture(GL_TEXTURE0 + textureIndex_);
    texture_->bind();
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

    for(const Comment &comment: qAsConst(comments))
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
    for(IQuizImageDataArtefact *&ptr : m_artefacts)
    {
        delete ptr;
        ptr = nullptr;
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

void DrawingTextureArtefact::setTexture(const QImage &image_)
{
    m_image = image_;
}


bool DrawingShaderArtefact::operator == (const DrawingShaderArtefact &drawingArtefact_) const
{
    return static_cast<const DrawingArtefact &>(*this) ==  static_cast<const DrawingArtefact &>(drawingArtefact_);
}

bool DrawingShaderArtefact::operator < (const DrawingShaderArtefact &drawingArtefact_) const
{
    return static_cast<const DrawingArtefact &>(*this) <  static_cast<const DrawingArtefact &>(drawingArtefact_);
}

void DrawingShaderArtefact::deepCopy()
{
    m_shaderCode = QString(m_shaderCode.constData(), m_shaderCode.length());
}

void DrawingShaderArtefact::setShader(const QString &shaderCode_)
{
    m_shaderCode = shaderCode_;
}


DrawingArgument::DrawingArgument(ArgumentValueDataArray *valueDataArray_ /*= nullptr*/)
    :m_valueDataArray(valueDataArray_),
      m_impl(nullptr)
{
}

DrawingArgument::~DrawingArgument()
{
    m_valueDataArray = nullptr;
    delete m_impl;
    m_impl = nullptr;
}

void DrawingArgument::create(QOpenGLShaderProgram *program_)
{
    if(nullptr != m_impl)
    {
        m_impl->create(program_);
    }
}

bool DrawingArgument::isTexture() const
{
    return nullptr != m_impl ? m_impl->isTexture() : false;
}

void DrawingArgument::setTextureIndex(int textureIndex_)
{
    if(nullptr != m_impl)
    {
        m_impl->setTextureIndex(textureIndex_);
    }
}

QString DrawingArgument::getTextureName() const
{
    return nullptr != m_impl ? m_impl->getTextureName() : QString();
}

void DrawingArgument::createTexture(QImage *image_)
{
    if(nullptr != m_impl)
    {
        m_impl->createTexture(image_);
    }
}

int DrawingArgument::getArraySize() const
{
    return nullptr != m_impl ? m_impl->getArraySize() : 0;
}

int DrawingArgument::getMaxIndex() const
{
    return nullptr != m_impl ? m_impl->getMaxIndex() : 0;
}

int DrawingArgument::getVBOPartSize() const
{
    return nullptr != m_impl ? m_impl->getVBOPartSize() : 0;
}

void DrawingArgument::setVBOPartOffset(int offset_)
{
    if(nullptr != m_impl)
    {
        m_impl->setVBOPartOffset(offset_);
    }
}

void DrawingArgument::writeVBOPart(QOpenGLBuffer *vbo_, int offset_, int sizeItems_) const
{
    if(nullptr != m_impl)
    {
        m_impl->writeVBOPart(vbo_, offset_, sizeItems_);
    }
}

void DrawingArgument::use(QOpenGLShaderProgram *program_) const
{
    if(nullptr != m_impl)
    {
        m_impl->use(program_);
    }
}

void DrawingArgument::bindTexture(QOpenGLFunctions *f_)
{
    if(nullptr != m_impl)
    {
        m_impl->bindTexture(f_);
    }
}

void DrawingArgument::draw(QOpenGLFunctions *f_) const
{
    if(nullptr != m_impl)
    {
        m_impl->draw(f_);
    }
}

void DrawingArgument::release(QOpenGLShaderProgram *program_) const
{
    if(nullptr != m_impl)
    {
        m_impl->release(program_);
    }
}

QString DrawingArgument::getArgumentName() const
{
    return nullptr != m_valueDataArray ? m_valueDataArray->getName() : QString();
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
    return getArgumentName() == argument_.getArgumentName();
}

bool DrawingArgument::operator < (const DrawingArgument &argument_) const
{
    return getArgumentName() < argument_.getArgumentName();
}

bool DrawingArgument::doesValueEqual(const DrawingArgument &argument_) const
{
    return intValues() == argument_.intValues() &&
            floatValues() == argument_.floatValues() &&
            stringValues() == argument_.stringValues()
            ;
}

void DrawingArgument::deepCopy()
{
    if(nullptr != m_impl ||
            nullptr == m_valueDataArray
            ) { return; }
    m_impl = m_valueDataArray->createOpenGlValue();
    m_valueDataArray = nullptr;
}


OpenGLDrawingImageData *DrawingImageData::copy()
{
    OpenGLDrawingImageData *result = new OpenGLDrawingImageData();

    result->m_textures.reserve(m_texturesSet.size());
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
        result->m_arguments.push_back(argument_);
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
    }
    for(DrawingArgument &argument_ : result->m_arguments)
    {
        argument_.deepCopy();
    }
    return result;
}

void DrawingImageData::setAllArgumentValues(ArgumentValueDataArray *argumentValueDataArray_)
{
    DrawingArgument arg(argumentValueDataArray_);
    std::multiset<DrawingArgument>::const_iterator itb = m_argumentsSet.lower_bound(arg);
    std::multiset<DrawingArgument>::const_iterator ite = m_argumentsSet.upper_bound(arg);
    for(std::multiset<DrawingArgument>::const_iterator it = itb;
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
    for(const QuizImageDataObject *obj_ : objects_)
    {
        if(nullptr == obj_) { continue; }
        for(const IQuizImageDataArtefact *artefact_: obj_->getArtefacts())
        {
            if(nullptr == artefact_) { continue; }

            if(artefact_->isVertexShader())
            {
                DrawingShaderArtefact drawingArtefact;
                drawingArtefact.setId(artefact_->getId());
                drawingArtefact.setShader(artefact_->getVertexShader());
                vertexShaders.insert(drawingArtefact);
            }
            else if(artefact_->isFragmentShader())
            {
                DrawingShaderArtefact drawingArtefact;
                drawingArtefact.setId(artefact_->getId());
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
    freeTempValueDataArray();
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
    for(QuizImageDataObject *&ptr_ : m_objects)
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
    m_drawingData.addTexture(fromTexture);

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
    m_tempValueDataArray.push_back(valueDataArray);
    valueDataArray = nullptr;
    useNewFromImageUrl();

    // set to image
    DrawingTextureArtefact toTexture;
    toTexture.setFilename(getToImageUrl());
    toTexture.setTexture(QImage(getToImageUrl()));
    m_drawingData.addTexture(toTexture);

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
    m_tempValueDataArray.push_back(valueDataArray);
    valueDataArray = nullptr;
    useNewToImageUrl();
}

void QuizImageData::freeTempValueDataArray()
{
    for(ArgumentValueDataArray *&ptr_ : m_tempValueDataArray)
    {
        delete ptr_;
        ptr_ = nullptr;
    }
    m_tempValueDataArray.clear();
}

OpenGLDrawingImageData *QuizImageData::getDrawingData()
{
    OpenGLDrawingImageData *result = m_drawingData.copy();
    freeTempValueDataArray();
    return result;
}
