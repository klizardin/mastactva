#include "netapi.h"
#include <QNetworkReply>
#include <QHttpMultiPart>
#include <QByteArray>
#include <QFile>
#include <QNetworkAccessManager>


RequestData::RequestData(const RequestData *defaultRequestData /*= nullptr*/)
{
    if(nullptr != defaultRequestData)
    {
        setRequestID(defaultRequestData->getRequestID());
        setUrlBase(defaultRequestData->getUrlBase());
        setAuth(defaultRequestData->getLogin(), defaultRequestData->getPass());
    }
}

int RequestData::getRequestID() const
{
    return m_requestID;
}

void RequestData::setRequestID(int requestID_)
{
    m_requestID = requestID_;
}

void RequestData::setUrlBase(const QString& urlBase_)
{
    m_urlBase = urlBase_;
}

const QString &RequestData::getUrlBase() const
{
    return m_urlBase;
}

void RequestData::setJsonReply(bool jsonReply_)
{
    m_jsonReply = jsonReply_;
}

bool RequestData::isJsonReply() const
{
    return m_jsonReply;
}

void RequestData::setAuth(const QString &login_, const QString &pass_)
{
    m_login = login_;
    m_pass = pass_;
}

const QString &RequestData::getLogin() const
{
    return m_login;
}

const QString &RequestData::getPass() const
{
    return m_pass;
}

void RequestData::setReply(QNetworkReply *reply_)
{
    m_reply = reply_;
}

QNetworkReply *RequestData::getReply()
{
    return m_reply;
}

MultiPartRequestData::MultiPartRequestData(const RequestData *defaultRequestData /*= nullptr*/,
                     const MultiPartRequestData* defaultMultiPartRequestData /*= nullptr*/)
    :RequestData(defaultRequestData)
{
    if(nullptr != defaultMultiPartRequestData)
    {
        std::copy(std::begin(defaultMultiPartRequestData->m_httpParts),
                  std::end(defaultMultiPartRequestData->m_httpParts),
                  std::inserter(m_httpParts, std::end(m_httpParts)));
    }
}


MultiPartRequestData::~MultiPartRequestData()
{
    if(m_multiPartOwn)
    {
        delete  m_multiPart;
    }
    m_multiPart = nullptr;
}

QHttpMultiPart *MultiPartRequestData::getHttpMultiPart(bool releaseOwnship /*= false*/)
{
    if(releaseOwnship)
    {
        m_multiPartOwn = false;
    }
    return m_multiPart;
}

void MultiPartRequestData::setHttpMultiPart(QHttpMultiPart *multiPart_)
{
    m_multiPart = multiPart_;
    m_multiPartOwn = true;
}

void MultiPartRequestData::addPart(const QString &header_, const QByteArray &data_)
{
    m_httpParts.append(QHttpPart());
    m_httpParts.back().setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(header_));
    m_httpParts.back().setBody(data_);
}

void MultiPartRequestData::addPart(const QString &header_, QFile *data_, bool takeOwnship_ /*=true*/)
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

void MultiPartRequestData::appendParts()
{
    Q_ASSERT(nullptr != m_multiPart);
    if(nullptr == m_multiPart)
    {
        return;
    }
    for(const QHttpPart& part : m_httpParts)
    {
        m_multiPart->append(part);
    }
    m_httpParts.clear();
}

JsonRequestData::JsonRequestData(const RequestData *defaultRequestData /*= nullptr*/)
    :RequestData(defaultRequestData)
{
}

JsonRequestData::~JsonRequestData()
{
}

void JsonRequestData::setDocument(const QJsonDocument &jsondoc_)
{
    m_jsondoc = jsondoc_;
}

void JsonRequestData::getDocument(QByteArray& byteArray_)
{
    byteArray_ = m_jsondoc.toJson(QJsonDocument::Compact);
}

void JsonRequestData::getDocumentLength(QByteArray& byteArray_)
{
    byteArray_ = QByteArray::number(m_jsondoc.toJson(QJsonDocument::Compact).size());
}

NetAPI::NetAPI(QObject *parent_)
    :QObject(parent_)
{
}

NetAPI::~NetAPI()
{
    delete m_networkManager;
    m_networkManager = nullptr;
}

RequestData &NetAPI::getDefaultRequestData()
{
    return m_defaultRequestData;
}

MultiPartRequestData &NetAPI::getDefaultMultiPartRequestData()
{
    return m_defaultMultiPartRequestData;
}

bool NetAPI::init()
{
    if(nullptr != m_networkManager)
    {
        return true;
    }
    m_networkManager = new QNetworkAccessManager();
    QObject::connect(m_networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replayFinished(QNetworkReply *)));
    return nullptr != m_networkManager;
}

int NetAPI::getNextRequestID()
{
    return m_nextRequestID++;
}

RequestData *NetAPI::startRequest()
{
    RequestData *rd = new RequestData(&getDefaultRequestData());
    rd->setRequestID(getNextRequestID());
    m_requests.push_back(rd);
    return rd;
}

JsonRequestData *NetAPI::startJsonRequest()
{
    JsonRequestData *rd = new JsonRequestData(&getDefaultRequestData());
    rd->setRequestID(getNextRequestID());
    m_requests.push_back(rd);
    return rd;
}

MultiPartRequestData *NetAPI::startMultiPartFormData()
{
    QHttpMultiPart* multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    MultiPartRequestData* rd = new MultiPartRequestData(&getDefaultRequestData(), &getDefaultMultiPartRequestData());
    rd->setRequestID(getNextRequestID());
    rd->setHttpMultiPart(multiPart);
    m_requests.push_back(rd);
    return rd;
}

void NetAPI::get(const QString &urlStr_, RequestData *request_)
{
    Q_ASSERT(nullptr != request_);
    if(nullptr == request_)
    {
        return;
    }
    QUrl url(request_->getUrlBase() + urlStr_);
    QNetworkRequest request(url);

    // https://stackoverflow.com/questions/1512849/basic-authentication-with-qt-qnetworkaccessmanager
    QString user_password = request_->getLogin() + QString(":") + request_->getPass();
    QByteArray user_password_data = user_password.toLocal8Bit().toBase64();
    QString headerData = "Basic " + user_password_data;
    request.setRawHeader("Authorization", headerData.toLocal8Bit());

    if(!init())
    {
        return;
    }

    request_->setReply(m_networkManager->get(request));
}

void NetAPI::post(const QString &urlStr_, JsonRequestData *request_)
{
    Q_ASSERT(nullptr != request_);
    if(nullptr == request_)
    {
        return;
    }
    QUrl url(request_->getUrlBase() + urlStr_);
    QNetworkRequest request(url);
    // https://stackoverflow.com/questions/1512849/basic-authentication-with-qt-qnetworkaccessmanager
    QString user_password = request_->getLogin() + QString(":") + request_->getPass();
    QByteArray user_password_data = user_password.toLocal8Bit().toBase64();
    QString headerData = "Basic " + user_password_data;
    request.setRawHeader("Authorization", headerData.toLocal8Bit());

    QByteArray jsonString;
    request_->getDocument(jsonString);
    QByteArray postDataSize;
    request_->getDocumentLength(postDataSize);

    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("Content-Length", postDataSize);

    if(!init())
    {
        return;
    }

    request_->setReply(m_networkManager->post(request, jsonString));
}

void NetAPI::patch(const QString &urlStr_, JsonRequestData *request_)
{
    Q_ASSERT(nullptr != request_);
    if(nullptr == request_)
    {
        return;
    }
    QUrl url(request_->getUrlBase() + urlStr_);
    QNetworkRequest request(url);
    // https://stackoverflow.com/questions/1512849/basic-authentication-with-qt-qnetworkaccessmanager
    QString user_password = request_->getLogin() + QString(":") + request_->getPass();
    QByteArray user_password_data = user_password.toLocal8Bit().toBase64();
    QString headerData = "Basic " + user_password_data;
    request.setRawHeader("Authorization", headerData.toLocal8Bit());

    QByteArray jsonString;
    request_->getDocument(jsonString);
    QByteArray postDataSize;
    request_->getDocumentLength(postDataSize);

    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("Content-Length", postDataSize);

    if(!init())
    {
        return;
    }

    request_->setReply(m_networkManager->sendCustomRequest(request, "PATCH", jsonString));
}

void NetAPI::post(const QString &urlStr_, MultiPartRequestData *request_)
{
    Q_ASSERT(nullptr != request_);
    Q_ASSERT(nullptr != request_->getHttpMultiPart());
    if(nullptr == request_
            || nullptr == request_->getHttpMultiPart())
    {
        return;
    }
    request_->appendParts();

    QUrl url(request_->getUrlBase() + urlStr_);
    QNetworkRequest request(url);

    // https://stackoverflow.com/questions/1512849/basic-authentication-with-qt-qnetworkaccessmanager
    QString user_password = request_->getLogin() + QString(":") + request_->getPass();
    QByteArray user_password_data = user_password.toLocal8Bit().toBase64();
    QString headerData = "Basic " + user_password_data;
    request.setRawHeader("Authorization", headerData.toLocal8Bit());

    if(!init())
    {
        return;
    }

    request_->setReply(m_networkManager->post(request, request_->getHttpMultiPart()));
    request_->getHttpMultiPart(true)->setParent(request_->getReply());
}

void NetAPI::replayFinished(QNetworkReply *reply_)
{
    const auto fit = std::find_if(
                m_requests.begin(),
                m_requests.end(), [reply_](RequestData* r)->bool
    {
            return r->getReply()==reply_;
    });
    if(m_requests.end() == fit)
    {
        return;
    }
    if((*fit)->isJsonReply())
    {
        QJsonDocument jsonReply = QJsonDocument::fromJson(reply_->readAll());
        emit onJsonRequestFinished(*fit, jsonReply);
    }
    else
    {
        emit onRequestFinished(*fit, reply_);
    }
    m_requests.erase(fit);
}

NetAPI* NetAPI::apiSingelton = nullptr;

void NetAPI::createSingelton(QObject *parent_)
{
    Q_ASSERT(nullptr == apiSingelton);
    if(nullptr != apiSingelton)
    {
        return;
    }
    apiSingelton = new NetAPI(parent_);
}

NetAPI *NetAPI::getSingelton()
{
    return apiSingelton;
}
