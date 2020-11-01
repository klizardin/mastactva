#ifndef NETAPI_H
#define NETAPI_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QList>
#include <QJsonDocument>
#include <Layout.h>
#include "qmlobjects.h"
#include "IModel.h"


class NetAPI;


class RequestData
{
public:
    const QString &getRequestName() const;
    int getItemId() const;
    int getItemIndex() const;

protected:
    void setRequestName(const QString &requestName_);
    void setItenId(int itemId_);
    void setItenIndex(int itemIndex_);

private:
    QString m_requestName;
    int m_itemId = -1;          // TODO: may be better to use QVariant
    int m_itemIndex = -1;

    friend class NetAPI;
};


class NetAPI : public QObject
{
    Q_OBJECT
public:
    NetAPI(QObject *parent_ = nullptr);

    template<class DataType_>
    QString getListRequestName() const
    {
        return getDataLayout<DataType_>().getLayoutJsonName() + "/GET";
    }

    template<class DataType_>
    QString addItemRequestName() const
    {
        return getDataLayout<DataType_>().getLayoutJsonName() + "/POST";
    }

    template<class DataType_>
    QString setItemRequestName() const
    {
        return getDataLayout<DataType_>().getLayoutJsonName() + "/PATCH";
    }

    template<class DataType_>
    RequestData *getList(const QString &currentRef_)
    {
        QString parentModel;
        QString parentModelJsonFieldName;
        getDataLayout<DataType_>().getRef(currentRef_, parentModel, parentModelJsonFieldName);
        IListModel *parentModelPtr = QMLObjects::getInstance().getListModel(parentModel);
        if(nullptr != parentModelPtr)
        {
            const QVariant idField = parentModelJsonFieldName.isEmpty()
                    ? parentModelPtr->getCurrentIndexIdFieldValue()
                    : parentModelPtr->getCurrentIndexFieldValue(parentModelJsonFieldName)
                    ;
            return getListByRefImpl(getDataLayout<DataType_>().getLayoutJsonName(), currentRef_, idField);
        }
        else
        {
            return getListImpl(getDataLayout<DataType_>().getLayoutJsonName());
        }
    }

    template<class DataType_>
    RequestData *addItem(const DataType_ *item_)
    {
    }

    template<class DataType_>
    RequestData *setItem(const DataType_ *item_)
    {
    }

signals:
    void onResponse(int errorCode_, RequestData *request_, const QJsonDocument &reply_);

protected:
    RequestData *getListByRefImpl(const QString &jsonLayoutName, const QString &ref_, const QVariant &id_);
    RequestData *getListImpl(const QString &jsonLayoutName);


private:
};

#endif // NETAPI_H
