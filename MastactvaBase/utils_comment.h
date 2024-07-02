#ifndef UTILS_COMMENT_H
#define UTILS_COMMENT_H


#include <QString>
#include <QVector>
#include <QHash>

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


void getShaderComments(const QString &shaderText_, QVector<Comment> &comments_);    // return shader's comments
void getLuaComments(const QString &shaderText_, QVector<Comment> &comments_);       // return lua's comments


#endif // UTILS_COMMENT_H
