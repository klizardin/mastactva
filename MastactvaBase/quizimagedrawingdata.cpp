#include "quizimagedrawingdata.h"


const QString &ArgumentBase::getName() const
{
    return m_name;
}

void ArgumentBase::setName(QString name_)
{
    m_name = std::move(name_);
}

const QString &ArgumentBase::getStorage() const
{
    return m_storage;
}

void ArgumentBase::setStorage(QString storage_)
{
    m_storage = std::move(storage_);
}

const QString &ArgumentBase::getType() const
{
    return m_type;
}

void ArgumentBase::setType(QString type_)
{
    m_type = std::move(type_);
}

const QString &ArgumentBase::getValue() const
{
    return m_value;
}

void ArgumentBase::setValue(QString value_)
{
    m_value = std::move(value_);
}

const QString &ArgumentBase::getDefaultValue() const
{
    return m_defaultValue;
}

void ArgumentBase::setDefaultValue(QString defaultValue_)
{
    m_defaultValue = std::move(defaultValue_);
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

// TODO: rerfactoring to std::unique_ptr<>
ArgumentValueDataArray *ArgumentBase::createValueDataArray() const
{
    enum class Fields : int {name, arraySize, tupleSize, isIntType, isFloatType, isMatrixType, isString, isTexture};
    using TypeInfo = std::tuple<const char *, int, bool, bool, bool, bool, bool, int>;
    static const TypeInfo typeInfos[] = {
        { g_intTypeName, 1, 1,        true,  false, false, false, false },
        { g_ivec2TypeName, 2, 2,      true,  false, false, false, false },
        { g_ivec3TypeName, 3, 3,      true,  false, false, false, false },
        { g_ivec4TypeName, 4, 4,      true,  false, false, false, false },
        { g_floatTypeName, 1, 1,      false, true,  false, false, false },
        { g_vec2TypeName, 2, 2,       false, true,  false, false, false },
        { g_vec3TypeName, 3, 3,       false, true,  false, false, false },
        { g_vec4TypeName, 4, 4,       false, true,  false, false, false },
        { g_mat2TypeName, 4, 4,       false, true,  true,  false, false },
        { g_mat3TypeName, 9, 9,       false, true,  true,  false, false },
        { g_mat4TypeName, 16,16,      false, true,  true,  false, false },
        { g_stringsTypeName, -1,1,    false, false, false, true,  false },
        { g_sampler1DTypeName, 1,1,   false, false, false, true,  true  },
        { g_sampler2DTypeName, 1,1,   false, false, false, true,  true  },
        { g_sampler3DTypeName, 1,1,   false, false, false, true,  true  },
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
    const int tupleSize = std::get<toUnderlaying(Fields::tupleSize)>(*fit);
    const bool isIntArrayType = std::get<toUnderlaying(Fields::isIntType)>(*fit);
    const bool isFloatArrayType = std::get<toUnderlaying(Fields::isFloatType)>(*fit);
    const bool isMatrixType = std::get<toUnderlaying(Fields::isMatrixType)>(*fit);
    const bool isStringArrayType = std::get<toUnderlaying(Fields::isString)>(*fit);
    const bool isTextureType = std::get<toUnderlaying(Fields::isTexture)>(*fit);

    if(isIntArrayType)
    {
        //return new ArgumentValueDataIntArray(*this, arraySize, tupleSize);
    }
    else if(isFloatArrayType)
    {
        //return new ArgumentValueDataFloatArray(*this, arraySize, tupleSize, isMatrixType);
    }
    else if(isStringArrayType)
    {
        //return new ArgumentValueDataStringArray(*this, arraySize, tupleSize, isTextureType);
    }
    else
    {
        //return nullptr;
    }
    return nullptr;
}

class JsonString : protected QJsonValue
{
public:
    template<typename JsonType_,
             typename std::enable_if<
                 std::is_base_of<QJsonValue, JsonType_>::value
                 || std::is_base_of<QJsonObject, JsonType_>::value
                 || std::is_base_of<QJsonArray, JsonType_>::value
                 || std::is_base_of<QJsonDocument, JsonType_>::value
                 ,bool
                 >::type = true>
    JsonString(const JsonType_ &obj_, const QString &name_)
    {
        static_cast<QJsonValue&>(*this) = obj_[name_];
    }
    operator bool() const
    {
        return isUndefined() && isString();
    }
    bool operator ! () const
    {
        return !operator bool();
    }
    QString get() const
    {
        return toString();
    }
};

template<class JsonType_>
std::unique_ptr<ArgumentBase> fromJsonT(const JsonType_ &obj_, bool isInput_, const ArgumentBase *)
{
    std::unique_ptr<ArgumentBase> result(new ArgumentBase());
    result->setInput(isInput_);

    JsonString jsvName(obj_, g_argumentNameName);
    if(!jsvName) { return nullptr; }
    result->setName(jsvName.get());

    JsonString jsvStorage(obj_, g_argumentStorageName);
    if(!jsvStorage) { return nullptr; }
    result->setStorage(jsvStorage.get());

    JsonString jsvType(obj_, g_argumentTypeName);
    if(!jsvType) { return nullptr; }
    result->setType(jsvType.get());

    JsonString jsvValue(obj_, g_argumentValueName);
    bool hasValue = false;
    if(jsvValue)
    {
        result->setValue(jsvValue.get());
        hasValue = true;
    }

    JsonString jsvDefaultValue(obj_, g_argumentDefaultValueName);
    if(!jsvDefaultValue && !hasValue) { return nullptr; }
    result->setDefaultValue(jsvDefaultValue.get());

    return result;
}

std::unique_ptr<ArgumentBase> ArgumentBase::fromJson(const QJsonDocument &obj_, bool isInput_ /*= true*/)
{
    return fromJsonT(obj_, isInput_, static_cast<const ArgumentBase *>(nullptr));
}

std::unique_ptr<ArgumentBase> ArgumentBase::fromJson(const QJsonObject &obj_, bool isInput_ /*= true*/)
{
    return fromJsonT(obj_, isInput_, static_cast<const ArgumentBase *>(nullptr));
}
