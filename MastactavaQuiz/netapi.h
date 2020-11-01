#ifndef NETAPI_H
#define NETAPI_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QList>
#include <QJsonDocument>
#include <Layout.h>


class RequestData
{
};


class NetAPI : public QObject
{
    Q_OBJECT
public:
    NetAPI(QObject *parent_ = nullptr);

    template<typename DataType_>
    RequestData *getList()
    {
    }

signals:
    void onResponse(int errorCode_,RequestData *request, const QJsonDocument &reply_);


private:
};

#endif // NETAPI_H
