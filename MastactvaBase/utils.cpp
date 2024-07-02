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

#include "utils.h"
#include <random>
#include <QStringList>
#include <QUrl>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QCryptographicHash>
#include <QTimeZone>
#include <QTextStream>
#include <QDebug>
#include "../MastactvaBase/qmlobjects.h"
#include "../MastactvaBase/serverfiles.h"
#include "../MastactvaBase/names.h"
#include "../MastactvaBase/defines.h"


QString leftDoubleCR(const QString &str_)
{
    QString res(str_);
    QStringList frags = res.split("\n\n", Qt::SkipEmptyParts);
    for(QString &s: frags)
    {
        s.replace("\n", " ");
    }
    return frags.join("\n\n");
}

qreal sqr(qreal val_)
{
    return val_*val_;
}

qreal cube(qreal val_)
{
    return val_ * sqr(val_);
}

bool isSpace(const QChar &ch_)
{
    const QChar::Category c = ch_.category();
    const bool res = (c >= QChar::Separator_Space && c <= QChar::Separator_Paragraph) ||
            (c >= QChar::Punctuation_Connector && c <= QChar::Punctuation_Other) ||
            (c >= QChar::Mark_SpacingCombining && c <= QChar::Mark_Enclosing)
            ;
    return res;
}

bool isSymbol(const QChar &ch_)
{
    const QChar::Category c = ch_.category();
    const bool res = (c >= QChar::Letter_Uppercase && c <= QChar::Letter_Other) ||
            (c >= QChar::Symbol_Math && c <= QChar::Symbol_Other)
            ;
    return res;
}

bool isLetterNumeric(const QChar &ch_)
{
    const QChar::Category c = ch_.category();
    const bool res = ((c >= QChar::Letter_Uppercase) && (c <= QChar::Letter_Other)) ||
            ((c >= QChar::Number_DecimalDigit) && (c <= QChar::Number_Other));
    return res;
}

bool isNumeric(const QChar &ch_)
{
    const QChar::Category c = ch_.category();
    const bool res = (c >= QChar::Number_DecimalDigit) && (c <= QChar::Number_Other);
    return res;
}

bool isNumeric(const QString &str_)
{
    return std::all_of(std::begin(str_), std::end(str_), [](const QChar &ch)->bool
    {
        return isNumeric(ch);
    });
}

QString absoluteHomePath(const QString &path_)
{
    return path_.startsWith("~/") ? QDir{QDir::homePath()}.filePath(path_.mid(2)) : path_;
}


//static const char *g_argStorageAttribure = "attribute";
//static const char *g_argStorageUniform = "uniform";
//static const char *g_argPrecisionLow = "lowp";
//static const char *g_argPrecisionMedium = "mediump";
//static const char *g_argPrecisionHigh = "highp";


QString calculateHash(const QByteArray &data_)
{
    return QString("%1").arg(QString(QCryptographicHash::hash(data_, QCryptographicHash::RealSha3_256).toHex()));
}

QString calculateHash(const QString &data_)
{
    return calculateHash(data_.toUtf8());
}

QString calculateFileURLHash(const QString &fileUrl_)
{
    QUrl url(fileUrl_);
    QString filename = url.toLocalFile();
    if(filename.isEmpty())
    {
        QFile f1(fileUrl_);
        if(!f1.open(QIODevice::ReadOnly)) { return QString(); }
        QByteArray fd = f1.readAll();
        return calculateHash(fd);
    }
    else
    {
        QFile f1(filename);
        if(!f1.open(QIODevice::ReadOnly)) { return QString(); }
        QByteArray fd = f1.readAll();
        return calculateHash(fd);
    }
}

QString loadTextFile(const QString &filename_)
{
    QByteArray fd = loadBinaryFile(filename_);
    return getTextFromBinaryData(fd);
}

QString loadTextFileUrl(const QString &filenameUrl_)
{
    QUrl url(filenameUrl_);
    return loadTextFile(url.toLocalFile());
}

#if defined(QMLOBJECTS_CPP) && defined(SERVERFILES_CPP)

QString loadTextFileByUrl(const QString &filenameUrl_, bool useServerFiles_ /*= true*/)
{
    if(useServerFiles_)
    {
        ServerFiles *sf = QMLObjectsBase::getInstance().getServerFiles();
        if(!sf ||
                !sf->isUrlDownloaded(filenameUrl_)
                ) { return QString(); }
        return loadTextFileUrl(sf->get(filenameUrl_));
    }
    else
    {
        return loadTextFileUrl(filenameUrl_);
    }
}

#endif

QByteArray loadBinaryFile(const QString &filename_)
{
    QFile file(filename_);
    if(!file.open(QIODevice::ReadOnly)) { return QByteArray(); }
    QByteArray fd = file.readAll();
    return fd;
}

QByteArray loadBinaryFileUrl(const QString &filenameUrl_)
{
    QUrl url(filenameUrl_);
    return loadBinaryFile(url.toLocalFile());
}

#if defined(QMLOBJECTS_CPP) && defined(SERVERFILES_CPP)

QByteArray loadBinaryFileByUrl(const QString &filenameUrl_, bool useServerFiles_ /*= true*/)
{
    if(useServerFiles_)
    {
        ServerFiles *sf = QMLObjectsBase::getInstance().getServerFiles();
        if(!sf ||
                !sf->isUrlDownloaded(filenameUrl_)
                ) { return QByteArray(); }
        return loadBinaryFileUrl(sf->get(filenameUrl_));
    }
    else
    {
        return loadBinaryFileUrl(filenameUrl_);
    }
}

#endif

QString getTextFromBinaryData(const QByteArray &data_)
{
    return QString::fromUtf8(data_);
}

void saveTextFile(const QString filepath_, const QString &text_)
{
    QFile file(filepath_);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return;
    }
    QTextStream stream(&file);
    stream << text_;
}

#if QT_CONFIG(opengl)

bool set_value(const QString &valStr_, GLint& val_)
{
    bool ok = false;
    val_ = QVariant::fromValue(valStr_).toInt(&ok);
    return ok;
}

bool set_value(const QString &valStr_, GLfloat& val_)
{
    bool ok = false;
    val_ = QVariant::fromValue(valStr_).toDouble(&ok);
    return ok;
}

bool set_value(const QString &valStr_, QString& val_)
{
    val_ = valStr_;
    return true;
}

void generateUniformRands(const QVector<GLfloat> &args_, QVector<GLfloat> &valuesArray_)
{
    if(args_.size() < 2) { return; }
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> dis(args_[0], args_[1]);
    for(int i = 0; i < valuesArray_.size(); i++)
    {
        valuesArray_[i] = dis(gen);
    }
}

void generateUniformRands(const QVector<GLint> &args_, QVector<GLint> &valuesArray_)
{
    if(args_.size() < 2) { return; }
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> dis(args_[0], args_[1]);
    for(int i = 0; i < valuesArray_.size(); i++)
    {
        valuesArray_[i] = dis(gen);
    }
}

#endif  // #if QT_CONFIG(opengl)


bool Bool::get()
{
    return m_val;
}

void Bool::set(bool val_)
{
    m_val = val_;
}

bool Bool::val()
{
    return get();
}

bool& Bool::ref()
{
    return m_val;
}


bool isDefaultImage(const QString &imageURLStr_)
{
    return imageURLStr_ == g_noImage ||
            imageURLStr_ == g_noImageQRC
            ;
}

bool isDefaultImage(const QUrl &imageUrl_)
{
    return isDefaultImage(imageUrl_.toString());
}

QString setDefaultImageIfEmpty(const QString &imageURLStr_)
{
    if(imageURLStr_.isEmpty() ||
            isDefaultImage(imageURLStr_)
            )
    {
        return g_noImage;
    }
    else
    {
        return imageURLStr_;
    }
}

#if QT_CONFIG(opengl)

QStringList getOpenGLErrors()
{
    QStringList res;
    using ErrorInfoType = std::tuple<GLenum, const char*>;
    static const ErrorInfoType errors[] = {
        {GL_NO_ERROR, "GL_NO_ERROR"},
        {GL_INVALID_ENUM, "GL_INVALID_ENUM"},
        {GL_INVALID_VALUE, "GL_INVALID_VALUE"},
        {GL_INVALID_OPERATION, "GL_INVALID_OPERATION"},
        {GL_INVALID_FRAMEBUFFER_OPERATION, "GL_INVALID_FRAMEBUFFER_OPERATION"},
        {GL_OUT_OF_MEMORY, "GL_OUT_OF_MEMORY"},
    };

    GLenum error = GL_NO_ERROR;
    do {
        error = glGetError();
        if (error != GL_NO_ERROR)
        {
            const auto fit = std::find_if(
                        std::begin(errors),
                        std::end(errors),
                        [&error](const ErrorInfoType &errorInfo_)->bool
            {
               return error == std::get<0>(errorInfo_);
            });
            if(std::end(errors) != fit)
            {
                res.append(std::get<1>(*fit));
            }
        }
    } while (error != GL_NO_ERROR);
    return res;
}

#endif  // #if QT_CONFIG(opengl)

QString subpathJoin(const QString &relPath_, const QString &namespacePath_)
{
    if(!namespacePath_.isEmpty() && !relPath_.isEmpty())
    {
        return namespacePath_ + QDir::separator() + relPath_;
    }
    else if(!namespacePath_.isEmpty() && relPath_.isEmpty())
    {
        return namespacePath_;
    }
    else if(namespacePath_.isEmpty() && !relPath_.isEmpty())
    {
        return relPath_;
    }
    else
    {
        return QString();
    }
}

bool isSimpleQVariantType(const QVariant &data_)
{
    static QMetaType s_simpleTypes[] = {
        QMetaType{QMetaType::Bool},
        QMetaType{QMetaType::Int},
        QMetaType{QMetaType::UInt},
        QMetaType{QMetaType::LongLong},
        QMetaType{QMetaType::ULongLong},
        QMetaType{QMetaType::Double},
        QMetaType{QMetaType::QChar},
        QMetaType{QMetaType::QString},
        QMetaType{QMetaType::QStringList},
        QMetaType{QMetaType::QByteArray},
        QMetaType{QMetaType::QBitArray},
        QMetaType{QMetaType::QDate},
        QMetaType{QMetaType::QTime},
        QMetaType{QMetaType::QDateTime},
        QMetaType{QMetaType::QUrl},
        QMetaType{QMetaType::QLocale},
        QMetaType{QMetaType::QRect},
        QMetaType{QMetaType::QRectF},
        QMetaType{QMetaType::QSize},
        QMetaType{QMetaType::QSizeF},
        QMetaType{QMetaType::QLine},
        QMetaType{QMetaType::QLineF},
        QMetaType{QMetaType::QPoint},
        QMetaType{QMetaType::QPointF}
    };
    return std::end(s_simpleTypes)
            != std::find(
                std::begin(s_simpleTypes),
                std::end(s_simpleTypes),
                data_.metaType()
                )
            ;
}

QStringList getUniqueValues(const QStringList &values_)
{
    QSet<QString> unique;
    for(const QString &value_ : qAsConst(values_))
    {
        unique.insert(value_);
    }
    QStringList result;
    result.reserve(unique.size());
    for(const QString &value_ : qAsConst(unique))
    {
        result.push_back(value_);
    }
    return result;
}

QStringList replace(const QStringList &stringList_, const QString from_, const QString & to_)
{
    QStringList result;
    result.reserve(stringList_.size());
    for(const QString &str_ : qAsConst(stringList_))
    {
        QString r = str_;
        r.replace(from_, to_);
        result.push_back(r);
    }
    return result;
}

QStringList removeEmpty(const QStringList &values_)
{
    QStringList result;
    result.reserve(values_.size());
    for(const QString &str_ : qAsConst(values_))
    {
        if(str_.trimmed().isEmpty())
        {
            continue;
        }
        result.push_back(str_);
    }
    return result;
}

QStringList trimmed(const QStringList &values_)
{
    QStringList result;
    result.reserve(values_.size());
    for(const QString &str_ : qAsConst(values_))
    {
        result.push_back(str_.trimmed());
    }
    return result;
}

