#ifndef APPCONSTS_H
#define APPCONSTS_H


#include <QString>
#include <QVector>
#include <QVector3D>
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
    int m_maxImagePoints = 64;
    QVector<QVector3D> m_colors;
    bool m_useColorsVoronoyDiagram = false;
    QString m_language;

protected:
    void set(const QString &line_);
    void load();

public:
    AppConsts();
    virtual ~AppConsts();

    void addServerURL(const QString &serverURL_);
    ServerURLsModel *getServerURLsModel();
    int getMaxImagePoints() const;
    const QVector<QVector3D> &getColors() const;
    bool useColorsVoronoyDiagram() const;
    void setUseColorsVoronoyDiagram(bool useColorsVoronoyDiagram_);
    void initColors();
    void switchLanguage(const QString &lang_);
    const QString &getLanguage() const;
    int getLanguageIndex() const;

    static AppConsts *getInstance();
    static void freeInstance();
    void save() const;
};


#endif // APPCONSTS_H
