#include "utils.h"
#include <random>
#include <QStringList>
#include <QUrl>
#include <QFile>
#include <QFileInfo>
#include <QCryptographicHash>
#include <QTimeZone>
#include <QTextStream>
#include <QTextCodec>
#include <QRegExp>
#include <QDebug>
#include <QJsonArray>
#include <set>
#include "../MastactvaBase/qmlobjects.h"
#include "../MastactvaBase/serverfiles.h"
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


static const char *g_nl = "\n";
static const char *g_cb = "/*";
static const char *g_ce = "*/";
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


void getShaderComments(const QString &shaderText_, QVector<Comment> &comments_)
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
        p = shaderText_.indexOf(g_cb, p);
        if(p < 0) { break; }
        p += QString(g_cb).length();
        p = shaderText_.indexOf(g_ce, p);
        if(p < 0) { break; }
        p += QString(g_ce).length();
        cnt++;
    }
    comments_.reserve(cnt);
    for(p = 0; p >= 0;)
    {
        p = shaderText_.indexOf(g_cb, p);
        if(p < 0) { break; }
        const int cb = p + QString(g_cb).length();
        p += QString(g_cb).length();
        p = shaderText_.indexOf(g_ce, p);
        if(p < 0) { break; }
        const int ce = p;
        p += QString(g_ce).length();
        comments_.push_back(Comment(cb, ce));
    }

    for(Comment &comment : comments_)
    {
        comment.extractValues(shaderText_);
        if(comment.isAlignedToLeft(shaderText_))
        {
            comment.findLeftLine(indexesOfNL, shaderText_);
        }
        else
        {
            comment.findRightLine(indexesOfNL, shaderText_);
        }
        comment.extractLineValues(shaderText_);

        //qDebug() << comment.values();
    }
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

QDateTime nowTz()
{
    const QDateTime cdt = QDateTime::currentDateTime();
    const QDateTime cdtz = QDateTime(cdt.date(), cdt.time(), QTimeZone::systemTimeZone());
    //qDebug() << dateTimeToJsonString(cdtz);
    return cdtz.toLocalTime();
}

QDateTime dateTimeFromJsonString(const QString& dateTimeZ_)
{
    QString dateTimeZ(dateTimeZ_);
    QTextStream s(&dateTimeZ);
    int year = 0, month = 0, day = 0, hours = 0, minites = 0, seconds = 0, ms = 0, tz = 0;
    char tmp;
    s >> year >> tmp >> month >> tmp >> day >> tmp;
    s >> hours >> tmp >> minites >> tmp >> seconds >> tmp >> ms >> tmp >> tz;
    if(seconds > 100)
    {
        seconds /= 1000;
        ms = seconds % 1000;
    }
    while(ms > 1000) { ms /= 10; }
    const QDateTime cdt = QDateTime(QDate(year, month, day), QTime(hours, minites, seconds, ms), QTimeZone(tz*3600));
    //qDebug() << dateTimeToJsonString(cdt);
    return cdt.toLocalTime();
}

QString dateTimeToJsonString(const QDateTime &dt_)
{
    // server uses UTC
    const QString res = dt_.toUTC().toString(Qt::DateFormat::ISODateWithMs);
    //qDebug() << res;
    return res;
}

QString loadTextFile(const QString &filename_)
{
    QByteArray fd = loadBinaryFile(filename_);
    return getTextFromBinaryData(fd);
}

QString loadTextFileByUrl(const QString &filenameUrl_, bool useServerFiles_ /*= true*/)
{
    if(useServerFiles_)
    {
        ServerFiles *sf = QMLObjectsBase::getInstance().getServerFiles();
        if(nullptr == sf ||
                !sf->isUrlDownloaded(filenameUrl_)
                ) { return QString(); }
        QUrl url(sf->get(filenameUrl_));
        return loadTextFile(url.toLocalFile());
    }
    else
    {
        QUrl url(filenameUrl_);
        return loadTextFile(url.toLocalFile());
    }
}

QByteArray loadBinaryFile(const QString &filename_)
{
    QFile file(filename_);
    if(!file.open(QIODevice::ReadOnly)) { return QByteArray(); }
    QByteArray fd = file.readAll();
    return fd;
}

QByteArray loadBinaryFileByUrl(const QString &filenameUrl_, bool useServerFiles_ /*= true*/)
{
    if(useServerFiles_)
    {
        ServerFiles *sf = QMLObjectsBase::getInstance().getServerFiles();
        if(nullptr == sf ||
                !sf->isUrlDownloaded(filenameUrl_)
                ) { return QByteArray(); }
        QUrl url(sf->get(filenameUrl_));
        return loadBinaryFile(url.toLocalFile());
    }
    else
    {
        QUrl url(filenameUrl_);
        return loadBinaryFile(url.toLocalFile());
    }
}

QString getTextFromBinaryData(const QByteArray &data_)
{
    QTextCodec *codec = QTextCodec::codecForUtfText(data_);
    if(nullptr == codec) { return QString(); }
    return codec->toUnicode(data_);
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


void WavefrontOBJItem::setLine(int line_)
{
    m_line = line_;
}

int WavefrontOBJItem::getLine() const
{
    return m_line;
}

void WavefrontOBJItem::setComment(const QString &comment_)
{
    m_comment = comment_;
}

const QString &WavefrontOBJItem::getComment() const
{
    return m_comment;
}


Vector3di::Vector3di(int x_, int y_, int z_)
    :m_x(x_), m_y(y_), m_z(z_)
{
}

int Vector3di::x() const
{
    return m_x;
}

void Vector3di::setX(int x_)
{
    m_x = x_;
}

int Vector3di::y() const
{
    return m_y;
}

void Vector3di::setY(int y_)
{
    m_y = y_;
}

int Vector3di::z() const
{
    return m_z;
}

void Vector3di::setZ(int z_)
{
    m_z = z_;
}

int Vector3di::operator[] (std::size_t index_) const
{
    switch(index_)
    {
    case 0:
        return m_x;
    case 1:
        return m_y;
    case 2:
        return m_z;
    default:
        return 0;
    }
}

int& Vector3di::operator[] (std::size_t index_)
{
    static int fish;
    switch(index_)
    {
    case 0:
        return m_x;
    case 1:
        return m_y;
    case 2:
        return m_z;
    default:
        return fish;
    }
}

void Vector3di::mask(const Vector3di &val_)
{
    for(std::size_t i = 0; i < 3; i++)
    {
        if(val_[i] < 0) { (*this)[i] = -1; }
    }
}

bool Vector3di::operator == (const Vector3di &val_) const
{
    return (*this)[0] == val_[0] &&
            (*this)[1] == val_[1] &&
            (*this)[2] == val_[2]
            ;
}

bool Vector3di::operator < (const Vector3di &val_) const
{
    if((*this)[0] < val_[0]) { return true; }
    if((*this)[0] > val_[0]) { return false; }
    if((*this)[1] < val_[1]) { return true; }
    if((*this)[1] > val_[1]) { return false; }
    return (*this)[2] < val_[2];
}


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


void parseWavefrontOBJLine(const QStringRef &line_, QVector4D &data_)
{
    QVector<QStringRef> values = line_.split(QChar(' '), Qt::SkipEmptyParts);
    data_.setZ(1.0f);
    std::size_t j = 0;
    for(const QStringRef &val_: values)
    {
        bool ok = false;
        double vald = QVariant::fromValue(val_).toDouble(&ok);
        if(ok)
        {
            data_[j] = vald;
            j++;
        }
    }
}

void parseWavefrontOBJLine(const QStringRef &line_, QVector3D &data_)
{
    QVector<QStringRef> values = line_.split(QChar(' '), Qt::SkipEmptyParts);
    std::size_t j = 0;
    for(const QStringRef &val_: values)
    {
        bool ok = false;
        double vald = QVariant::fromValue(val_).toDouble(&ok);
        if(ok)
        {
            data_[j] = vald;
            j++;
        }
    }
}

void parseWavefrontOBJLine(const QStringRef &line_, QVector<Vector3di> &data_)
{
    QVector<QStringRef> values = line_.split(QChar(' '), Qt::SkipEmptyParts);
    for(const QStringRef &val_: values)
    {
        QVector<QStringRef> faceItems = val_.split(QChar('/'), Qt::SkipEmptyParts);
        std::size_t j = 0;
        Vector3di val;
        for(const QStringRef &fval_: faceItems)
        {
            bool ok = false;
            int vali = QVariant::fromValue(fval_).toInt(&ok);
            if(ok)
            {
                val[j] = vali;
                j++;
            }
        }
        data_.push_back(val);
    }
}

void parseWavefrontOBJLine(const QStringRef &line_, QVector<int> &data_)
{
    QVector<QStringRef> values = line_.split(QChar(' '), Qt::SkipEmptyParts);
    for(const QStringRef &val_: values)
    {
        bool ok = false;
        int vali = QVariant::fromValue(val_).toInt(&ok);
        if(ok)
        {
            data_.push_back(vali);
        }
    }
}

void parseWavefrontOBJLine(const QStringRef &line_, QString &data_)
{
    data_ = line_.toString();
}

void parseWavefrontOBJLine(const QStringRef &line_, Bool &data_)
{
    bool ok = false;
    const int iv = line_.toInt(&ok);
    if(ok)
    {
        data_.set(iv != 0);
        return;
    }
    const bool fv = line_.indexOf("off", 0, Qt::CaseInsensitive) >= 0;
    if(fv)
    {
        data_.set(false);
        return;
    }
    const bool tv = line_.indexOf("on", 0, Qt::CaseInsensitive) >= 0;
    if(tv)
    {
        data_.set(true);
        return;
    }
    const bool bfv = line_.indexOf("false", 0, Qt::CaseInsensitive) >= 0;
    if(bfv)
    {
        data_.set(false);
        return;
    }
    const bool btv = line_.indexOf("true", 0, Qt::CaseInsensitive) >= 0;
    if(btv)
    {
        data_.set(true);
        return;
    }
    data_.set(false);
}


template<typename WavefrontOBJType_>
void initWavefrontOBJItem(
        WavefrontOBJType_ &d_,
        const QStringRef &dataLine_,
        const QStringRef &comment_,
        int lineNumber_,
        QVector<WavefrontOBJType_> &vec_
        )
{
    d_.setComment(comment_.toString());
    d_.setLine(lineNumber_);
    parseWavefrontOBJLine(dataLine_, d_);
    vec_.push_back(d_);
}

bool WavefrontOBJ::processLine(const QStringRef &line_, const QStringRef &comment_, int lineNumber_)
{
    QStringRef dataLine;
    if(startsWith(line_, "v ", dataLine))
    {
        WavefrontOBJVertex v;
        initWavefrontOBJItem(v, dataLine, comment_, lineNumber_, m_vertex);
        return true;
    }
    else if(startsWith(line_, "vt ", dataLine))
    {
        WavefrontOBJVertexTexture vt;
        initWavefrontOBJItem(vt, dataLine, comment_, lineNumber_, m_vertexTexture);
        return true;
    }
    else if(startsWith(line_, "vn ", dataLine))
    {
        WavefrontOBJVertexNormal vn;
        initWavefrontOBJItem(vn, dataLine, comment_, lineNumber_, m_normal);
        return true;
    }
    else if(startsWith(line_, "vp ", dataLine))
    {
        WavefrontOBJVertexParameter vp;
        initWavefrontOBJItem(vp, dataLine, comment_, lineNumber_, m_vertexParameter);
        return true;
    }
    else if(startsWith(line_, "f ", dataLine))
    {
        WavefrontOBJFaceElement f;
        initWavefrontOBJItem(f, dataLine, comment_, lineNumber_, m_faceElements);
        return true;
    }
    else if(startsWith(line_, "l ", dataLine))
    {
        WavefrontOBJLineElement l;
        initWavefrontOBJItem(l, dataLine, comment_, lineNumber_, m_lineElements);
        return true;
    }
    else if(startsWith(line_, "o ", dataLine))
    {
        WavefrontOBJObjectName o;
        initWavefrontOBJItem(o, dataLine, comment_, lineNumber_, m_objectNames);
        return true;
    }
    else if(startsWith(line_, "g ", dataLine))
    {
        WavefrontOBJGroupName g;
        initWavefrontOBJItem(g, dataLine, comment_, lineNumber_, m_groupNames);
        return true;
    }
    else if(startsWith(line_, "mtllib ", dataLine))
    {
        WavefrontOBJMaterialLib mtllib;
        initWavefrontOBJItem(mtllib, dataLine, comment_, lineNumber_, m_materialLibs);
        return true;
    }
    else if(startsWith(line_, "usemtl ", dataLine))
    {
        WavefrontOBJMaterialName usemtl;
        initWavefrontOBJItem(usemtl, dataLine, comment_, lineNumber_, m_materialNames);
        return true;
    }
    else if(startsWith(line_, "s ", dataLine))
    {
        WavefrontOBJSmoothing s;
        initWavefrontOBJItem(s, dataLine, comment_, lineNumber_, m_smoothing);
        return true;
    }
    else if(line_.trimmed().isEmpty() && !comment_.isEmpty())
    {
        // whole line comment
        WavefrontOBJItem c;
        c.setComment(comment_.toString());
        c.setLine(lineNumber_);
        m_comments.push_back(c);
        return true;
    }
    return false;
}

void WavefrontOBJ::correct()
{
    for(WavefrontOBJFaceElement &fItems_ : m_faceElements)
    {
        for(Vector3di &f_: static_cast<QVector<Vector3di>&>(fItems_))
        {
            if(f_.x() < 0)
            {
                f_.setX(m_vertex.size() - f_.x());
            }
            else
            {
                f_.setX(f_.x() - 1);
            }

            if(f_.y() < 0)
            {
                f_.setY(m_vertex.size() - f_.y());
            }
            else
            {
                f_.setY(f_.y() - 1);
            }

            if(f_.z() < 0)
            {
                f_.setZ(m_vertex.size() - f_.z());
            }
            else
            {
                f_.setZ(f_.z() - 1);
            }
        }
    }
    for(WavefrontOBJLineElement &lItems_ : m_lineElements)
    {
        for(int &l_: static_cast<QVector<int>&>(lItems_))
        {
            if(l_ < 0)
            {
                l_ = m_vertex.size() - l_;
            }
            else
            {
                --l_;
            }
        }
    }
}

bool WavefrontOBJ::validate() const
{
    const bool fInRange = std::all_of(
                std::begin(m_faceElements),
                std::end(m_faceElements),
                [this](const WavefrontOBJFaceElement &fItems_)->bool
    {
        return std::all_of(
                std::begin(static_cast<const QVector<Vector3di>&>(fItems_)),
                std::end(static_cast<const QVector<Vector3di>&>(fItems_)),
                [this](const Vector3di &f_)->bool
        {
            return (f_.x() >= 0 && f_.x() < m_vertex.size()) &&
                ((f_.y() >= -1 && f_.y() < m_vertexTexture.size())) &&
                ((f_.z() >= -1 && f_.z() < m_normal.size()))
            ;
        });
    });
    if(!fInRange) { return false; }

    const bool lInRange = std::all_of(
                std::begin(m_lineElements),
                std::end(m_lineElements),
                [this](const WavefrontOBJLineElement &lItem_)->bool
    {
        return std::all_of(
                std::begin(static_cast<const QVector<int>&>(lItem_)),
                std::end(static_cast<const QVector<int>&>(lItem_)),
                [this](const int &l_)->bool
        {
            return l_ >= 0 && l_ < m_vertex.size();
        });
    });
    if(!lInRange) { return false; }

    return true;
}

bool WavefrontOBJ::startsWith(const QStringRef &line_, const QString &str_, QStringRef &dataLine_)
{
    if(line_.startsWith(str_))
    {
        dataLine_ = line_.mid(str_.length());
        return true;
    }
    else
    {
        dataLine_.clear();
        return false;
    }
}

bool WavefrontOBJ::hasTextureIndicies() const
{
    return std::all_of(
                std::begin(m_faceElements),
                std::end(m_faceElements),
                [this](const WavefrontOBJFaceElement &fItems_)->bool
    {
        return std::all_of(
                std::begin(fItems_),
                std::end(fItems_),
                [this](const Vector3di &f_)->bool
        {
            return f_.y() >= 0 && f_.y() < m_vertexTexture.size();
        });
    });
}

bool WavefrontOBJ::hasNormalIndicies() const
{
    return std::all_of(
                std::begin(m_faceElements),
                std::end(m_faceElements),
                [this](const WavefrontOBJFaceElement &fItems_)->bool
    {
        return std::all_of(
                std::begin(fItems_),
                std::end(fItems_),
                [this](const Vector3di &f_)->bool
        {
            return f_.z() >= 0 && f_.z() < m_vertexTexture.size();
        });
    });
}

QJsonDocument WavefrontOBJ::toJsonData() const
{
    const bool hasTexIndixes = hasTextureIndicies();
    const bool hasNormIndixes = hasNormalIndicies();
    Vector3di mask(0, hasTexIndixes ? 0 : -1, hasNormIndixes ? 0 : -1);

    QJsonObject objects;
    QString prevObjectName = g_firstSpecialwordName;
    if(m_objectNames.isEmpty())
    {
        QJsonObject obj;
        buildObject(-1, -1, mask, obj);
        objects.insert(prevObjectName, obj);
    }
    else
    {
        int prevLineNumber = 0;
        for(const WavefrontOBJObjectName &objectName: m_objectNames)
        {
            QJsonObject obj;
            if(buildObject(prevLineNumber, objectName.getLine(), mask, obj))
            {
                objects.insert(prevObjectName, obj);
            }
            prevLineNumber = objectName.getLine();
            prevObjectName = static_cast<const QString&>(objectName);
        }
        if(!m_faceElements.isEmpty())
        {
            QJsonObject obj;
            buildObject(prevLineNumber, m_faceElements.back().getLine(), mask, obj);
            objects.insert(prevObjectName, obj);
        }
    }

    QJsonObject step;
    step.insert(g_currentSpecialwordName, objects);
    QJsonObject object;
    object.insert(g_currentSpecialwordName, step);

    return QJsonDocument(object);
}


QVector<WavefrontOBJFaceElement>::const_iterator WavefrontOBJ::lower_bound(
        const QVector<WavefrontOBJFaceElement> &faceElements_, int lineNumber_)
{
    int i1 = 0, i2 = faceElements_.size();
    for(;i2 - i1 > 1; )
    {
        const int i0 = (i2 - i1) / 2;
        const int i0line = faceElements_[i0].getLine();
        if(i0line <= lineNumber_)
        {
            i1 = i0;
        }
        if(i0line >= lineNumber_)
        {
            i2 = i0;
        }
    }
    return std::begin(faceElements_) + i1;
}

QVector<WavefrontOBJFaceElement>::const_iterator WavefrontOBJ::upper_bound(
        const QVector<WavefrontOBJFaceElement> &faceElements_, int lineNumber_)
{
    int i1 = 0, i2 = faceElements_.size();
    for(;i2 - i1 > 1; )
    {
        const int i0 = (i2 - i1) / 2;
        const int i0line = faceElements_[i0].getLine();
        if(i0line <= lineNumber_)
        {
            i1 = i0;
        }
        if(i0line >= lineNumber_)
        {
            i2 = i0;
        }
    }
    return std::begin(faceElements_) + i2;
}

bool WavefrontOBJ::buildObject(
        int startLineNumber_, int endLineNumber_,
        const Vector3di &mask_,
        QJsonObject &obj_
        ) const
{
    QVector<WavefrontOBJFaceElement>::const_iterator fbit = std::begin(m_faceElements);
    QVector<WavefrontOBJFaceElement>::const_iterator feit = std::end(m_faceElements);
    if(!(startLineNumber_ < 0 && endLineNumber_ < 0))
    {
        fbit = upper_bound(m_faceElements, startLineNumber_);
        feit = lower_bound(m_faceElements, endLineNumber_);
    }
    std::set<Vector3di> unique;
    int indexesCount = 0;
    for(QVector<WavefrontOBJFaceElement>::const_iterator it = fbit; it != feit; ++it)
    {
        int fc = 0;
        for(const Vector3di &f_ : static_cast<const QVector<Vector3di>&>(*it))
        {
            Vector3di f(f_);
            f.mask(mask_);
            unique.insert(f);
            // allocate triangles for polygons (pt.count() > 3)
            ++indexesCount;
            if(fc >= 3) { indexesCount += 2; }
            ++fc;
        }
    }

    if(unique.empty()) { return false; }

    QVector<QVector4D> vertex;
    QVector<QVector3D> vertexTexture;
    QVector<QVector3D> vertexNormal;
    QVector<int> indexes;
    indexes.resize(indexesCount);

    vertex.resize(unique.size());
    vertexTexture.resize(unique.size());
    vertexNormal.resize(unique.size());

    int j = 0;
    for(const Vector3di &f_: unique)
    {
        if(f_.x() >= 0 && f_.x() < m_vertex.size())
        {
            vertex[j] = m_vertex[f_.x()];
        }
        if(f_.y() >= 0 && f_.y() < m_vertexTexture.size())
        {
            vertexTexture[j] = m_vertexTexture[f_.y()];
        }
        if(f_.z() >= 0 && f_.z() < m_normal.size())
        {
            vertexNormal[j] = m_normal[f_.z()];
        }
        ++j;
    }

    for(QVector<WavefrontOBJFaceElement>::const_iterator it = fbit; it != feit; ++it)
    {
        int fc = 0;
        std::vector<std::size_t> ii;
        for(const Vector3di &f_ : static_cast<const QVector<Vector3di>&>(*it))
        {
            Vector3di f(f_);
            f.mask(mask_);
            std::set<Vector3di>::const_iterator fit = unique.find(f);
            std::size_t ci = 0;
            if(std::end(unique) != fit) { ci = std::distance(std::begin(unique), fit); }
            // build triangles
            if(fc >= 3 && !ii.empty())
            {
                const std::size_t prev = ii.back();
                ii.push_back(ii[0]);
                ii.push_back(prev);
            }
            ii.push_back(ci);
            ++fc;
        }
    }

    QJsonObject vertexObj;
    int vi = 0;
    for(const QVector4D &v_ : vertex)
    {
        QJsonArray arr;
        for(std::size_t i = 0; i < 4; i++)
        {
            arr.append(QJsonValue(v_[i]));
        }
        vertexObj.insert(QString::number(vi), arr);
        ++vi;
    }
    obj_.insert(g_vertexSpecialwordName, vertexObj);
    if(mask_.y() >= 0)
    {
        Q_ASSERT(vertexTexture.size() == vertex.size());
        int vti = 0;
        for(const QVector3D &v_ : vertexTexture)
        {
            QJsonArray arr;
            for(std::size_t i = 0; i < 3; i++)
            {
                arr.append(QJsonValue(v_[i]));
            }
            vertexObj.insert(QString::number(vti), arr);
            ++vti;
        }
        obj_.insert(g_vertexTextureSpecialwordName, vertexObj);
    }
    if(mask_.z() >= 0)
    {
        Q_ASSERT(vertexTexture.size() == vertex.size());
        int vni = 0;
        for(const QVector3D &v_ : vertexNormal)
        {
            QJsonArray arr;
            for(std::size_t i = 0; i < 3; i++)
            {
                arr.append(QJsonValue(v_[i]));
            }
            vertexObj.insert(QString::number(vni), arr);
            ++vni;
        }
        obj_.insert(g_vertexNormalSpecialwordName, vertexObj);
    }
    QJsonArray indexesArr;
    for(const int &index_ : indexes)
    {
        indexesArr.append(index_);
    }
    obj_.insert(g_indexesSpecialwordName, indexesArr);

    return true;
}


QStringRef removeComment(const QStringRef &str_, QStringRef &comment_)
{
    const int cs = str_.indexOf(QChar('#'));
    if(cs >= 0)
    {
        comment_ = str_.mid(cs);
        return str_.mid(0, cs);
    }
    else
    {
        return  str_;
    }
}

QJsonDocument graphicsOBJtoJson(const QString &objData_)
{
    WavefrontOBJ *obj = parseGraphicsOBJ(objData_);
    QJsonDocument resultDocument;
    if(nullptr != obj)
    {
        resultDocument = obj->toJsonData();
    }
    delete obj;
    obj = nullptr;
    return resultDocument;
}

WavefrontOBJ *parseGraphicsOBJ(const QString &objData_)
{
    WavefrontOBJ *res = new WavefrontOBJ();
    QVector<QStringRef> lines = objData_.splitRef(QRegExp("[\r\n]"), Qt::SkipEmptyParts);
    int lineNumber = 0;
    for(const QStringRef &line0 : lines)
    {
        QStringRef comment;
        const QStringRef line = removeComment(line0, comment);
        if(res->processLine(line, comment, lineNumber)) { ++lineNumber; }
    }
    res->correct();
    if(!res->validate())
    {
        delete res;
        res = nullptr;
    }
    return res;
}
