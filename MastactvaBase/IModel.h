#ifndef IMODEL_H
#define IMODEL_H


#include <QString>
#include <QVariant>
#include <QHash>

// TODO: review methods use, remove extra methods

class IListModel
{
public:
    virtual QObject* getModel() const = 0;
    virtual void initResponse() = 0;
    virtual const QString &getQMLLayoutName() const = 0;
    virtual const QString &getJsonLayoutName() const = 0;
    virtual QString getLayoutIdFiledName() const = 0;
    virtual bool containsAppId(const QVariant &appId_) const = 0;
    virtual QVariant getCurrentIndexAppId() const = 0;
    virtual bool getValuesForAppId(const QVariant &appId_, QHash<QString, QVariant> &values_) const = 0;
    virtual QVariant getIdFieldValueForAppId(const QVariant &appId_) const = 0;
    virtual QVariant getFieldValueForAppId(const QVariant &appId_, const QString &jsonFieldName) const = 0;
};

class IListModelInfo
{
public:
    virtual void startLoadChildModel() = 0;
    virtual void endLoadChildModel() = 0;
    virtual bool isListLoadedImpl() const = 0;
    virtual void listLoadedVF() = 0;
    virtual void itemAddedVF() = 0;
    virtual void itemSetVF() = 0;
    virtual void itemDeletedVF() = 0;
    virtual void errorVF(int errorCode_, const QString &errorCodeStr_, const QJsonDocument &reply_) = 0;
};

Q_DECLARE_METATYPE(IListModelInfo*)

class IListModelInfoObjectImpl : public IListModelInfo
{
public:
    virtual void startLoadChildModel() override;
    virtual void endLoadChildModel() override;
    virtual bool isListLoadedImpl() const override;
    virtual void listLoadedVF() override;
    virtual void itemAddedVF() override;
    virtual void itemSetVF() override;
    virtual void itemDeletedVF() override;
    virtual void errorVF(int errorCode_, const QString &errorCodeStr_, const QJsonDocument &reply_) override;

private:
    int m_loadingChildenModels = 0;
};


#endif // IMODEL_H
