#ifndef CONTAINERUTILS_H
#define CONTAINERUTILS_H


#include <QString>
#include <QHash>
#include <QVariant>


QHash<QString, QVariant> mergePrefareFirst(
        const QHash<QString, QVariant> &v1_,
        const QHash<QString, QVariant> &v2_
        );


#endif // CONTAINERUTILS_H
