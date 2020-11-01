#ifndef NETAPI_H
#define NETAPI_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QList>
#include <QJsonDocument>
#include <Layout.h>


class NetAPI;


class RequestData
{
public:
    const QString &getRequestName() const;

protected:
    void setRequestName(const QString &requestName_);

private:
    QString m_requestName;

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
    RequestData *getList()
    {
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


private:
};

#endif // NETAPI_H
