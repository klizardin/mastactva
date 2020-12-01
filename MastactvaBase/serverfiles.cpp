#include "serverfiles.h"
#include <QFileInfo>
#include <QDir>
#include <QNetworkReply>
#include "../MastactvaBase/utils.h"
#include <numeric>


static const QString g_currentDir = "./";


ServerFiles::ServerFiles(QObject *parent_ /* = nullptr*/)
    : QObject(parent_)
{
    m_rootDir = g_currentDir;
}

ServerFiles::~ServerFiles()
{
    clearDownloads();
}

void ServerFiles::setRootDir(const QString &path_)
{
    m_rootDir = path_;
}

void ServerFiles::add(const QString &url_, const QString &hash_, const QString &relCachePath_)
{
    if(isUrlDownloaded(url_) && testHash(url_, hash_))
    {
        emit downloaded(url_);
        return;
    }
    ServerFileDownload *sfd = new ServerFileDownload(this);
    m_downloads.push_back(sfd);
    QObject::connect(sfd, SIGNAL(finished(ServerFileDownload *)), this, SLOT(finished(ServerFileDownload *)));
    QObject::connect(sfd, SIGNAL(progress()), this, SLOT(progressSlot()));
    sfd->setPath(m_rootDir, relCachePath_);
    sfd->setFile(url_, hash_);
    sfd->start(m_manager);
}

bool ServerFiles::isUrlDownloaded(const QString &url_) const
{
    return m_map.contains(url_);
}

QString ServerFiles::get(const QString &url_) const
{
    if(isUrlDownloaded(url_)) { return m_map.value(url_); }
    else { return url_; }
}

void ServerFiles::clean(const QDateTime &to_)
{
    Q_UNUSED(to_);
    // TODO: add implementation
}

qreal ServerFiles::getProgressRate(const QStringList &urls_) const
{
    const bool done = std::all_of(std::begin(m_downloads), std::end(m_downloads),
                                  [&urls_](const ServerFileDownload *download_)->bool
    {
        return nullptr == download_  ||
                (
                    nullptr != download_ &&
                    std::end(urls_) == std::find(std::begin(urls_), std::end(urls_), download_->getUrl())
                );
    });
    if(done) { return 1.0; }

    qint64 recieved = std::accumulate(std::begin(m_downloads), std::end(m_downloads), qint64(0),
                                      [&urls_](qint64 val_, const ServerFileDownload *download_)->qint64
    {
        return val_ +
            (
                (
                    nullptr != download_
                    && std::end(urls_) != std::find(std::begin(urls_), std::end(urls_), download_->getUrl())
                )
                ? download_->bytesReceived()
                : 0
            );
    });
    qint64 total = std::accumulate(std::begin(m_downloads), std::end(m_downloads), qint64(0),
                                      [&urls_](qint64 val_, const ServerFileDownload *download_)->qint64
    {
        return val_ +
            (
                (
                    nullptr != download_
                    && std::end(urls_) != std::find(std::begin(urls_), std::end(urls_), download_->getUrl())
                )
                ? download_->bytesTotal()
                : 0
             );
    });

    if(0 == total) { return 0.0; }
    return qreal((std::min(recieved, total) * 1000) / total) / qreal(1000.0);
}

void ServerFiles::finished(ServerFileDownload *download_)
{
    if(nullptr == download_) { return; }
    QObject::disconnect(download_, SIGNAL(finished(ServerFileDownload *)), this, SLOT(finished(ServerFileDownload *)));
    QObject::disconnect(download_, SIGNAL(progress()), this, SLOT(progressSlot()));

    const auto fitd = std::find(std::begin(m_downloads), std::end(m_downloads), download_);
    if(std::end(m_downloads) == fitd)
    {
        delete download_;
        download_ = nullptr;
        return;
    }
    m_downloads.erase(fitd);

    const QString url = download_->getUrl();

    const auto fitm = m_map.find(url);
    m_map.erase(fitm);

    const bool ok = download_->ok();
    if(ok) { m_map.insert(url, download_->getLocalURL()); }

    delete download_;
    download_ = nullptr;

    if(ok) { emit downloaded(url); }
}

void ServerFiles::progressSlot()
{
    emit progress();
}

bool ServerFiles::testHash(const QString &url_, const QString &hash_) const
{
    if(!isUrlDownloaded(url_)) { return false; }
    return calculateFileURLHash(get(url_)) == hash_;
}

void ServerFiles::clearDownloads()
{
    for(auto *&p: m_downloads)
    {
        delete p;
        p = nullptr;
    }
    m_downloads.clear();
}


ServerFileDownload::ServerFileDownload(QObject *parent_ /*= nullptr*/)
    :QObject(parent_)
{
}

void ServerFileDownload::setPath(const QString &rootDir, const QString &relPath_)
{
    m_rootDir = rootDir;
    m_relPath = relPath_;
}
void ServerFileDownload::setFile(const QString &url_, const QString &hash_)
{
    m_url = url_;
    m_hash = hash_;
}

void ServerFileDownload::start(QNetworkAccessManager &manager_)
{
    m_filename = getFilename();
    m_outputFile.setFileName(m_filename);
    if (!m_outputFile.open(QIODevice::WriteOnly))
    {
        m_outputFile.close();

        emit finished(this);
        return;
    }
    QUrl url(m_url);
    QNetworkRequest request(url);
    m_download = manager_.get(request);
    connect(m_download, &QNetworkReply::downloadProgress, this, &ServerFileDownload::downloadProgress);
    connect(m_download, &QNetworkReply::finished, this, &ServerFileDownload::downloadFinished);
    connect(m_download, &QNetworkReply::readyRead, this, &ServerFileDownload::downloadReadyRead);
}

bool ServerFileDownload::ok() const
{
    return calculateFileURLHash(getLocalURL()) == m_hash;
}

const QString &ServerFileDownload::getUrl() const
{
    return m_url;
}

QString ServerFileDownload::getLocalURL() const
{
    return QUrl::fromLocalFile(m_filename).toString();
}

void ServerFileDownload::downloadProgress(qint64 bytesReceived_, qint64 bytesTotal_)
{
    m_bytesReceived = bytesReceived_;
    m_bytesTotal = bytesTotal_;

    emit progress();
}

void ServerFileDownload::downloadFinished()
{
    m_outputFile.close();
    bool success = false;
    if (m_download->error())
    {
        m_outputFile.remove();
    }
    else
    {
        if (isHttpRedirect())
        {
            reportRedirect();
            m_outputFile.remove();
        }
        else
        {
            success = true;
        }
    }

    m_download->deleteLater();
    if(success)
    {
        emit finished(this);
    }
}

bool ServerFileDownload::isHttpRedirect() const
{
    const int statusCode = m_download->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    return statusCode == 301 || statusCode == 302 || statusCode == 303
           || statusCode == 305 || statusCode == 307 || statusCode == 308;
}

void ServerFileDownload::reportRedirect()
{
    int statusCode = m_download->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    QUrl requestUrl = m_download->request().url();
    qDebug() << "Request: " << requestUrl.toDisplayString()
             << " was redirected with code: " << statusCode;

    QVariant target = m_download->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (!target.isValid())
        return;
    QUrl redirectUrl = target.toUrl();
    if (redirectUrl.isRelative())
        redirectUrl = requestUrl.resolved(redirectUrl);
    qDebug() << "Redirected to: " << redirectUrl.toDisplayString();
}

void ServerFileDownload::downloadReadyRead()
{
    m_outputFile.write(m_download->readAll());
}

QString ServerFileDownload::getFilename()
{
    QUrl url(m_url);
    QString path = url.path();
    QString basename = QFileInfo(path).fileName();
    if(basename.isEmpty()) { return QString(); }
    QString savePath = QDir(m_rootDir).filePath(m_relPath);
    QDir dir(savePath);
    dir.remove(basename);
    return dir.filePath(basename);
}

qint64 ServerFileDownload::bytesReceived() const
{
    return m_bytesReceived;
}

qint64 ServerFileDownload::bytesTotal() const
{
    return m_bytesTotal;
}
