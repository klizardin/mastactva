#include "utils.h"
#include <QStringList>

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

bool isSymbol(const QChar &ch_)
{
    return ch_.category() >= QChar::Letter_Uppercase;
}

static const QString g_nl = "\n";
static const QString g_cb = "/*";
static const QString g_ce = "*/";
static const QString g_leftAlign = "<";
static const QString g_nameStart = "@";
static const QString g_argStart = "uniform";
static const QString g_argPrecisionLow = "lowp";
static const QString g_argPrecisionMedium = "mediump";
static const QString g_argPrecisionHigh = "highp";


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
        if(isSymbol(str_.at(i))) { return false; }
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
        te = cb - g_cb.length();                        // test line before comment
        ;
        te = indexesOfNL_[i0],                          // test line before nl
        i0--                                            // test prev line
        )
    {
        int tb = indexesOfNL_[i0] + g_nl.length();      // test line begining, skip nl
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
    for(int i0 = high_bound(indexesOfNL_, ce + g_ce.length()),  // start index on nl
        tb = ce + g_cb.length();                                // test line after comment
        ;
        tb = indexesOfNL_[i0] + g_nl.length(),                  // test line after nl
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
        i += g_nameStart.length();
        int nb = i;
        for(;i < ce && isSymbol(shaderText_.at(i)); ++i) {}; // find name
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
    if(!line.startsWith(g_argStart)) { return; }
    line = line.mid(g_argStart.length()).trimmed();
    if(line.startsWith(g_argPrecisionHigh))
    {
        line = line.mid(g_argPrecisionHigh.length()).trimmed();
    }
    else if(line.startsWith(g_argPrecisionMedium))
    {
        line = line.mid(g_argPrecisionMedium.length()).trimmed();
    }
    else if(line.startsWith(g_argPrecisionLow))
    {
        line = line.mid(g_argPrecisionLow.length()).trimmed();
    }
    int i = 0;
    int tb = i;
    for(;isSymbol(line.at(i)); i++) {}
    int te = i + 1;
    for(;!isSymbol(line.at(i)); i++) {}
    int nb = i;
    for(;isSymbol(line.at(i)); i++) {}
    int ne = i + 1;
    m_values.insert(g_typeName, line.mid(tb, te - tb));
    m_values.insert(g_nameName, line.mid(nb, ne - nb));
}

void getShaderComments(const QString &shaderText_, QVector<Comment> &comments_)
{
    comments_.clear();
    QVector<int> indexesOfNL;
    int p = 0; int cnt = 0;
    for(p = shaderText_.indexOf(g_nl); p >= 0; p += g_nl.length(), p = shaderText_.indexOf(g_nl, p))
    {
        cnt++;
    }
    indexesOfNL.reserve(cnt);
    for(p = shaderText_.indexOf(g_nl); p >= 0; p += g_nl.length(), p = shaderText_.indexOf(g_nl, p))
    {
        indexesOfNL.push_back(p);
    }

    for(cnt = 0, p = 0; p >= 0;)
    {
        p = shaderText_.indexOf(g_cb, p);
        if(p < 0) { break; }
        p += g_cb.length();
        p = shaderText_.indexOf(g_ce, p);
        if(p < 0) { break; }
        p += g_ce.length();
        cnt++;
    }
    comments_.reserve(cnt);
    for(p = 0; p >= 0;)
    {
        p = shaderText_.indexOf(g_cb, p);
        if(p < 0) { break; }
        const int cb = p + g_cb.length();
        p += g_cb.length();
        p = shaderText_.indexOf(g_ce, p);
        if(p < 0) { break; }
        const int ce = p;
        p += g_ce.length();
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
    }
}
