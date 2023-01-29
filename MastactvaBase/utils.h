/*
    Copyright 2021

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

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
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLShader>


/*
 * Comment item (to parse comments from the shaders and lua code)
*/
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


QString leftDoubleCR(const QString &str_);      // return with doubled \n
qreal sqr(qreal val_);                          // squre
qreal cube(qreal val_);                         // cube
bool isSpace(const QChar &ch_);                 // is the char a space
bool isSymbol(const QChar &ch_);                // is char a symbol
bool isLetterNumeric(const QChar &ch_);         // is char somewhat a letter or a number
bool isNumeric(const QChar &ch_);               // is char a number
bool isNumeric(const QString &str_);            // is atrinmg is number
void getShaderComments(const QString &shaderText_, QVector<Comment> &comments_);    // return shader's comments
void getLuaComments(const QString &shaderText_, QVector<Comment> &comments_);       // return lua's comments
QString calculateHash(const QString &data_);            // return hash value for the text data
QString calculateFileURLHash(const QString &fileUrl_);  // return hash for the file data
QString loadTextFile(const QString &filename_);         // load text file
// TODO: separate utils. it is serverfiles utils
QString loadTextFileUrl(const QString &filenameUrl_);   // load text file by URL
QString loadTextFileByUrl(const QString &filenameUrl_, bool useServerFiles_ = true);    // (TODO: inject dep)  load text file by the server files interface
QByteArray loadBinaryFile(const QString &filename_);    // load binary file
QByteArray loadBinaryFileUrl(const QString &filenameUrl_);  // load binary file by URL
QByteArray loadBinaryFileByUrl(const QString &filenameUrl_, bool useServerFiles_ = true);    // (TODO: inject dep) load binary file by the server files interface
QString getTextFromBinaryData(const QByteArray &data_);     // return text from the binary data
void saveTextFile(const QString filepath_, const QString &text_);       // save text to the text file
bool isSimpleQVariantType(const QVariant &data_);     // is type the simple qvariant type
QStringList getUniqueValues(const QStringList &values_);    // return just unique values (remove repeted values)
QStringList replace(const QStringList &stringList_, const QString from_, const QString & to_);  // replace string in the string list
QStringList removeEmpty(const QStringList &values_);    // remove empty strings from the string list
QStringList trimmed(const QStringList &values_);    // trim string list's strings

/*
 * open gl help functions
*/
#if QT_CONFIG(opengl)

bool set_value(const QString &valStr_, GLint& val_);    // set value to GLint from the string
bool set_value(const QString &valStr_, GLfloat& val_);  // set value to GLfloat from the string
bool set_value(const QString &valStr_, QString& val_);  // set value to QString from the string
void generateUniformRands(const QVector<GLfloat> &args_, QVector<GLfloat> &valuesArray_);   // generate random values
void generateUniformRands(const QVector<GLint> &args_, QVector<GLint> &valuesArray_);       // generate random values
QStringList getOpenGLErrors();  // return open gl error as list of strings

#endif  // #if QT_CONFIG(opengl)


/*
 * pathes join
*/
QString subpathJoin(const QString &relPath_, const QString &namespacePath_);


/*
 * convert QVariantList to the list of values with the template type
 * with required array size
*/
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

/*
 * convert QString list of values to the array of value with the template type
 * with required array size
*/
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


/*
 * enum conversions
 * {
*/
template <typename EnumType_> inline
constexpr auto to_underlying(EnumType_ enumValue_) noexcept
{
    return static_cast<std::underlying_type_t<EnumType_>>(enumValue_);
}

template <typename EnumType_> inline
constexpr auto to_enum(const std::underlying_type_t<EnumType_> &val_) noexcept
{
    return static_cast<EnumType_>(val_);
}
// }

/*
 * primitives of imitation of the intialized/uninitialized value type
 * with std::pair or with std::tuple
 * {
*/
template <typename Type_> inline
constexpr bool has_value(const std::pair<Type_, bool> &data_) noexcept
{
    return data_.second;
}

template <typename Type_> inline
constexpr bool has_value(const std::tuple<Type_, bool> &data_) noexcept
{
    return std::get<1>(data_);
}

template <typename Type_>
constexpr bool & has_value(std::pair<Type_, bool> &data_) noexcept
{
    return data_.second;
}

template <typename Type_> inline
constexpr bool & has_value(std::tuple<Type_, bool> &data_) noexcept
{
    return std::get<1>(data_);
}

template <typename Type_> inline
constexpr const Type_ & value(const std::pair<Type_, bool> &data_) noexcept
{
    return data_.first;
}

template <typename Type_>
constexpr const Type_ & value(const std::tuple<Type_, bool> &data_) noexcept
{
    return std::get<0>(data_);
}

template <typename Type_> inline
constexpr Type_ & value(std::pair<Type_, bool> &data_) noexcept
{
    return data_.first;
}

template <typename Type_> inline
constexpr Type_ & value(std::tuple<Type_, bool> &data_) noexcept
{
    return std::get<0>(data_);
}

template <typename Type_>
constexpr bool has_value(const std::pair<bool, Type_> &data_) noexcept
{
    return data_.first;
}

template <typename Type_> inline
constexpr bool has_value(const std::tuple<bool, Type_> &data_) noexcept
{
    return std::get<0>(data_);
}

template <typename Type_> inline
constexpr bool & has_value(std::pair<bool, Type_> &data_) noexcept
{
    return data_.first;
}

template <typename Type_> inline
constexpr bool & has_value(std::tuple<bool, Type_> &data_) noexcept
{
    return std::get<0>(data_);
}

template <typename Type_> inline
constexpr const Type_ & value(const std::pair<bool, Type_> &data_) noexcept
{
    return data_.second;
}

template <typename Type_> inline
constexpr const Type_ & value(const std::tuple<bool, Type_> &data_) noexcept
{
    return std::get<1>(data_);
}

template <typename Type_> inline
constexpr Type_ & value(std::pair<bool, Type_> &data_) noexcept
{
    return data_.second;
}

template <typename Type_> inline
constexpr Type_ & value(std::tuple<bool, Type_> &data_) noexcept
{
    return std::get<1>(data_);
}
// }

bool isDefaultImage(const QString &imageURLStr_);   // is it a default image
bool isDefaultImage(const QUrl &imageUrl_);         // is it a default image
QString setDefaultImageIfEmpty(const QString &imageURLStr_);    // setup default image if the URL is empty

/*
 * return "fish" value
 * {
*/
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
// }


// primitive to simplify the casting
template<typename TargetType_, typename SrcType_> inline
std::unique_ptr<TargetType_> unique_ptr_static_cast(std::unique_ptr<SrcType_> &&ptr_)
{
    return std::unique_ptr<TargetType_>{static_cast<TargetType_*>(ptr_.release())};
}


/*
 * classes and fucntions to convert json data to the types
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


/*
 * IDefaultData to setup some data from the implementation of the interface
*/
template<typename DataType_>
class IDefaultData
{
public:
    virtual ~IDefaultData() = default;
    virtual void initialize(DataType_ &data_, int argsSetIndex_ = 0) const = 0;
    virtual int getArgSetCount() const { return 1; }
    virtual std::pair<const char *, const char *> getDescription() const { return {nullptr, nullptr}; }
};


/*
 * class to imcapsulate dynamic cast for the types (BaseType_ -> DerivedType_)
*/
template<typename DerivedType_, typename BaseType_>
class DynamicCastPtr
{
public:
    explicit DynamicCastPtr(BaseType_ *ptr_)
    {
        m_ptr = dynamic_cast<DerivedType_*>(ptr_);
    }

    operator bool () const
    {
        return nullptr != m_ptr;
    }

    bool operator ! () const
    {
        return !operator bool();
    }

    DerivedType_ * operator -> () const
    {
        Q_ASSERT(operator bool());
        return m_ptr;
    }

private:
    DerivedType_ *m_ptr = nullptr;
};


#endif // UTILS_H
