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
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLShader>
#include "utils_json.h"
#include "utils_enum.h"
#include "utils_optional.h"



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
QString absoluteHomePath(const QString &path_);
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
template<typename Type_> inline
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
template<typename Type_> inline
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


template<typename Type> inline
Type sign(const Type& val)
{
    return val > 0 ? 1: val < 0 ? -1 : 0;
}

template<typename Type>
struct VarSetReset
{
    VarSetReset(Type& var_, const Type& valueToSet_, const Type& valueToReset_)
        :var(var_),valueToReset(valueToReset_)
    {
        var = valueToSet_;
    }
    ~VarSetReset()
    {
        var = valueToReset;
    }
private:
    Type& var;
    Type valueToReset;
};


#endif // UTILS_H
