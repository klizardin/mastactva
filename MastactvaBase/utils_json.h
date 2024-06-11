#ifndef UTILS_JSON_H
#define UTILS_JSON_H

#include <QJsonValue>

/*
 * classes and functions to convert json data to the types
 * {
*/
class JsonValue : protected QJsonValue
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
    void set(const JsonType_ &obj_, const QString &name_)
    {
        static_cast<QJsonValue&>(*this) = obj_[name_];
    }

    bool isValid() const
    {
        return !isUndefined();
    }
};


template<typename Type_>
struct JsonValueTraints
{
    using type = JsonValue;
};

#define JSON_VALUE_TRAITS(Type_, JsonValueType_)    \
template<>                                          \
struct JsonValueTraints<Type_>                      \
{                                                   \
    using type = JsonValueType_;                    \
};                                                  \
/*end define JSON_VALUE_TRAITS*/

class JsonString : public JsonValue
{
public:
    using type = QString;

    bool isValid() const
    {
        return JsonValue::isValid() && isString();
    }

    operator type() const
    {
        return toString();
    }
};
JSON_VALUE_TRAITS(QString, JsonString)


class JsonBool : public JsonValue
{
public:
    using type = bool;

    bool isValid() const
    {
        return JsonValue::isValid() && isBool();
    }

    operator type() const
    {
        return toBool();
    }
};
JSON_VALUE_TRAITS(bool, JsonBool)


class JsonInt : public JsonValue
{
public:
    using type = int;

    bool isValid() const
    {
        return JsonValue::isValid() && isDouble();
    }

    operator type() const
    {
        return toInt();
    }
};
JSON_VALUE_TRAITS(int, JsonInt)


class JsonDouble : public JsonValue
{
public:
    using type = double;

    bool isValid() const
    {
        return JsonValue::isValid() && isDouble();
    }

    operator type() const
    {
        return toDouble();
    }
};
JSON_VALUE_TRAITS(double, JsonDouble)


template<class JsonObjectType_>
class IJsonFieldInfo
{
public:
    enum class Type {
        Optional,
        Required
    };

    virtual ~IJsonFieldInfo() = default;
    virtual bool get(const JsonObjectType_ &obj_) const = 0;
};

// TODO: add pinup tests
template<class JsonObjectType_, class ValueType_>
class IJsonFieldInfoImpl : public IJsonFieldInfo<JsonObjectType_>
{
public:
    using FieldType = typename IJsonFieldInfo<JsonObjectType_>::Type;
    using JsonValueType = typename JsonValueTraints<ValueType_>::type;

    IJsonFieldInfoImpl(
            QString &&name_,
            FieldType fieldType_,
            std::function<void(ValueType_)> &&setter_
            )
        : m_name(std::move(name_)),
          m_fieldType(fieldType_),
          m_setter(std::move(setter_))
    {
    }

    virtual bool get(const JsonObjectType_ &obj_) const override
    {
        JsonValueType value;
        value.set(obj_, m_name);
        if(value.isValid())
        {
            m_setter(value);
        }
        else
        {
            return m_fieldType == IJsonFieldInfo<JsonObjectType_>::Type::Optional;
        }
        return true;
    }
private:
    QString m_name;
    FieldType m_fieldType;
    std::function<void(ValueType_)> m_setter;
};

template<class JsonObjectType_>
class JsonFieldsInfo
{
public:
    using FieldType = typename IJsonFieldInfo<JsonObjectType_>::Type;

    JsonFieldsInfo()
    {
    }

    template<class ValueType_>
    void add(
            QString name_,
            FieldType fieldType_,
            std::function<void(ValueType_)> setter_
            )
    {
        std::unique_ptr<IJsonFieldInfo<JsonObjectType_>> val(
                    new IJsonFieldInfoImpl<JsonObjectType_, ValueType_>(
                        std::move(name_),
                        fieldType_,
                        std::move(setter_)
                        )
                    );
        m_fields.push_back(std::move(val));
    }

    bool get(const JsonObjectType_ &obj_)
    {
        for(const auto &field : m_fields)
        {
            if(!field->get(obj_))
            {
                return false;
            }
        }
        return true;
    }

private:
    std::vector<std::unique_ptr<IJsonFieldInfo<JsonObjectType_>>> m_fields;
};
// }


#endif // UTILS_JSON_H
