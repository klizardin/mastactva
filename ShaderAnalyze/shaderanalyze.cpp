#include "shaderanalyze.h"
#include <QTextCodec>


ShaderAnalyze::ShaderAnalyze(QObject *parent) : QObject(parent)
{
}


static const QString g_nl = "\n";
static const QString g_cb = "/*";
static const QString g_ce = "*/";
static const QString g_leftAlign = "<";


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

class Comment
{
public:
    Comment(int cb_ = -1, int ce_ = -1,int lb_ = -1, int le_ = -1);
    bool isAlignedToLeft(const QString &shaderText_) const;
    void findLeftLine(const QVector<int> &indexesOfNL_, const QString &shaderText_);
    void findRightLine(const QVector<int> &indexesOfNL_, const QString &shaderText_);

    int cb = 0;
    int ce = 0;
    int lb = 0;
    int le = 0;
};

Comment::Comment(int cb_ /*= -1*/, int ce_ /*= -1*/,int lb_ /*= -1*/, int le_ /*= -1*/)
    :cb(cb_), ce(ce_), lb(lb_), le(le_)
{}

bool isSymbol(const QChar &ch_)
{
    return ch_.category() >= QChar::Letter_Uppercase;
}

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
    if(i0 < 0 || i0 >= cb) { return false; }
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
        int te = indexesOfNL_[i0] + g_nl.length();              // test line end
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

void ShaderAnalyze::process(const QByteArray &data_)
{
    QTextCodec *codec = QTextCodec::codecForUtfText(data_);
    QString shaderText = codec->toUnicode(data_);

    // 1. find all \n in shader text
    QVector<int> indexesOfNL;
    int p = 0; int cnt = 0;
    for(p = shaderText.indexOf(g_nl); p >= 0; p = shaderText.indexOf(g_nl, p)) { cnt++; }
    indexesOfNL.reserve(cnt);
    for(p = shaderText.indexOf(g_nl); p >= 0; p = shaderText.indexOf(g_nl, p)) { indexesOfNL.push_back(p); }

    QVector<Comment> comments;
    for(cnt = 0, p = 0; p >= 0;)
    {
        p = shaderText.indexOf(g_cb, p);
        if(p < 0) { break; }
        p = shaderText.indexOf(g_ce, p);
        if(p < 0) { break; }
        cnt++;
    }
    comments.reserve(cnt);
    for(p = 0; p >= 0;)
    {
        p = shaderText.indexOf(g_cb, p);
        if(p < 0) { break; }
        const int cb = p + g_cb.length();
        p = shaderText.indexOf(g_ce, p);
        if(p < 0) { break; }
        const int ce = p;
        comments.push_back(Comment(cb, ce));
    }

    for(Comment &comment : comments)
    {
        if(comment.isAlignedToLeft(shaderText))
        {
            comment.findLeftLine(indexesOfNL, shaderText);
        }
        else
        {
            comment.findRightLine(indexesOfNL, shaderText);
        }
    }
}
