#include "quizimagedata.h"
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
    using TypeInfo = std::tuple<const char *, int, bool, bool, bool, int>;
    static const TypeInfo typeInfos[] = {
        { "int", 1, false, false, false, 1 },
        { "float", 1, true, false, false, 1 },
        { "vec2", 2, true, false, false, 2 },
        { "vec3", 3, true, false, false, 3 },
        { "vec4", 4, true, false, false, 4 },
        { "mat2", 4, true, true, false, 4 },
        { "mat3", 9, true, true, false, 9 },
        { "mat4", 16, true, true, false, 16 },
        { "strings", -1, true, true, true, 1 },
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
    const int tupleSize = std::get<5>(*fit);

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
        return new ArgumentValueDataStringArray(*this, arraySize, tupleSize);
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
    const auto fit = std::find_if(
                std::begin(*this),
                std::end(*this),
                [&argumentName_, isAny_, isInput_](const ArgumentBase &arg_) -> bool
    {
        return arg_.getName() == argumentName_ && (isAny_ || (!isAny_ && arg_.isInput() == isInput_));
    });
    return std::end(*this) != fit;
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
        { "attribute", true, false, false },
        { "uniform", false, true, false },
        { "indexes", false, false, true },
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

const QVector<GLfloat> &ArgumentValueDataFloatArray::getValues() const
{
    return m_values;
}

bool ArgumentValueDataFloatArray::isMatrixType() const
{
    return m_isMatrixType;
}


ArgumentValueDataStringArray::ArgumentValueDataStringArray(const ArgumentBase &from_, int arraySize_, int tupleSize_)
    : ArgumentValueDataArray(from_, arraySize_, tupleSize_)
{
}

OpenGLArgumentValueBase *ArgumentValueDataStringArray::createOpenGlValue()
{
    if(m_isAttribute)
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



void OpenGLArgumentValueBase::initAttribureValueId(QOpenGLShaderProgram *program_, const QString &name_)
{
    m_id = program_->attributeLocation(name_);
    program_->bindAttributeLocation(name_, m_id);
}

void OpenGLArgumentValueBase::useAttributeValue(
        QOpenGLShaderProgram *program_,
        GLenum type_,
        int offset_,
        int tupleSize_) const
{
    program_->setAttributeBuffer(m_id, type_, offset_, tupleSize_, 0);
    program_->enableAttributeArray(m_id);
}

void OpenGLArgumentValueBase::writeAttributeValue(QOpenGLBuffer *vbo_, int offset_, int sizeItems_, const QVector<GLint> &values_, int partSize_, int tupleSize_) const
{
    Q_ASSERT(partSize_ <= tupleSize_ * sizeItems_);
    vbo_->write(
                offset_,
                &values_[0],
                partSize_
                );
}

void OpenGLArgumentValueBase::writeAttributeValue(QOpenGLBuffer *vbo_, int offset_, int sizeItems_, const QVector<GLfloat> &values_, int partSize_, int tupleSize_) const
{
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
    program_->disableAttributeArray(m_id);
}

void OpenGLArgumentValueBase::initUniformValueId(QOpenGLShaderProgram *program_, const QString &name_)
{
    m_id = program_->uniformLocation(name_);
}

void OpenGLArgumentValueBase::setUniformValue(
        QOpenGLShaderProgram *program_,
        const QVector<GLint> &values_,
        int arraySize_,
        bool isMatrixType) const
{
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
    f_->glDrawArrays(GL_TRIANGLES, 0, (size_ / 3) * 3);
}


bool IQuizImageDataArtefact::setArtefact(const Artefact *artefact_, int stepIndex_)
{
    if(nullptr == artefact_ || !artefact_->isObjectLoaded()) { return false; }
    // before set data as setData can add arguments, so setup default arguments
    if(!setArguments(artefact_->getArtefactArg())) { return false; }
    if(!setData(loadBinaryFileByUrl(artefact_->filename()))) { return false; }
    m_stepIndex = stepIndex_;
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

bool IQuizImageDataArtefact::setArguments(const QString &shaderCode_)
{
    if(shaderCode_.trimmed().isEmpty()) { return false; }

    return true;
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


bool QuizImageDataTexture1DArtefact::setData(const QByteArray &data_)
{
    return m_texture1D.loadFromData(data_);
}


bool QuizImageDataTexture2DArtefact::setData(const QByteArray &data_)
{
    return m_texture2D.loadFromData(data_);
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

QuizImageDataObject *QuizImageDataObject::create(EffectObjects *effectObject_)
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
        ObjectArtefact *objectArtefact = effectObject_->getObjectArtefacts()->dataItemAtImpl(i);
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


QuizImageData::QuizImageData()
{
    m_newFromImageUrl = g_noImage;
    m_newToImageUrl = g_noImage;
}

void QuizImageData::setFromImageUrl(const QString &fromImageUrl_)
{
    m_newFromImageUrl = fromImageUrl_;
    if(m_newFromImageUrl.isEmpty() || m_newFromImageUrl == g_noImageQRC)
    {
        m_newFromImageUrl = g_noImage;
    }
}

void QuizImageData::setToImageUrl(const QString &toImageUrl_)
{
    m_newToImageUrl = toImageUrl_;
    if(m_newToImageUrl.isEmpty() || m_newToImageUrl == g_noImageQRC)
    {
        m_newToImageUrl = g_noImage;
    }
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

void QuizImageData::setEffectId(int effectId_)
{
    m_newEffectId = effectId_;
}

void QuizImageData::setArgumentSetId(int argumentSetId_)
{
    m_newArgumentSetId = argumentSetId_;
}

bool QuizImageData::effectChanged() const
{
    return m_newEffectId != m_effectId || m_newArgumentSetId != m_argumentSetId;
}

void QuizImageData::useNewEffect()
{
    m_newEffectId = m_effectId;
    m_newArgumentSetId = m_argumentSetId;
}

bool QuizImageData::isFromImageIsUrl() const
{
    return !isDefaultImage(m_fromImageUrl);
}

bool QuizImageData::isToImageIsUrl() const
{
    return !isDefaultImage(m_toImageUrl);
}

const QString &QuizImageData::getFromImageUrl() const
{
    return m_fromImageUrl;
}

const QString &QuizImageData::getToImageUrl() const
{
    return m_toImageUrl;
}

void QuizImageData::extractArguments(const Effect *effect_, const EffectArgSet *argumentSet_)
{
    if(nullptr == effect_)
    {
        clearArguments();
        setShaders(QString(), QString());
        initDefaultShaders();
        return;
    }

    // read shaders files
    ArtefactTypeModel *artefactTypeModel = static_cast<ArtefactTypeModel *>(
                QMLObjectsBase::getInstance().getListModel(g_artefactTypeModel)
                );
    Q_ASSERT(nullptr != artefactTypeModel && artefactTypeModel->sizeImpl() > 0);
    ServerFiles *sf = QMLObjectsBase::getInstance().getServerFiles();
    Q_ASSERT(nullptr != sf);

    setShaders(QString(), QString());

    const EffectObjectsModel *effectObjectsModel = effect_->getEffectObjects();
    Q_ASSERT(nullptr != effectObjectsModel && effectObjectsModel->isListLoaded());

    for(int i = 0; i < effectObjectsModel->sizeImpl(); i++ )
    {
        const EffectObjects *effectObjects = effectObjectsModel->dataItemAtImpl(i);
        Q_ASSERT(nullptr != effectObjects);

        const ObjectArtefactModel *effectObjectArtefactModel = effectObjects->getObjectArtefacts();
        Q_ASSERT(nullptr != effectObjectArtefactModel && effectObjectArtefactModel->isListLoadedImpl());

        for(int j = 0; j < effectObjectArtefactModel->sizeImpl(); j++)
        {
            const ObjectArtefact *effectObjectArtefact = effectObjectArtefactModel->dataItemAtImpl(j);
            Q_ASSERT(nullptr != effectObjectArtefact);

            const ArtefactModel * artefactModel = effectObjectArtefact->getArtefact();
            Q_ASSERT(nullptr != artefactModel && artefactModel->isListLoadedImpl());

            for(int k = 0; k < artefactModel->sizeImpl(); k++)
            {
                const Artefact *artefact = artefactModel->dataItemAtImpl(k);
                Q_ASSERT(nullptr != artefact);
                Q_ASSERT(sf->isUrlDownloaded(artefact->filename()));

                ArtefactType *artefactType = artefactTypeModel->findDataItemByIdImpl(artefact->type());
                QString shaderText;
                Q_ASSERT(nullptr != artefactType);

                if(g_artefactTypeVertex == artefactType->type() ||
                        g_artefactTypeFragment == artefactType->type())
                {
                    shaderText = ::loadTextFileByUrl(artefact->filename());
                    if(g_artefactTypeVertex == artefactType->type())
                    {
                        setVertexShader(shaderText);
                    }
                    else if(g_artefactTypeFragment == artefactType->type())
                    {
                        setFragmentShader(shaderText);
                    }
                }
            }
        }
    }

    initDefaultShaders();

    ArtefactArgTypeModel *artefactArgTypeModel = static_cast<ArtefactArgTypeModel *>(
                QMLObjectsBase::getInstance().getListModel(g_artefactArgTypeModel)
                );
    Q_ASSERT(nullptr != artefactArgTypeModel && artefactArgTypeModel->isListLoaded());

    clearArguments();

    // read default arguments values
    const EffectArgModel *effectArguments = effect_->getEffectArguments();
    Q_ASSERT(nullptr != effectArguments && effectArguments->isListLoaded());
    for(int i = 0; i < effectArguments->sizeImpl(); ++i)
    {
        ArgumentInfo ai;
        const EffectArg *effectArgument = effectArguments->dataItemAtImpl(i);
        Q_ASSERT(nullptr != effectArgument);
        ai.setName(effectArgument->name());
        const ArtefactArgType *argType = artefactArgTypeModel->findDataItemByIdImpl(effectArgument->argTypeId());
        Q_ASSERT(nullptr != argType);
        ai.setArgId(effectArgument->id());
        ai.setType(argType->type());
        ai.setValue(effectArgument->defaultValue());
        appendArguments(ai);
    }

    if(nullptr != argumentSet_)
    {
        const EffectArgValueModel *argumentValuesModel = argumentSet_->getArgumentValues();
        Q_ASSERT(nullptr != argumentValuesModel && argumentValuesModel->isListLoaded());
        for(int i = 0; i < argumentValuesModel->sizeImpl(); ++i)
        {
            const EffectArgValue *effectArgumentValue = argumentValuesModel->dataItemAtImpl(i);
            Q_ASSERT(nullptr != effectArgumentValue);
            const int argId = effectArgumentValue->getArgId();
            setArgumentValue(argId, effectArgumentValue->value());
        }
    }
}
