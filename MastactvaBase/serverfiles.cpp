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

#include "serverfiles.h"
#include <QFileInfo>
#include <QDir>
#include <QDirIterator>
#include <QNetworkReply>
#include <numeric>
#include "../MastactvaBase/utils.h"
#include "../MastactvaBase/names.h"
#include "../MastactvaBase/defines.h"


ServerFiles::ServerFiles(QObject *parent_ /* = nullptr*/)
    : QObject(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "ServerFiles::ServerFiles()" << QThread::currentThread() << QThread::currentThreadId();
#endif

#if defined(ASSETS_ROOT)
    m_rootDir = g_assestsDir;
#else
    m_rootDir = g_currentDir;
#endif
#if defined(TRACE_SERVER_FILES)
    qDebug() << "m_rootDir = " << m_rootDir;
#endif
}

ServerFiles::~ServerFiles()
{
    clearDownloads();
}

QString ServerFiles::getRootDir() const
{
    return m_rootDir;
}

void ServerFiles::setRootDir(const QString &path_)
{
#if defined(ASSETS_ROOT)
    Q_UNUSED(path_);
    // work only with assets path
#else
    m_rootDir = path_;
#endif
#if defined(TRACE_SERVER_FILES)
    qDebug() << "m_rootDir = " << m_rootDir;
#endif
}

void ServerFiles::add(
        const QString &url_,
        const QString &hash_,
        const QString &relCachePath_,
        const QString &namespacePath_
        )
{
#if defined(TRACE_SERVER_FILES)
    qDebug() << "ServerFiles::add() url = " << url_;
#endif

    // check if url has currently been downloaded
    if(isUrlDownloaded(url_) && (isResourceSheme(url_) || testHash(url_, hash_)))
    {
#if defined(TRACE_SERVER_FILES)
        qDebug() << "ServerFiles::add() already has been downloaded " << url_;
#endif
        emit downloaded(url_);
        return;
    }

    if(isResourceSheme(url_))
    {
        addResourceUrl(url_);
#if defined(TRACE_SERVER_FILES)
    qDebug() << "ServerFiles::add() resource url = " << url_;
#endif
        emit downloaded(url_);
        return;
    }

    // check if url is currently downloading
    const auto fitd = std::find_if(std::begin(m_downloads), std::end(m_downloads),
                 [&url_](ServerFileDownload *sfd_)->bool
    {
        return sfd_ && sfd_->getUrl() == url_;
    });
    if(std::end(m_downloads) != fitd)
    {
        ServerFileDownload *sfd = *fitd;
        if(sfd->getHash() != hash_)
        {
            // update hash
            // prefere new hash
            // it is just a heuristic, really this hash may not be the latest
            sfd->setFile(url_, hash_);
        }
#if defined(TRACE_SERVER_FILES)
        qDebug() << "ServerFiles::add() is downloading now " << url_;
#endif
        return;
    }

    // start new download
#if defined(TRACE_SERVER_FILES)
    qDebug() << "ServerFiles::add() start to download new url " << url_;
#endif

    ServerFileDownload *sfd = new ServerFileDownload(this);
    m_downloads.push_back(sfd);
    QObject::connect(sfd, SIGNAL(finished(ServerFileDownload *)),
                     this, SLOT(finished(ServerFileDownload *)));
    QObject::connect(sfd, SIGNAL(progress()),
                     this, SLOT(progressSlot()));
    sfd->setPath(m_rootDir, relCachePath_, namespacePath_);
    sfd->setFile(url_, hash_);

    sfd->start(m_manager);
}

bool ServerFiles::remove(const QString &url_)
{
    if(!isUrlDownloaded(url_))
    {
        return false;
    }

    QFile::remove(m_map.value(url_));
    m_map.remove(url_);
    return true;
}

bool ServerFiles::removeLocalFile(const QString &filename_)
{
    QFileInfo fi(filename_);
    const QList<QString> keys = m_map.keys();
    for(const QString &key_ : keys)
    {
        QFileInfo fik(m_map.value(key_));
        if(fik.absoluteFilePath() == fi.absoluteFilePath())
        {
            QFile::remove(fi.absoluteFilePath());
            m_map.remove(key_);
            return true;
        }
    }
    return false;
}

bool ServerFiles::isUrlDownloaded(const QString &url_) const
{
    const bool res = m_map.contains(url_);
    return res;
}

QString ServerFiles::get(const QString &url_) const
{
    if(isUrlDownloaded(url_)) { return m_map.value(url_); }
    else { return url_; }
}

void ServerFiles::clean(const QDateTime &beforeEqualDate_)
{
    clearDownloads();
    QDirIterator fit(m_rootDir, QStringList() << "*.*", QDir::NoFilter, QDirIterator::Subdirectories);
    while(fit.hasNext())
    {
        QFile f(fit.next());
        QFileInfo fi(f);
        if(fi.lastModified() <= beforeEqualDate_)
        {
            if(!removeLocalFile(fi.absoluteFilePath()))
            {
                f.remove();
            }
        }
    }
}

qreal ServerFiles::getProgressRate(const QStringList &urls_) const
{
    const bool done = std::all_of(std::begin(m_downloads), std::end(m_downloads),
                                  [&urls_](const ServerFileDownload *download_)->bool
    {
        return !download_  ||
                (
                    download_ &&
                    std::end(urls_)
                        == std::find(
                            std::begin(urls_),
                            std::end(urls_),
                            download_->getUrl()
                            )
                );
    });
    if(done) { return 1.0; }

    qint64 recieved = std::accumulate(std::begin(m_downloads),
                                      std::end(m_downloads), qint64(0),
                                      [&urls_](qint64 val_, const ServerFileDownload *download_)->qint64
    {
        return val_ +
            (
                (
                    download_
                    && std::end(urls_)
                        != std::find(
                                std::begin(urls_),
                                std::end(urls_),
                                download_->getUrl()
                                )
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
                    download_
                    && std::end(urls_)
                        != std::find(
                                std::begin(urls_),
                                std::end(urls_),
                                download_->getUrl()
                                )
                )
                ? download_->bytesTotal()
                : 0
             );
    });

    if(0 == total) { return 0.0; }
    return qreal((std::min(recieved, total) * 1000) / total) / qreal(1000.0);
}

void ServerFiles::cancel(const QStringList &urls_)
{
    QList<ServerFileDownload *> toRemove;
    for(ServerFileDownload *download: qAsConst(m_downloads))
    {
        if(!download) { continue; }
        const auto fit = std::find(std::begin(urls_), std::end(urls_), download->getUrl());
        if(std::end(urls_) == fit) { continue; }
        download->cancel();
        toRemove.push_back(download);
    }
    for(ServerFileDownload *download: qAsConst(toRemove))
    {
        QObject::disconnect(download, SIGNAL(finished(ServerFileDownload *)),
                            this, SLOT(finished(ServerFileDownload *)));
        QObject::disconnect(download, SIGNAL(progress()),
                            this, SLOT(progressSlot()));
        auto fit = std::find(std::begin(m_downloads), std::end(m_downloads), download);
        delete *fit;
        *fit = nullptr;
        m_downloads.erase(fit);
    }
}

void ServerFiles::reset()
{
    QList<ServerFileDownload *> toRemove;
    for(ServerFileDownload *download: qAsConst(m_downloads))
    {
        if(!download) { continue; }
        toRemove.push_back(download);
    }
    for(ServerFileDownload *download: qAsConst(toRemove))
    {
        QObject::disconnect(download, SIGNAL(finished(ServerFileDownload *)),
                            this, SLOT(finished(ServerFileDownload *)));
        QObject::disconnect(download, SIGNAL(progress()),
                            this, SLOT(progressSlot()));
        auto fit = std::find(std::begin(m_downloads), std::end(m_downloads), download);
        delete *fit;
        *fit = nullptr;
        m_downloads.erase(fit);
    }
    m_map.clear();
}

void ServerFiles::finished(ServerFileDownload *download_)
{
    if(!download_) { return; }
    QObject::disconnect(download_, SIGNAL(finished(ServerFileDownload *)),
                        this, SLOT(finished(ServerFileDownload *)));
    QObject::disconnect(download_, SIGNAL(progress()),
                        this, SLOT(progressSlot()));

    const auto fitd = std::find(std::begin(m_downloads),
                                std::end(m_downloads),
                                download_);
    if(std::end(m_downloads) == fitd)
    {
        delete download_;
        download_ = nullptr;
        return;
    }
    m_downloads.erase(fitd);

    const QString url = download_->getUrl();

    const auto fitm = m_map.find(url);
    const QString oldUrl = std::end(m_map) != fitm ? fitm.value() : QString();
    m_map.erase(fitm);

    const bool ok = download_->ok();
    if(ok) { m_map.insert(url, download_->getLocalURL()); }
    download_->removeOldFile(oldUrl);

    delete download_;
    download_ = nullptr;

    if(ok)
    {
#if defined(TRACE_SERVER_FILES)
        qDebug() << "ServerFiles::finished() ok " << url;
#endif
        emit downloaded(url);
    }
    else
    {
#if defined(TRACE_SERVER_FILES)
        qDebug() << "ServerFiles::finished() failed " << url;
#endif
    }
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

bool ServerFiles::isResourceSheme(const QString &url_)
{
    QUrl url(url_);
    return url.scheme() == "qrc";
}

void ServerFiles::addResourceUrl(const QString &url_)
{
    const auto fitm = m_map.find(url_);
    const QString oldUrl = std::end(m_map) != fitm ? fitm.value() : QString();
    m_map.erase(fitm);

    m_map.insert(url_, url_);
}


ServerFileDownload::ServerFileDownload(QObject *parent_ /*= nullptr*/)
    :QObject(parent_)
{
}

void ServerFileDownload::setPath(
        const QString &rootDir,
        const QString &relPath_,
        const QString &namespacePath_
        )
{
    m_rootDir = rootDir;
    m_relPath = subpathJoin(namespacePath_, relPath_);
}

void ServerFileDownload::setFile(const QString &url_, const QString &hash_)
{
    m_url = url_;
    m_hash = hash_;
}

void ServerFileDownload::start(QNetworkAccessManager &manager_)
{
    //qDebug() << "ServerFileDownload::start() url = " << m_url;
    bool exists = false;
    m_filename = getFilename(exists);
    if(exists)
    {
        emit finished(this);
        return;
    }
    m_outputFile.setFileName(m_filename);
    //qDebug() << "m_filename = " << m_filename;
    if (!m_outputFile.open(QIODevice::WriteOnly))
    {
        //qDebug() << "!m_outputFile.open(QIODevice::WriteOnly)";
        m_outputFile.close();

        emit finished(this);
        return;
    }
    QUrl url(m_url);
    QNetworkRequest request(url);
    m_download = manager_.get(request);
    connect(m_download, &QNetworkReply::downloadProgress,
            this, &ServerFileDownload::downloadProgress);
    connect(m_download, &QNetworkReply::finished,
            this, &ServerFileDownload::downloadFinished);
    connect(m_download, &QNetworkReply::readyRead,
            this, &ServerFileDownload::downloadReadyRead);
}

void ServerFileDownload::cancel()
{
    //qDebug() << "ServerFileDownload::cancel() url = " << m_url;

    disconnect(m_download, &QNetworkReply::downloadProgress,
               this, &ServerFileDownload::downloadProgress);
    disconnect(m_download, &QNetworkReply::finished,
               this, &ServerFileDownload::downloadFinished);
    disconnect(m_download, &QNetworkReply::readyRead,
               this, &ServerFileDownload::downloadReadyRead);
    m_outputFile.close();
    if(!ok())
    {
        m_outputFile.remove();
    }
}

bool ServerFileDownload::ok() const
{
    return calculateFileURLHash(getLocalURL()) == m_hash;
}

const QString &ServerFileDownload::getUrl() const
{
    return m_url;
}

const QString &ServerFileDownload::getHash() const
{
    return m_hash;
}

QString ServerFileDownload::getLocalURL() const
{
    return QUrl::fromLocalFile(m_filename).toString();
}

void ServerFileDownload::downloadProgress(qint64 bytesReceived_, qint64 bytesTotal_)
{
    //qDebug() << "ServerFileDownload::downloadProgress() url = " << m_url << " bytesReceived_ = " << bytesReceived_ << " bytesTotal_ = " << bytesTotal_;

    m_bytesReceived = bytesReceived_;
    m_bytesTotal = bytesTotal_;

    emit progress();
}

void ServerFileDownload::downloadFinished()
{
    //qDebug() << "ServerFileDownload::downloadProgress() url = " << m_url;

    m_outputFile.close();
    disconnect(m_download, &QNetworkReply::downloadProgress,
               this, &ServerFileDownload::downloadProgress);
    disconnect(m_download, &QNetworkReply::finished,
               this, &ServerFileDownload::downloadFinished);
    disconnect(m_download, &QNetworkReply::readyRead,
               this, &ServerFileDownload::downloadReadyRead);
    bool success = false;
    if (m_download->error())
    {
        qDebug() << "download error() = " << m_download->error();
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
    if (!target.isValid()) { return; }
    QUrl redirectUrl = target.toUrl();
    if (redirectUrl.isRelative()) { redirectUrl = requestUrl.resolved(redirectUrl); }
    qDebug() << "Redirected to: " << redirectUrl.toDisplayString();
}

void ServerFileDownload::downloadReadyRead()
{
    //qDebug() << "ServerFileDownload::downloadReadyRead() url = " << m_url;

    m_outputFile.write(m_download->readAll());
}

QString ServerFileDownload::getFilename(bool &exists_)
{
    exists_ = false;
    m_oldName = QString();
    QUrl url(m_url);
    QString path = url.path();
    QFileInfo fi0(path);
    QString basename = fi0.fileName();
    QString ext = fi0.suffix();
    if(basename.isEmpty()) { return QString(); }
    QString savePath = QDir(m_rootDir).filePath(m_relPath);
    QDir dir(savePath);
    if(!dir.exists()) { dir.mkpath("."); }
    if(QFile::exists(dir.filePath(basename)))
    {
        QFileInfo fi1(dir.filePath(basename));
        const QString existingFilename = fi1.absoluteFilePath();
        m_oldName = existingFilename;
        const QString url = QUrl::fromLocalFile(existingFilename).toString();
        if(calculateFileURLHash(url) == m_hash)
        {
            exists_ = true;
            return existingFilename;
        }
        if(ext.isEmpty())
        {
            basename += ".";
        }
        else
        {
            basename = basename.mid(0, basename.length() - (ext.length() + 1)) + ".";
            ext = QString(".") + ext;
        }
        int i = 0;
        for(; QFile::exists(dir.filePath(basename + QString::number(i) + ext)); i++)
        {
            const QString filename = basename + QString::number(i) + ext;
            const QString url = QUrl::fromLocalFile(filename).toString();
            if(calculateFileURLHash(url) == m_hash)
            {
                exists_ = true;
                break;
            }
        }
        basename = basename + QString::number(i) + ext;
    }
    QFileInfo fi2(dir.filePath(basename));
    return fi2.absoluteFilePath();
}

qint64 ServerFileDownload::bytesReceived() const
{
    return m_bytesReceived;
}

qint64 ServerFileDownload::bytesTotal() const
{
    return m_bytesTotal;
}

const QString &ServerFileDownload::getOldName() const
{
    return m_oldName;
}

void ServerFileDownload::removeOldFile(const QString &oldURL_)
{
    if(!oldURL_.isEmpty() &&
            (m_oldName.isEmpty() ||
             QUrl::fromLocalFile(m_oldName).toString() != oldURL_)
            ) { return; }

    if(m_oldName != m_filename && !m_oldName.isEmpty())
    {
        QFile(m_oldName).remove();
    }
    QFileInfo fi(m_oldName);
    QString dir = fi.path();
    QString basename = fi.fileName();
    QString ext = fi.suffix();
    if(ext.isEmpty())
    {
        basename += ".";
    }
    else
    {
        basename = basename.mid(0, basename.length() - (ext.length() + 1)) + ".";
        ext = QString(".") + ext;
    }
    QDir d(dir);
    for(int i = 0; QFile::exists(d.filePath(basename + QString::number(i) + ext)); i++)
    {
        const QString filename = d.filePath(basename + QString::number(i) + ext);
        if(filename != m_filename)
        {
            QFile(filename).remove();
        }
    }
    m_oldName.clear();
}
