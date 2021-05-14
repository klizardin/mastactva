#include "containerutils.h"


QHash<QString, QVariant> mergePrefareFirst(
        const QHash<QString, QVariant> &v1_,
        const QHash<QString, QVariant> &v2_
        )
{
    QHash<QString, QVariant> res;
    for(QHash<QString, QVariant>::const_iterator it = std::cbegin(qAsConst(v2_));
        it != std::cend(qAsConst(v2_));
        ++it)
    {
        if(v1_.contains(it.key()))
        {
            res.insert(it.key(), v1_.value(it.key())); // for common prefere v1_
        }
        else
        {
            res.insert(it.key(), it.value());
        }
    }
    for(QHash<QString, QVariant>::const_iterator it = std::cbegin(qAsConst(v1_));
        it != std::cend(qAsConst(v1_));
        ++it)
    {
        if(!v2_.contains(it.key()))
        {
            res.insert(it.key(), it.value());
        }
    }
    return res;
}
