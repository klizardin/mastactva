/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

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
    QString m_lastLogin;
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
    void setLastLogin(const QString &lastLogin_);
    QString getLastLogin() const;
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
