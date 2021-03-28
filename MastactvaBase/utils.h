#ifndef UTILS_H
#define UTILS_H


#include <QString>
#include <QHash>
#include <QDateTime>
#include <QByteArray>
#include <type_traits>
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
QDateTime nowTz();
QDateTime dateTimeFromJsonString(const QString& dateTimeZ_);
QString dateTimeToJsonString(const QDateTime &dt_);
QString loadTextFile(const QString &filename_);
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


#endif // UTILS_H
