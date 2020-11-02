#ifndef QMLOBJECTS_H
#define QMLOBJECTS_H

#include <QVector>


class QObject;
class IListModel;
class NetAPI;


class QMLObjects
{
protected:
    QMLObjects() = default;

public:
    static QMLObjects &getInstance();

    void setRoot(QObject *root_);
    NetAPI *getNetAPI();
    IListModel *getListModel(const QString &layoutName_);

protected:
    void searchObjects();
    IListModel *findListModel(const QString &layoutName_) const;

private:
    QObject *m_root = nullptr;
    QVector<IListModel *> m_models;
    NetAPI *m_netAPI = nullptr;
};

#endif // QMLOBJECTS_H
