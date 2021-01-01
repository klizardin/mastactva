#ifndef REQUESTDATA_H
#define REQUESTDATA_H


#include <QString>
#include <QVariant>
#include "../MastactvaBase/Layout.h"


class NetAPI;


class RequestData
{
public:
    RequestData() = default;
    virtual ~RequestData() = default;

    const QString &getRequestName() const;
    QVariant getItemId() const;
    QVariant getItemAppId() const;
    void allowModelProccessError();
    void setItemData(void *itemData_);
    void *getItemData();
    void setSetCurrentItemIndex(bool setCurrentItemIndex_);
    bool getSetCurrentItemIndex() const;

    template<class DataType_>
    static QString getListRequestName()
    {
        return getDataLayout<DataType_>().getLayoutJsonName() + "/GET";
    }

    template<class DataType_>
    static QString addItemRequestName()
    {
        return getDataLayout<DataType_>().getLayoutJsonName() + "/POST";
    }

    template<class DataType_>
    static QString setItemRequestName()
    {
        return getDataLayout<DataType_>().getLayoutJsonName() + "/PATCH";
    }

    template<class DataType_>
    static QString delItemRequestName()
    {
        return getDataLayout<DataType_>().getLayoutJsonName() + "/DELETE";
    }

protected:
    void setRequestName(const QString &requestName_);
    void setItemId(const QVariant &itemId_);
    void setItemAppId(const QVariant &itemAppId_);
    bool processErrorInNetAPI() const;

private:
    bool m_processErrorInNetAPI = true;
    QString m_requestName;
    QVariant m_itemId = QVariant::fromValue(-1);
    QVariant m_itemAppId = QVariant::fromValue(-1);
    void *m_itemData = nullptr;
    bool m_setCurrentItemIndex = false;

    friend class NetAPI;
};


#endif // REQUESTDATA_H
