#ifndef APPCONSTS_H
#define APPCONSTS_H

#include <QString>
#include <QVector>
#include <QAbstractListModel>
#include <QtQuick/QQuickItem>


class ServerURLsModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    enum RoleNames {
        TextRole = Qt::UserRole,
    };


public:
    ServerURLsModel(const QVector<QString> &data_);
    void setURLs(const QVector<QString> &data_);

    virtual int rowCount(const QModelIndex &parent_ = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index_, int role_ = Qt::DisplayRole) const override;

protected:
    virtual QHash<int, QByteArray> roleNames() const override;

private:
    QVector<QString> m_data;
    QHash<int, QByteArray> m_roleNames;
};


class AppConsts
{
protected:
    QVector<QString> m_serverUrls;
    ServerURLsModel *m_serverURLsModel = nullptr;

protected:
    void load();

public:
    AppConsts();
    virtual ~AppConsts();

    void addServerURL(const QString &serverURL_);
    ServerURLsModel *getServerURLsModel();

    static AppConsts *getInstance();
    static void freeInstance();
    void save() const;
};

#endif // APPCONSTS_H
