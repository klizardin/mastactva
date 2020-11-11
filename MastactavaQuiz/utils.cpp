#include "utils.h"
#include <QStringList>

QString leftDoubleCR(const QString &str_)
{
    QString res(str_);
    QStringList frags = res.split("\n\n", Qt::SkipEmptyParts);
    for(QString &s: frags)
    {
        s.replace("\n", "");
    }
    return frags.join("\n\n");
}

qreal sqr(qreal val_)
{
    return val_*val_;
}
