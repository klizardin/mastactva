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
#include "../MastactvaBase/netappconsts.h"
#include "../MastactvaBase/utils.h"


NetRequestData::~NetRequestData()
{
    if(nullptr != m_reply)
    {
        m_reply->deleteLater();
    }
    m_reply = nullptr;
}

QNetworkReply *NetRequestData::getReply() const
{
    return m_reply;
}

void NetRequestData::setReply(QNetworkReply *reply_)
{
    m_reply = reply_;
}

bool NetRequestData::compare(QNetworkReply *reply_) const
{
    return nullptr != m_reply && m_reply == reply_;
}


class JsonRequestData : public NetRequestData
{
public:
    JsonRequestData(const QHash<QString, QVariant> &values_);
    virtual ~JsonRequestData() override = default;

    void getDocument(QByteArray &data_);
    void getDocumentLength(QByteArray &dataLength_);

private:
    QByteArray jsonData;
};


class MultipartRequestData : public NetRequestData
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


NetAPI::NetAPI(QObject *parent_ /*= nullptr*/)
    : QObject(parent_)
{
    updateAuthConsts();
}

NetAPI::~NetAPI()
{
    clearData();
}

void NetAPI::updateAuthConsts()
{
    m_hostName = NetAppConsts::getInstance().m_serverURL;
    m_loggin = NetAppConsts::getInstance().m_playLogin;
    m_pass = NetAppConsts::getInstance().m_playPassword;
    m_hostUrlBase = m_hostName;
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

QHash<QString, QVariant> NetAPI::merge(const QHash<QString, QVariant> &v1_, const QHash<QString, QVariant> &v2_)
{
    QHash<QString, QVariant> res;
    for(QHash<QString, QVariant>::const_iterator it = v2_.begin(); it != v2_.end(); ++it)
    {
        if(v1_.contains(it.key()))
        {
            res.insert(it.key(), v1_.value(it.key())); // for common prefere v1_
        }
        else
        {
            res.insert(it.key(), it.value());
        }
    }
    for(QHash<QString, QVariant>::const_iterator it = v1_.begin(); it != v1_.end(); ++it)
    {
        if(!v2_.contains(it.key()))
        {
            res.insert(it.key(), it.value());
        }
    }
    return res;
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
                m_requests.end(), [reply_](NetRequestData* r)->bool
    {
        return r->compare(reply_);
    });
    if(m_requests.end() == fit) { return; }
    RequestData* rd = *fit;
    m_requests.erase(fit);

    QJsonDocument jsonReply = QJsonDocument::fromJson(reply_->readAll());
    const QString errorCodeStr = QMetaEnum::fromType<QNetworkReply::NetworkError>().valueToKey(reply_->error());
    if(reply_->error() != QNetworkReply::NoError)
    {
        qDebug() << "HTTP Error : " << reply_->error();
        qDebug() << jsonReply.toJson();

        if(rd->processErrorInNetAPI())
        {
            emit error(int(reply_->error()), errorCodeStr, jsonReply);
        }
    }
    emit response(int(reply_->error()), errorCodeStr, rd, jsonReply);

    delete rd;
    rd = nullptr;
}

RequestData *NetAPI::emptyRequest(const QString &requestName_, const QVariant &itemAppId_, const QVariant &itemId_)
{
    NetRequestData *rd = new NetRequestData();
    rd->setRequestName(requestName_);
    rd->setItemAppId(itemAppId_);
    rd->setItemId(itemId_);
    return rd;
}

void NetAPI::freeRequest(RequestData *&r_)
{
    delete r_;
    r_ = nullptr;
}

RequestData *NetAPI::getListByRefImpl(const QString& requestName_,
                                      const QString &jsonLayoutName_,
                                      const QString &ref_,
                                      const QVariant &id_,
                                      bool jsonParams_,
                                      const QHash<QString, QVariant> &extraFields_)
{
    if(!id_.isValid()) { return nullptr; }
    Q_ASSERT(id_.toString() != "false");
    const QString urlString = m_hostUrlBase + QString("%1/%2/by_%3/")
            .arg(jsonLayoutName_,
                jsonParams_ ? QString("0") : id_.toString(),
                ref_)
            ;
    QUrl url(urlString);
    QNetworkRequest request(url);

    setBasicAuthentification(&request);
    if(!init()) { return nullptr; }

    NetRequestData *rd = nullptr;
    if(jsonParams_ || !extraFields_.isEmpty())
    {
        QHash<QString, QVariant> values(extraFields_);
        values.insert("id", id_.toString());
        JsonRequestData *jrd = new JsonRequestData(values);
        QByteArray jsonString;
        jrd->getDocument(jsonString);
        QByteArray postDataSize;
        jrd->getDocumentLength(postDataSize);
        request.setRawHeader("Content-Type", "application/json");
        request.setRawHeader("Content-Length", postDataSize);
        jrd->setReply(m_networkManager->sendCustomRequest(request, "GET", jsonString));
        rd = jrd;
    }
    else
    {
        rd = new NetRequestData();
        rd->setReply(m_networkManager->get(request));
    }
    rd->setRequestName(requestName_);
    m_requests.push_back(rd);
    return rd;
}

RequestData *NetAPI::getListByProcedureImpl(const QString& requestName_,
                                            const QString &jsonLayoutName_,
                                            const QString &procedureName_,
                                            bool jsonParams_,
                                            const QHash<QString, QVariant> &extraFields_)
{
    const QVariant id = extraFields_.contains("id") ? extraFields_.value("id") : QVariant::fromValue(QString("0"));
    QString urlString = m_hostUrlBase + QString("%1/%2/%3/")
            .arg(jsonLayoutName_,
                jsonParams_ ? QString("0") : id.toString(),
                procedureName_)
            ;
    int count = -1;
    const QList<QString> keys = extraFields_.keys();
    for(const QString &key : keys)
    {
        bool ok = false;
        const QVariant v = QVariant::fromValue(key);
        const QString vs = v.toString();
        int ki = v.toInt(&ok);
        ok &= isNumeric(vs);
        if(ok) { count = std::max(count, ki); }
    }
    for(int i = 0; i < count + 1; i++)
    {
        if(extraFields_.contains(QString::number(i)))
        {
            urlString += QString("%1/").arg(extraFields_.value(QString::number(i)).toString());
        }
        else
        {
            urlString += QString("0/");
        }
    }
    QUrl url(urlString);
    QNetworkRequest request(url);

    setBasicAuthentification(&request);
    if(!init()) { return nullptr; }

    NetRequestData *rd = nullptr;
    if(jsonParams_ || !extraFields_.isEmpty())
    {
        QHash<QString, QVariant> values(extraFields_);
        values.insert("id", id.toString());
        JsonRequestData *jrd = new JsonRequestData(values);
        QByteArray jsonString;
        jrd->getDocument(jsonString);
        QByteArray postDataSize;
        jrd->getDocumentLength(postDataSize);
        request.setRawHeader("Content-Type", "application/json");
        request.setRawHeader("Content-Length", postDataSize);
        jrd->setReply(m_networkManager->sendCustomRequest(request, "GET", jsonString));
        rd = jrd;
    }
    else
    {
        rd = new NetRequestData();
        rd->setReply(m_networkManager->get(request));
    }
    rd->setRequestName(requestName_);
    m_requests.push_back(rd);
    return rd;
}

RequestData *NetAPI::getListImpl(const QString& requestName_,
                                 const QString &jsonLayoutName_,
                                 const QHash<QString, QVariant> &extraFields_
                                 )
{
    const QString urlString = m_hostUrlBase + QString("%1/")
            .arg(jsonLayoutName_)
            ;
    QUrl url(urlString);
    QNetworkRequest request(url);

    setBasicAuthentification(&request);
    if(!init()) { return nullptr; }

    NetRequestData *rd = nullptr;
    if(!extraFields_.isEmpty())
    {
        JsonRequestData *jrd = new JsonRequestData(extraFields_);
        QByteArray jsonString;
        jrd->getDocument(jsonString);
        QByteArray postDataSize;
        jrd->getDocumentLength(postDataSize);
        request.setRawHeader("Content-Type", "application/json");
        request.setRawHeader("Content-Length", postDataSize);
        jrd->setReply(m_networkManager->sendCustomRequest(request, "GET", jsonString));
        rd = jrd;
    }
    else
    {
        rd = new NetRequestData();
        rd->setReply(m_networkManager->get(request));
    }
    rd->setRequestName(requestName_);
    m_requests.push_back(rd);
    return rd;
}

static bool anyArgIsFile(const QHash<QString, QVariant> &values_)
{
    const QList<QVariant> values = values_.values();
    for(const QVariant &v : values)
    {
        QObject * obj = qvariant_cast<QObject *>(v);
        QFile *f = qobject_cast<QFile*>(obj);
        if(nullptr != f) { return true; }
    }
    return false;
}


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
                        .arg(it.key(),
                            fileInfo.fileName().replace("\"", "")
                            ),
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
    for(const QHttpPart& part : qAsConst(m_httpParts))
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


RequestData *NetAPI::addItemImpl(const QString& requestName_, const QString &jsonLayoutName_, const QVariant &appId_, const QHash<QString, QVariant> &values_)
{
    const QString urlString = m_hostUrlBase + QString("%1/")
            .arg(jsonLayoutName_)
            ;
    QUrl url(urlString);
    QNetworkRequest request(url);

    setBasicAuthentification(&request);
    if(!init()) { return nullptr; }

    NetRequestData * rd = nullptr;
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
    rd->setRequestName(requestName_);
    m_requests.push_back(rd);
    return rd;
}

RequestData *NetAPI::setItemImpl(const QString& requestName_, const QString &jsonLayoutName_, const QVariant &id_, const QHash<QString, QVariant> &values_)
{
    Q_ASSERT(id_.isValid());
    Q_ASSERT(id_.toString() != "false");

    const QString urlString = m_hostUrlBase + QString("%1/%2/")
            .arg(jsonLayoutName_,
                id_.toString())
            ;
    QUrl url(urlString);
    QNetworkRequest request(url);

    setBasicAuthentification(&request);
    if(!init()) { return nullptr; }

    NetRequestData * rd = nullptr;
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
    rd->setRequestName(requestName_);
    m_requests.push_back(rd);
    return rd;
}

RequestData *NetAPI::delItemImpl(const QString& requestName_, const QString &jsonLayoutName_, const QVariant &id_)
{
    Q_ASSERT(id_.isValid());
    Q_ASSERT(id_.toString() != "false");

    const QString urlString = m_hostUrlBase + QString("%1/%2/")
            .arg(jsonLayoutName_,
                id_.toString())
            ;
    QUrl url(urlString);
    QNetworkRequest request(url);

    setBasicAuthentification(&request);
    if(!init()) { return nullptr; }

    NetRequestData *rd = new NetRequestData();
    rd->setReply(m_networkManager->sendCustomRequest(request, "DELETE"));

    rd->setItemId(id_);
    rd->setRequestName(requestName_);
    m_requests.push_back(rd);
    return rd;
}

static NetAPI *s_netAPI = nullptr;

void NetAPI::createInstance(QObject *parent_)
{
    if(nullptr == s_netAPI)
    {
        s_netAPI = new NetAPI(parent_);
    }
}

NetAPI *NetAPI::getInstance()
{
    return s_netAPI;
}
