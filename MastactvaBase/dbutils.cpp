#include "dbutils.h"
#include "../MastactvaBase/names.h"


QString refName(const QString &ref_)
{
    return QString(g_refPrefix) + ref_;
}

QStringList refsNames(const QStringList &refs_)
{
    QStringList res;
    for(const auto &s : qAsConst(refs_))
    {
        res.push_back(refName(s));
    }
    return res;
}

QString toBindName(const QString &sqlName_)
{
    if(sqlName_.at(0)==QChar('"'))
    {
        return QString(":") + sqlName_.mid(1, sqlName_.length() - 2);
    }
    else
    {
        return QString(":") + sqlName_;
    }
}

QStringList equalToValueConditionsFromSqlNamesFromSqlNames(const QStringList &names_)
{
    QStringList res;
    for(const QString &sqlName : qAsConst(names_))
    {
        const QString ref = refName(sqlName);
        res.push_back(QString("%1=%2").arg(ref, toBindName(ref)));
    }
    return res;
}

QStringList filterNames(const QStringList &sqlNames_, const QList<QVariant> &leftNames_)
{
    if(leftNames_.isEmpty()) { return sqlNames_; }
    QStringList res;
    for(const QString &name: qAsConst(sqlNames_))
    {
        const auto fit = std::find_if(std::cbegin(leftNames_), std::cend(leftNames_),
                                      [&name](const QVariant &val)->bool
        {
           return val.isValid() && val.toString() == name;
        });
        if(std::cend(leftNames_) != fit)
        {
            res.push_back(name);
        }
    }
    return res;
}

QStringList applyFunction(const QStringList &sqlNames_, const QString &function_)
{
    if(function_.isEmpty()) { return sqlNames_; }
    QStringList res;
    for(const QString &name: qAsConst(sqlNames_))
    {
        res.push_back(QString("%1(%2)").arg(function_, name));
    }
    return res;
}

