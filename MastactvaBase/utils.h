#ifndef UTILS_H
#define UTILS_H


#include <type_traits>
#include <vector>
#include <memory>
#include <QString>
#include <QHash>
#include <QDateTime>
#include <QByteArray>
#include <QJsonValue>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLTexture>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLShader>


class Comment
{
public:
    Comment(int cb_ = -1, int ce_ = -1,int lb_ = -1, int le_ = -1);
    bool isAlignedToLeft(const QString &shaderText_) const;
    void findLeftLine(const QVector<int> &indexesOfNL_, const QString &shaderText_);
    void findRightLine(const QVector<int> &indexesOfNL_, const QString &shaderText_);
    void extractValues(const QString &shaderText_);
    void extractLineValues(const QString &shaderText_);
    void extractArgumentsLineValues(const QString &shaderText_);
    const QHash<QString, QString> &values() const;

private:
    int cb = 0;
    int ce = 0;
    int lb = 0;
    int le = 0;
    QHash<QString, QString> m_values;
};


class Bool
{
public:
    Bool() = default;
    bool get();
    void set(bool val_);
    bool val();
    bool& ref();
private:
    bool m_val = false;
};


QString leftDoubleCR(const QString &str_);
qreal sqr(qreal val_);
qreal cube(qreal val_);
bool isSpace(const QChar &ch_);
bool isSymbol(const QChar &ch_);
bool isLetterNumeric(const QChar &ch_);
bool isNumeric(const QChar &ch_);
bool isNumeric(const QString &str_);
void getShaderComments(const QString &shaderText_, QVector<Comment> &comments_);
QString calculateHash(const QString &data_);
QString calculateFileURLHash(const QString &fileUrl_);
QString loadTextFile(const QString &filename_);
// TODO: separate utils. it is serverfiles utils
QString loadTextFileByUrl(const QString &filenameUrl_, bool useServerFiles_ = true);
QByteArray loadBinaryFile(const QString &filename_);
QByteArray loadBinaryFileByUrl(const QString &filenameUrl_, bool useServerFiles_ = true);
QString getTextFromBinaryData(const QByteArray &data_);


#if QT_CONFIG(opengl)

bool set_value(const QString &valStr_, GLint& val_);
bool set_value(const QString &valStr_, GLfloat& val_);
bool set_value(const QString &valStr_, QString& val_);
void generateUniformRands(const QVector<GLfloat> &args_, QVector<GLfloat> &valuesArray_);
void generateUniformRands(const QVector<GLint> &args_, QVector<GLint> &valuesArray_);
QStringList getOpenGLErrors();

#endif  // #if QT_CONFIG(opengl)


template<typename Type_>
void extractValues(const QVariantList &values_, QVector<Type_> &valuesArray_, int arraySize_)
{
    int pos = 0;
    for(const QVariant &val_ : qAsConst(values_))
    {
        QString val = val_.toString().trimmed();
        if(val.isEmpty()) { continue; }
        if(arraySize_ >= 0 && pos >= valuesArray_.size())
        {
            continue;
        }
        Type_ value = Type_();
        if(set_value(val, value))
        {
            if(arraySize_ < 0 && pos >= valuesArray_.size())
            {
                valuesArray_.resize(pos+1);
            }
            valuesArray_[pos] = value;
            ++pos;
        }
    }
}

template<typename Type_>
void extractValues(const QString &valuesStr_, QVector<Type_> &valuesArray_, int arraySize_)
{
    QString value = valuesStr_;
    value.replace(QString("("), QString(", "));
    value.replace(QString(")"), QString(", "));
    value.replace(QString("{"), QString(", "));
    value.replace(QString("}"), QString(", "));
    value.replace(QString("\n"), QString(", "));
    QStringList values = value.split(QString(","));
    QVariantList valuesVar;
    valuesVar.reserve(values.size());
    for(const QString &s : qAsConst(values))
    {
        valuesVar.push_back(s);
    }
    extractValues(valuesVar, valuesArray_, arraySize_);
}

template <typename EnumType_>
constexpr auto to_underlying(EnumType_ enumValue_) noexcept
{
    return static_cast<std::underlying_type_t<EnumType_>>(enumValue_);
}

template <typename EnumType_>
constexpr auto to_enum(const std::underlying_type_t<EnumType_> &val_) noexcept
{
    return static_cast<EnumType_>(val_);
}

template <typename Type_>
constexpr bool has_value(const std::pair<Type_, bool> &data_) noexcept
{
    return data_.second;
}

template <typename Type_>
constexpr bool has_value(const std::tuple<Type_, bool> &data_) noexcept
{
    return std::get<1>(data_);
}

template <typename Type_>
constexpr bool & has_value(std::pair<Type_, bool> &data_) noexcept
{
    return data_.second;
}

template <typename Type_>
constexpr bool & has_value(std::tuple<Type_, bool> &data_) noexcept
{
    return std::get<1>(data_);
}

template <typename Type_>
constexpr const Type_ & value(const std::pair<Type_, bool> &data_) noexcept
{
    return data_.first;
}

template <typename Type_>
constexpr const Type_ & value(const std::tuple<Type_, bool> &data_) noexcept
{
    return std::get<0>(data_);
}

template <typename Type_>
constexpr Type_ & value(std::pair<Type_, bool> &data_) noexcept
{
    return data_.first;
}

template <typename Type_>
constexpr Type_ & value(std::tuple<Type_, bool> &data_) noexcept
{
    return std::get<0>(data_);
}

bool isDefaultImage(const QString &imageURLStr_);
bool isDefaultImage(const QUrl &imageUrl_);
QString setDefaultImageIfEmpty(const QString &imageURLStr_);

template<typename ArgType_, typename ReturnType_>
ReturnType_ &valueOrFish(ArgType_ &value_, const ReturnType_ *)
{
    Q_UNUSED(value_);
    static ReturnType_ fish;
    return fish;
}

template<typename SameType_>
SameType_ &valueOrFish(SameType_ &value_, const SameType_ *)
{
    return value_;
}


template<typename TargetType_, typename SrcType_> inline
std::unique_ptr<TargetType_> unique_ptr_static_cast(std::unique_ptr<SrcType_> &&ptr_)
{
    return std::unique_ptr<TargetType_>{static_cast<TargetType_*>(ptr_.release())};
}


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


template<typename DataType_>
class IDefaultData
{
public:
    virtual ~IDefaultData() = default;
    virtual void initialize(DataType_ &data_) const = 0;
};


template<typename ConcreteType_, typename BaseType_>
class ConcretePtr
{
public:
    explicit ConcretePtr(BaseType_ *ptr_)
    {
        m_ptr = dynamic_cast<ConcreteType_*>(ptr_);
    }

    operator bool () const
    {
        return nullptr != m_ptr;
    }

    bool operator ! () const
    {
        return !operator bool();
    }

    ConcreteType_ * operator -> () const
    {
        Q_ASSERT(operator bool());
        return m_ptr;
    }

private:
    ConcreteType_ *m_ptr = nullptr;
};


#endif // UTILS_H
