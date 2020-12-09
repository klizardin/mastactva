#ifndef QMLOBJECTS_H
#define QMLOBJECTS_H

#include <QVector>
#include "../MastactvaBase/IModel.h"


class QObject;
class NetAPI;
class ServerFiles;


class QMLObjectsBase
{
protected:
    QMLObjectsBase() = default;

public:
    static QMLObjectsBase &getInstance();

    void setRoot(QObject *root_);
    NetAPI *getNetAPI();
    ServerFiles *getServerFiles();
    IListModel *getListModel(const QString &layoutName_);
    void registerModel(const QString &layoutName_, IListModel *m_);
    void unregisterModel(const QString &layoutName_);

    virtual void setInitialized() = 0;
    virtual void modelError(int errorCode_, const QString &errorCodeStr_, const QJsonDocument &reply_) = 0;

protected:
    virtual void searchObjects() = 0;
    IListModel *findListModel(const QString &layoutName_) const;
    virtual void clearDependedFromRoot();

protected:
    QObject *m_root = nullptr;
    QVector<IListModel *> m_models;
    NetAPI *m_netAPI = nullptr;
    ServerFiles *m_serverFiles = nullptr;
};


#endif // QMLOBJECTS_H
