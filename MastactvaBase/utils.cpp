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


static const char *g_nl = "\n";
static const char *g_cb = "/*";
static const char *g_ce = "*/";
static const char *g_luaCb = "--[[";
static const char *g_luaCe = "--]]";
static const char *g_leftAlign = "<";
static const char *g_nameStart = "@";
//static const char *g_argStorageAttribure = "attribute";
//static const char *g_argStorageUniform = "uniform";
//static const char *g_argPrecisionLow = "lowp";
//static const char *g_argPrecisionMedium = "mediump";
//static const char *g_argPrecisionHigh = "highp";


static int low_bound(const QVector<int> &indexes_, int p_)
{
    int i0 = 0, i1 = indexes_.size() - 1;
    for(;i0 + 1 < i1;)
    {
        int i2 = (i0 + i1) / 2;
        if(indexes_[i2] < p_) { i0 = i2; }
        else { i1 = i2; }
    }
    return i0;
}

static int high_bound(const QVector<int> &indexes_, int p_)
{
    int i0 = 0, i1 = indexes_.size() - 1;
    for(;i0 + 1 < i1;)
    {
        int i2 = (i0 + i1) / 2;
        if(indexes_[i2] < p_) { i0 = i2; }
        else { i1 = i2; }
    }
    return i1;
}

Comment::Comment(int cb_ /*= -1*/, int ce_ /*= -1*/,int lb_ /*= -1*/, int le_ /*= -1*/)
    :cb(cb_), ce(ce_), lb(lb_), le(le_)
{}

bool isSpaces(const QString &str_, int i0_, int i1_)
{
    for(int i = i0_; i < i1_; i++)
    {
        if(!isSpace(str_.at(i))) { return false; }
    }
    return true;
}

bool Comment::isAlignedToLeft(const QString &shaderText_) const
{
    const int i0 = shaderText_.indexOf(g_leftAlign, cb);
    if(i0 < 0 || i0 >= ce) { return false; }
    return isSpaces(shaderText_, cb, i0);
}

void Comment::findLeftLine(const QVector<int> &indexesOfNL_, const QString &shaderText_)
{
    for(int i0 = low_bound(indexesOfNL_, cb),           // start index on nl
        te = cb - QString(g_cb).length();                        // test line before comment
        ;
        te = indexesOfNL_[i0],                          // test line before nl
        i0--                                            // test prev line
        )
    {
        int tb = indexesOfNL_[i0] + QString(g_nl).length();      // test line begining, skip nl
        if(!isSpaces(shaderText_, tb, te))              // is not empty test line
        {
            lb = tb;                                    // use this non empty line
            le = te;
            break;
        }
        else if(0 == i0)                                // if first line tested
        {
            lb = 0;                                     // use line before first nl
            le = tb;
            break;
        }
                                                        // continue testing
    }
}

void Comment::findRightLine(const QVector<int> &indexesOfNL_, const QString &shaderText_)
{
    for(int i0 = high_bound(indexesOfNL_, ce + QString(g_ce).length()),  // start index on nl
        tb = ce + QString(g_cb).length();                                // test line after comment
        ;
        tb = indexesOfNL_[i0] + QString(g_nl).length(),                  // test line after nl
        i0++                                                    // test prev line
        )
    {
        int te = indexesOfNL_[i0];                              // test line end
        if(!isSpaces(shaderText_, tb, te))                      // is not empty test line
        {
            lb = tb;                                            // use this non empty line
            le = te;
            break;
        }
        else if(i0 + 1 == indexesOfNL_.size())                  // if last line tested
        {
            lb = tb;                                            // use line before first nl
            le = shaderText_.length();
            break;
        }
                                                                // continue testing
    }
}

const QHash<QString, QString> &Comment::values() const
{
    return m_values;
}

void Comment::extractValues(const QString &shaderText_)
{
    int i = shaderText_.indexOf(g_nameStart, cb);
    if(0 > i || i >= ce) { return; }
    for(; i >= cb && i < ce;)
    {
        i += QString(g_nameStart).length();
        int nb = i;
        for(;i < ce && isLetterNumeric(shaderText_.at(i)); ++i) {}; // find name
        int ne = i;
        QString name = shaderText_.mid(nb, ne - nb);
        i = shaderText_.indexOf(g_nameStart, i);
        if(!name.isEmpty())
        {
            QString value = (i < 0 || i > ce)
                    ? shaderText_.mid(ne, ce - ne)
                    : shaderText_.mid(ne, i - ne);
            m_values.insert(name, value);
        }
    }
}

void Comment::extractLineValues(const QString &shaderText_)
{
    if(m_values.contains(g_argumentName))
    {
        extractArgumentsLineValues(shaderText_);
    }
}

void Comment::extractArgumentsLineValues(const QString &shaderText_)
{
    if(lb < 0 || le < 0) { return; }
    QString line = shaderText_.mid(lb, le - lb).trimmed();
    // TODO: make lexical analyze
    //       for object geometry data case
    int i = 0;
    int sb = i;
    for(;i < line.length(); )
    {
        QChar c = line.at(i);
        if(!isLetterNumeric(c)) { break; }
        i++;
    }
    int se = i;
    for(;i < line.length(); )
    {
        QChar c = line.at(i);
        if(isLetterNumeric(c)) { break; }
        i++;
    }
    int pb = i;
    for(;i < line.length(); )
    {
        QChar c = line.at(i);
        if(!isLetterNumeric(c)) { break; }
        i++;
    }
    int pe = i;
    for(;i < line.length(); )
    {
        QChar c = line.at(i);
        if(isLetterNumeric(c)) { break; }
        i++;
    }
    int tb = i;
    for(;i < line.length(); )
    {
        QChar c = line.at(i);
        if(!isLetterNumeric(c)) { break; }
        i++;
    }
    int te = i;
    for(;i < line.length(); )
    {
        QChar c = line.at(i);
        if(isLetterNumeric(c)) { break; }
        i++;
    }
    int nb = i;
    for(;i < line.length(); )
    {
        QChar c = line.at(i);
        if(!isLetterNumeric(c)) { break; }
        i++;
    }
    int ne = i;
    m_values.insert(g_storageName, line.mid(sb, se - sb));
    m_values.insert(g_precisionName, line.mid(pb, pe - pb));
    m_values.insert(g_typeName, line.mid(tb, te - tb));
    m_values.insert(g_nameName, line.mid(nb, ne - nb));
}


void getComments(
        const QString &shaderText_,
        const char *cb_,
        const char * ce_,
        bool extracNearType_,
        QVector<Comment> &comments_
        )
{
    comments_.clear();
    QVector<int> indexesOfNL;
    int p = 0; int cnt = 0;
    for(p = shaderText_.indexOf(g_nl); p >= 0; p += QString(g_nl).length(), p = shaderText_.indexOf(g_nl, p))
    {
        cnt++;
    }
    indexesOfNL.reserve(cnt);
    for(p = shaderText_.indexOf(g_nl); p >= 0; p += QString(g_nl).length(), p = shaderText_.indexOf(g_nl, p))
    {
        indexesOfNL.push_back(p);
    }

    for(cnt = 0, p = 0; p >= 0;)
    {
        p = shaderText_.indexOf(cb_, p);
        if(p < 0) { break; }
        p += QString(cb_).length();
        p = shaderText_.indexOf(ce_, p);
        if(p < 0) { break; }
        p += QString(ce_).length();
        cnt++;
    }
    comments_.reserve(cnt);
    for(p = 0; p >= 0;)
    {
        p = shaderText_.indexOf(cb_, p);
        if(p < 0) { break; }
        const int cb = p + QString(cb_).length();
        p += QString(cb_).length();
        p = shaderText_.indexOf(ce_, p);
        if(p < 0) { break; }
        const int ce = p;
        p += QString(ce_).length();
        comments_.push_back(Comment(cb, ce));
    }

    for(Comment &comment : comments_)
    {
        comment.extractValues(shaderText_);
        if(!extracNearType_)
        {
            continue;
        }

        if(comment.isAlignedToLeft(shaderText_))
        {
            comment.findLeftLine(indexesOfNL, shaderText_);
        }
        else
        {
            comment.findRightLine(indexesOfNL, shaderText_);
        }
        comment.extractLineValues(shaderText_);
    }

    for(Comment &comment : comments_)
    {
        Q_UNUSED(comment);
        //qDebug() << comment.values();
    }
}

void getShaderComments(const QString &shaderText_, QVector<Comment> &comments_)
{
    getComments(shaderText_, g_cb, g_ce, true, comments_);
}

void getLuaComments(const QString &shaderText_, QVector<Comment> &comments_)
{
    getComments(shaderText_, g_luaCb, g_luaCe, false, comments_);
}

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
