#include "utils_comment.h"
#include "../MastactvaBase/names.h"
#include "utils.h"


static const char *g_nl = "\n";
static const char *g_cb = "/*";
static const char *g_ce = "*/";
static const char *g_luaCb = "--[[";
static const char *g_luaCe = "--]]";
static const char *g_leftAlign = "<";
static const char *g_nameStart = "@";


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

bool isSpaces(const QString &str_, int i0_, int i1_)
{
    for(int i = i0_; i < i1_; i++)
    {
        if(!isSpace(str_.at(i))) { return false; }
    }
    return true;
}


Comment::Comment(int cb_ /*= -1*/, int ce_ /*= -1*/,int lb_ /*= -1*/, int le_ /*= -1*/)
    :cb(cb_), ce(ce_), lb(lb_), le(le_)
{}

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

