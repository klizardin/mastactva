#ifndef IMODEL_H
#define IMODEL_H


#include <QString>
#include <QVariant>
#include <QHash>

// TODO: review methods use, remove extra methods

class IListModel
{
public:
    virtual const QString &getLayoutName() const = 0;
    virtual QString getLayoutIdFiledName() const = 0;
    virtual QVariant getCurrentIndexAppId() const = 0;
    virtual bool getValuesForAppId(const QVariant &appId_, QHash<QString, QVariant> &values_) const = 0;
    virtual QVariant getIdFieldValueForAppId(const QVariant &appId_) const = 0;
    virtual QVariant getFieldValueForAppId(const QVariant &appId_, const QString &jsonFieldName) const = 0;
};


#endif // IMODEL_H
