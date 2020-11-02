#include "netapi.h"
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QHttpPart>
#include <QJsonDocument>
#include <QList>
#include <QString>


const QString &RequestData::getRequestName() const
{
    return m_requestName;
}

QVariant RequestData::getItemId() const
{
    return m_itemId;
}

QVariant RequestData::getItemAppId() const
{
    return m_itemAppId;
}

void RequestData::allowModelProccessError()
{
    m_processErrorInNetAPI = false;
}

void RequestData::setRequestName(const QString &requestName_)
{
    m_requestName = requestName_;
}

void RequestData::setItenId(const QVariant &itemId_)
{
    m_itemId = itemId_;
}

void RequestData::setItemAppId(const QVariant &itemAppId_)
{
    m_itemAppId = itemAppId_;
}

void RequestData::setReply(QNetworkReply *reply_)
{
    m_reply = reply_;
}

bool RequestData::compare(QNetworkReply *reply_) const
{
    return nullptr != m_reply && m_reply == reply_;
}

bool RequestData::processErrorInNetAPI() const
{
    return m_processErrorInNetAPI;
}


NetAPI::NetAPI(QObject *parent_ /*= nullptr*/)
    : QObject(parent_)
{
}

NetAPI::~NetAPI()
{
    clearData();
}

void NetAPI::setBasicAuthentification(QNetworkRequest* netRequest_)
{
    // https://stackoverflow.com/questions/1512849/basic-authentication-with-qt-qnetworkaccessmanager
    QString user_password = m_loggin + QString(":") + m_pass;
    QByteArray user_password_data = user_password.toLocal8Bit().toBase64();
    QString headerData = "Basic " + user_password_data;
    netRequest_->setRawHeader("Authorization", headerData.toLocal8Bit());
}

void NetAPI::clearData()
{
    if(nullptr != m_networkManager)
    {
        QObject::disconnect(m_networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replayFinished(QNetworkReply *)));
        delete m_networkManager;
        m_networkManager = nullptr;
    }
    for(auto *&p: m_requests)
    {
        delete p;
        p = nullptr;
    }
}

bool NetAPI::init()
{
    if(nullptr != m_networkManager) { return true; }
    m_networkManager = new QNetworkAccessManager();
    QObject::connect(m_networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replayFinished(QNetworkReply *)));
    return nullptr != m_networkManager;
}

void NetAPI::replayFinished(QNetworkReply *reply_)
{
    const auto fit = std::find_if(
                m_requests.begin(),
                m_requests.end(), [reply_](RequestData* r)->bool
    {
        return r->compare(reply_);
    });
    if(m_requests.end() == fit) { return; }
    RequestData* rd = *fit;
    m_requests.erase(fit);

    QJsonDocument jsonReply = QJsonDocument::fromJson(reply_->readAll());
    if(reply_->error() != QNetworkReply::NoError)
    {
        qDebug() << "HTTP Error : " << reply_->error();
        qDebug() << jsonReply.toJson();

        if(rd->processErrorInNetAPI())
        {
            emit error(int(reply_->error()), jsonReply);
        }
    }
    emit response(int(reply_->error()), rd, jsonReply);

    delete rd;
    rd = nullptr;
}

RequestData *NetAPI::getListByRefImpl(const QString &jsonLayoutName_, const QString &ref_, const QVariant &id_)
{
    const QString urlString = m_hostUrlBase + QString("%1/%2/by_%3/")
            .arg(jsonLayoutName_)
            .arg(id_.toString())
            .arg(ref_)
            ;
    QUrl url(urlString);
    QNetworkRequest request(url);

    setBasicAuthentification(&request);
    if(!init()) { return nullptr; }

    RequestData *rd = new RequestData();
    rd->setReply(m_networkManager->get(request));
    return rd;
}

RequestData *NetAPI::getListImpl(const QString &jsonLayoutName_)
{
    const QString urlString = m_hostUrlBase + QString("%1/")
            .arg(jsonLayoutName_)
            ;
    QUrl url(urlString);
    QNetworkRequest request(url);

    setBasicAuthentification(&request);
    if(!init()) { return nullptr; }

    RequestData *rd = new RequestData();
    rd->setReply(m_networkManager->get(request));
    return rd;
}

RequestData *NetAPI::addItemImpl(const QString &jsonLayoutName_, const QVariant &appId_, const QHash<QString, QVariant> &values_)
{
}

RequestData *NetAPI::setItemImpl(const QString &jsonLayoutName_, const QVariant &id_, const QHash<QString, QVariant> &values_)
{
}

