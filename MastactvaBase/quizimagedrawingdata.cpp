#include "quizimagedrawingdata.h"
#include "../MastactvaBase/utils.h"


const QString &ArgumentBase::getName() const
{
    return m_name;
}

const QString &ArgumentBase::getStorage() const
{
    return m_storage;
}

const QString &ArgumentBase::getType() const
{
    return m_type;
}

const QString &ArgumentBase::getValue() const
{
    return m_value;
}

const QString &ArgumentBase::getDefaultValue() const
{
    return m_defaultValue;
}

bool ArgumentBase::isValid() const
{
    return !getName().trimmed().isEmpty()
            && !getStorage().trimmed().isEmpty()
            && !getType().trimmed().isEmpty()
            ;
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

    QString type = getType().trimmed();
    const auto fit = std::find_if(
                std::begin(typeInfos),
                std::end(typeInfos),
                [type] (const TypeInfo &ti_) -> bool
    {
        return std::get<toUnderlaying(Fields::name)>(ti_) == type;
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


template<class JsonType_>
std::unique_ptr<ArgumentBase> fromJsonT(const JsonType_ &obj_, bool isInput_, const ArgumentBase *)
{
    std::unique_ptr<ArgumentBase> result = std::make_unique<ArgumentBase>();
    result->setInput(isInput_);

    using FieldType = typename IJsonFieldInfo<JsonType_>::Type;
    JsonFieldsInfo<JsonType_> jsonFieldsInfo;
    jsonFieldsInfo.template add<QString>(
                g_argumentNameName,
                FieldType::Required,
                [&result](QString name_)
    {
        result->setName(std::move(name_));
    });
    jsonFieldsInfo.template add<QString>(
                g_argumentStorageName,
                FieldType::Required,
                [&result](QString storage_)
    {
        result->setStorage(std::move(storage_));
    });
    jsonFieldsInfo.template add<QString>(
                g_argumentTypeName,
                FieldType::Required,
                [&result](QString type_)
    {
        result->setType(std::move(type_));
    });
    jsonFieldsInfo.template add<QString>(
                g_argumentValueName,
                FieldType::Optional,
                [&result](QString value_)
    {
        result->setValue(std::move(value_));
    });
    jsonFieldsInfo.template add<QString>(
                g_argumentDefaultValueName,
                FieldType::Optional,
                [&result](QString defaultValue_)
    {
        result->setDefaultValue(std::move(defaultValue_));
    });

    if(!jsonFieldsInfo.get(obj_)
            || !result->isValid()
            )
    {
        return {nullptr};
    }

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


void drawing_data::QuizImageObject::setTexture(const QString &name_, const QString &newFilename_)
{
    auto fit = std::find_if(
                std::begin(textures),
                std::end(textures),
                [&name_](const drawing_data::Texture &texture_)->bool
    {
        return texture_.name == name_;
    });
    if(std::end(textures) == fit)
    {
        return;
    }
    fit->filename = newFilename_;
}


void drawing_data::QuizImageObjects::setTexture(const QString &name_, const QString &newFilename_)
{
    for(std::shared_ptr<QuizImageObject> &object_ : objects)
    {
        if(!object_.operator bool())
        {
            continue;
        }

        object_->setTexture(name_, newFilename_);
    }
}
