#ifndef IMODEL_H
#define IMODEL_H


#include <QString>
#include <QVariant>
#include <QHash>


class IListModel
{
public:
    virtual const QString &getLayoutName() const = 0;
    virtual QString getLayoutIdFiledName() const = 0;
    virtual int getCurrentIndex() const = 0;
    virtual bool getCurrentIndexValues(QHash<QString, QVariant> &values_) const = 0;
    virtual QVariant getCurrentIndexIdFieldValue() const = 0;
    virtual QVariant getCurrentIndexFieldValue(const QString &jsonFieldName) const = 0;
};


#endif // IMODEL_H
