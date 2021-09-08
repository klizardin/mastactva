/*
    Copyright 2021

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

#ifndef SERVERFILES_H
#define SERVERFILES_H


#include <QObject>
#include <QHash>
#include <QList>
#include <QtQuick/QQuickItem>
#include <QNetworkAccessManager>


class ServerFileDownload : public QObject
{
    Q_OBJECT
public:
    explicit ServerFileDownload(QObject *parent_ = nullptr);

    void setPath(const QString &rootDir_, const QString &relPath_);
    void setFile(const QString &url_, const QString &hash_);
    void start(QNetworkAccessManager &manager_);
    void cancel();
    bool ok() const;
    const QString &getUrl() const;
    const QString &getHash() const;
    QString getLocalURL() const;
    qint64 bytesReceived() const;
    qint64 bytesTotal() const;
    const QString &getOldName() const;
    void removeOldFile(const QString &oldURL_);

protected:
    QString getFilename(bool &exists_);
    bool isHttpRedirect() const;
    void reportRedirect();

private slots:
    void downloadProgress(qint64 bytesReceived_, qint64 bytesTotal_);
    void downloadFinished();
    void downloadReadyRead();

signals:
    void finished(ServerFileDownload *download_);
    void progress();

private:
    QString m_rootDir;
    QString m_relPath;
    QString m_url;
    QString m_hash;
    QString m_filename;
    QString m_oldName;
    QNetworkReply *m_download = nullptr;
    QFile m_outputFile;
    qint64 m_bytesReceived = 0;
    qint64 m_bytesTotal = 0;
};


class ServerFiles : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit ServerFiles(QObject *parent_ = nullptr);
    virtual ~ServerFiles() override;

    Q_INVOKABLE QString getRootDir() const;
    Q_INVOKABLE void setRootDir(const QString &path_);
    Q_INVOKABLE void add(const QString &url_,
                         const QString &hash_,
                         const QString &relCachePath_);
    Q_INVOKABLE bool isUrlDownloaded(const QString &url_) const;
    Q_INVOKABLE QString get(const QString &url_) const;
    Q_INVOKABLE void clean(const QDateTime &beforeEqualDate_);
    Q_INVOKABLE qreal getProgressRate(const QStringList &urls_) const;
    Q_INVOKABLE void cancel(const QStringList &urls_);
    Q_INVOKABLE void reset();
    Q_INVOKABLE void remove(const QString &url_);

protected:
    bool testHash(const QString &url_, const QString &hash_) const;
    void clearDownloads();
    bool isResourceSheme(const QString &url_);
    void addResourceUrl(const QString &url_);

protected slots:
    void finished(ServerFileDownload *download_);
    void progressSlot();

signals:
    void downloaded(const QString &url_);
    void progress();

private:
    QString m_rootDir;
    QList<ServerFileDownload *> m_downloads;
    QHash<QString, QString> m_map;
    QNetworkAccessManager m_manager;
};


#endif // SERVERFILES_H
