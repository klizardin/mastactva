#include "netapi.h"
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QHttpPart>
#include <QJsonDocument>
#include <QList>
#include <QString>
#include <QFile>
#include <QFileInfo>
#include <QByteArray>


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

void RequestData::setItemId(const QVariant &itemId_)
{
    m_itemId = itemId_;
}

void RequestData::setItemAppId(const QVariant &itemAppId_)
{
    m_itemAppId = itemAppId_;
}

QNetworkReply *RequestData::getReply() const
{
    return m_reply;
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

void NetAPI::setInitialized()
{
    emit initialized();
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

RequestData *NetAPI::emptyRequest()
{
    return new RequestData();
}

void NetAPI::freeRequest(RequestData *&r_)
{
    delete r_;
    r_ = nullptr;
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

static bool anyArgIsFile(const QHash<QString, QVariant> &values_)
{
    for(const QVariant &v : values_.values())
    {
        QObject * obj = qvariant_cast<QObject *>(v);
        QFile *f = qobject_cast<QFile*>(obj);
        if(nullptr != f) { return true; }
    }
    return false;
}


class MultipartRequestData : public RequestData
{
public:
    MultipartRequestData(const QHash<QString, QVariant> &values_);
    virtual ~MultipartRequestData() override;

    QHttpMultiPart *getHttpMultiPart(bool releaseOwnship_ = false);

protected:
    void addPart(const QString &header_, const QByteArray &data_);
    void addPart(const QString &header_, QFile *data_, bool takeOwnship_ = true);
    void appendParts();

private:
    QHttpMultiPart *m_multiPart = nullptr;
    QList<QHttpPart> m_httpParts;
};


MultipartRequestData::MultipartRequestData(const QHash<QString, QVariant> &values_)
{
    m_multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    for(QHash<QString, QVariant>::const_iterator it = values_.begin(); it != values_.end(); ++it)
    {
        QObject * obj = qvariant_cast<QObject *>(it.value());
        QFile *f = qobject_cast<QFile*>(obj);
        if(nullptr != f)
        {
            QFileInfo fileInfo(f->fileName());
            addPart(QString("form-data; name=\"%1\"; filename=\"%2\"")
                        .arg(it.key())
                        .arg(fileInfo.fileName().replace("\"", "")),
                    f, true
                    );
        }
        else
        {
            addPart(QString("form-data; name=\"%1\"").arg(it.key()), it.value().toString().toUtf8());
        }
    }
    appendParts();
}

MultipartRequestData::~MultipartRequestData()
{
    delete m_multiPart;
    m_multiPart = nullptr;
}

void MultipartRequestData::addPart(const QString &header_, const QByteArray &data_)
{
    m_httpParts.append(QHttpPart());
    m_httpParts.back().setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(header_));
    m_httpParts.back().setBody(data_);
}

void MultipartRequestData::addPart(const QString &header_, QFile *data_, bool takeOwnship_ /*= true*/)
{
    m_httpParts.append(QHttpPart());
    m_httpParts.back().setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(header_));
    data_->open(QIODevice::ReadOnly);
    m_httpParts.back().setBodyDevice(data_);
    if(takeOwnship_)
    {
        Q_ASSERT(nullptr != m_multiPart);
        if(nullptr == m_multiPart)
        {
            return;
        }
        data_->setParent(m_multiPart);
    }
}

void MultipartRequestData::appendParts()
{
    Q_ASSERT(nullptr != m_multiPart);
    if(nullptr == m_multiPart) { return; }
    for(const QHttpPart& part : m_httpParts)
    {
        m_multiPart->append(part);
    }
    m_httpParts.clear();
}


QHttpMultiPart *MultipartRequestData::getHttpMultiPart(bool releaseOwnship_ /*= false*/)
{
    QHttpMultiPart *ret = m_multiPart;
    if(releaseOwnship_)
    {
        m_multiPart = nullptr;
    }
    return ret;
}


class JsonRequestData : public RequestData
{
public:
    JsonRequestData(const QHash<QString, QVariant> &values_);
    virtual ~JsonRequestData() override = default;

    void getDocument(QByteArray &data_);
    void getDocumentLength(QByteArray &dataLength_);

private:
    QByteArray jsonData;
};


JsonRequestData::JsonRequestData(const QHash<QString, QVariant> &values_)
{
    QJsonObject rec;
    for(QHash<QString, QVariant>::const_iterator it = values_.begin(); it != values_.end(); ++it)
    {
        rec.insert(it.key(), QJsonValue::fromVariant(it.value()));
    }
    QJsonDocument doc(rec);
    jsonData = doc.toJson(QJsonDocument::Compact);
}

void JsonRequestData::getDocument(QByteArray &data_)
{
    data_ = jsonData;
}

void JsonRequestData::getDocumentLength(QByteArray &dataLength_)
{
    dataLength_ = QByteArray::number(jsonData.size());
}


RequestData *NetAPI::addItemImpl(const QString &jsonLayoutName_, const QVariant &appId_, const QHash<QString, QVariant> &values_)
{
    const QString urlString = m_hostUrlBase + QString("%1/")
            .arg(jsonLayoutName_)
            ;
    QUrl url(urlString);
    QNetworkRequest request(url);

    setBasicAuthentification(&request);
    if(!init()) { return nullptr; }

    RequestData * rd = nullptr;
    if(anyArgIsFile(values_))
    {
        MultipartRequestData *mprd = new MultipartRequestData(values_);
        mprd->setItemAppId(appId_);
        mprd->setReply(m_networkManager->post(request, mprd->getHttpMultiPart()));
        mprd->getHttpMultiPart(true)->setParent(mprd->getReply());
        rd = mprd;
    }
    else
    {
        JsonRequestData *jrd = new JsonRequestData(values_);
        jrd->setItemAppId(appId_);
        QByteArray jsonString;
        jrd->getDocument(jsonString);
        QByteArray postDataSize;
        jrd->getDocumentLength(postDataSize);
        request.setRawHeader("Content-Type", "application/json");
        request.setRawHeader("Content-Length", postDataSize);
        jrd->setReply(m_networkManager->post(request, jsonString));
        rd = jrd;
    }
    return rd;
}

RequestData *NetAPI::setItemImpl(const QString &jsonLayoutName_, const QVariant &id_, const QHash<QString, QVariant> &values_)
{
    const QString urlString = m_hostUrlBase + QString("%1/%2/")
            .arg(jsonLayoutName_)
            .arg(id_.toString())
            ;
    QUrl url(urlString);
    QNetworkRequest request(url);

    setBasicAuthentification(&request);
    if(!init()) { return nullptr; }

    RequestData * rd = nullptr;
    if(anyArgIsFile(values_))
    {
        MultipartRequestData *mprd = new MultipartRequestData(values_);
        mprd->setItemId(id_);
        mprd->setReply(m_networkManager->sendCustomRequest(request, "PATCH", mprd->getHttpMultiPart()));
        mprd->getHttpMultiPart(true)->setParent(mprd->getReply());
        rd = mprd;
    }
    else
    {
        JsonRequestData *jrd = new JsonRequestData(values_);
        jrd->setItemId(id_);
        QByteArray jsonString;
        jrd->getDocument(jsonString);
        QByteArray postDataSize;
        jrd->getDocumentLength(postDataSize);
        request.setRawHeader("Content-Type", "application/json");
        request.setRawHeader("Content-Length", postDataSize);
        jrd->setReply(m_networkManager->sendCustomRequest(request, "PATCH", jsonString));
        rd = jrd;
    }
    return rd;
}

