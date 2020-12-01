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
    bool ok() const;
    const QString &getUrl() const;
    QString getLocalURL() const;

protected:
    QString getFilename();
    bool isHttpRedirect() const;
    void reportRedirect();

private slots:
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void downloadFinished();
    void downloadReadyRead();

signals:
    void finished(ServerFileDownload *download_);

private:
    QString m_rootDir;
    QString m_relPath;
    QString m_url;
    QString m_hash;
    QString m_filename;
    QNetworkReply *m_download = nullptr;
    QFile m_outputFile;
};


class ServerFiles : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit ServerFiles(QObject *parent_ = nullptr);
    virtual ~ServerFiles() override;

    Q_INVOKABLE void setRootDir(const QString &path_);
    Q_INVOKABLE void add(const QString &url_, const QString &hash_, const QString &relCachePath_);
    Q_INVOKABLE bool isUrlDownloaded(const QString &url_) const;
    Q_INVOKABLE QString get(const QString &url_) const;
    Q_INVOKABLE void clean(const QDateTime &to_);

protected:
    bool testHash(const QString &url_, const QString &hash_) const;
    void clearDownloads();

protected slots:
    void finished(ServerFileDownload *download_);

signals:
    void downloaded(const QString &url_);

private:
    QString m_rootDir;
    QList<ServerFileDownload *> m_downloads;
    QHash<QString, QString> m_map;
    QNetworkAccessManager m_manager;
};


#endif // SERVERFILES_H
