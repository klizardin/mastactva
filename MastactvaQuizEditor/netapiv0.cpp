#include "netapiv0.h"
#include <QNetworkReply>
#include <QHttpMultiPart>
#include <QByteArray>
#include <QFile>
#include <QNetworkAccessManager>
#include "qmlmainobjects.h"
#include "mastactvaapi.h"


RequestDataV0::RequestDataV0(const RequestDataV0 *defaultRequestData /*= nullptr*/)
{
    if(nullptr != defaultRequestData)
    {
        setRequestID(defaultRequestData->getRequestID());
        setUrlBase(defaultRequestData->getUrlBase());
        setAuth(defaultRequestData->getLogin(), defaultRequestData->getPass());
    }
}

RequestDataV0::~RequestDataV0()
{
    if(nullptr != m_reply)
    {
        m_reply->deleteLater();
    }
    m_reply = nullptr;
}

int RequestDataV0::getRequestID() const
{
    return m_requestID;
}

void RequestDataV0::setRequestID(int requestID_)
{
    m_requestID = requestID_;
}

void RequestDataV0::setSkipRequest(bool skipRequest)
{
    m_skip = skipRequest;
}

bool RequestDataV0::getSkipRequest() const
{
    return m_skip;
}

void RequestDataV0::setUrlBase(const QString& urlBase_)
{
    m_urlBase = urlBase_;
}

const QString &RequestDataV0::getUrlBase() const
{
    return m_urlBase;
}

bool RequestDataV0::processErrorInResponseHandle() const
{
    return m_processErrorInResposeHandle;
}

void RequestDataV0::setProcessErrorInResponseHandle(bool processErrorInResponseHandle_)
{
    m_processErrorInResposeHandle = processErrorInResponseHandle_;
}

void RequestDataV0::setJsonReply(bool jsonReply_)
{
    m_jsonReply = jsonReply_;
}

bool RequestDataV0::isJsonReply() const
{
    return m_jsonReply;
}

void RequestDataV0::setAuth(const QString &login_, const QString &pass_)
{
    m_login = login_;
    m_pass = pass_;
}

const QString &RequestDataV0::getLogin() const
{
    return m_login;
}

const QString &RequestDataV0::getPass() const
{
    return m_pass;
}

void RequestDataV0::setReply(QNetworkReply *reply_)
{
    m_reply = reply_;
}

QNetworkReply *RequestDataV0::getReply()
{
    return m_reply;
}

MultiPartRequestDataV0::MultiPartRequestDataV0(const RequestDataV0 *defaultRequestData /*= nullptr*/,
                     const MultiPartRequestDataV0* defaultMultiPartRequestData /*= nullptr*/)
    :RequestDataV0(defaultRequestData)
{
    if(nullptr != defaultMultiPartRequestData)
    {
        std::copy(std::begin(defaultMultiPartRequestData->m_httpParts),
                  std::end(defaultMultiPartRequestData->m_httpParts),
                  std::inserter(m_httpParts, std::end(m_httpParts)));
    }
}


MultiPartRequestDataV0::~MultiPartRequestDataV0()
{
    if(m_multiPartOwn)
    {
        delete  m_multiPart;
    }
    m_multiPart = nullptr;
}

QHttpMultiPart *MultiPartRequestDataV0::getHttpMultiPart(bool releaseOwnship /*= false*/)
{
    if(releaseOwnship)
    {
        m_multiPartOwn = false;
    }
    return m_multiPart;
}

void MultiPartRequestDataV0::setHttpMultiPart(QHttpMultiPart *multiPart_)
{
    m_multiPart = multiPart_;
    m_multiPartOwn = true;
}

void MultiPartRequestDataV0::addPart(const QString &header_, const QByteArray &data_)
{
    m_httpParts.append(QHttpPart());
    m_httpParts.back().setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(header_));
    m_httpParts.back().setBody(data_);
}

void MultiPartRequestDataV0::addPart(const QString &header_, QFile *data_, bool takeOwnship_ /*=true*/)
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

void MultiPartRequestDataV0::appendParts()
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

JsonRequestDataV0::JsonRequestDataV0(const RequestDataV0 *defaultRequestData /*= nullptr*/)
    :RequestDataV0(defaultRequestData)
{
}

JsonRequestDataV0::~JsonRequestDataV0()
{
}

void JsonRequestDataV0::setDocument(const QJsonDocument &jsondoc_)
{
    m_jsondoc = jsondoc_;
}

void JsonRequestDataV0::getDocument(QByteArray& byteArray_)
{
    byteArray_ = m_jsondoc.toJson(QJsonDocument::Compact);
}

void JsonRequestDataV0::getDocumentLength(QByteArray& byteArray_)
{
    byteArray_ = QByteArray::number(m_jsondoc.toJson(QJsonDocument::Compact).size());
}

NetAPIV0::NetAPIV0(QObject *parent_)
    :QObject(parent_)
{
}

NetAPIV0::~NetAPIV0()
{
    delete m_networkManager;
    m_networkManager = nullptr;
}

RequestDataV0 &NetAPIV0::getDefaultRequestData()
{
    return m_defaultRequestData;
}

MultiPartRequestDataV0 &NetAPIV0::getDefaultMultiPartRequestData()
{
    return m_defaultMultiPartRequestData;
}

bool NetAPIV0::init()
{
    if(nullptr != m_networkManager)
    {
        return true;
    }
    m_networkManager = new QNetworkAccessManager();
    QObject::connect(m_networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replayFinished(QNetworkReply *)));
    return nullptr != m_networkManager;
}

int NetAPIV0::getNextRequestID()
{
    return m_nextRequestID++;
}

RequestDataV0 *NetAPIV0::startRequest()
{
    RequestDataV0 *rd = new RequestDataV0(&getDefaultRequestData());
    rd->setRequestID(getNextRequestID());
    m_requests.push_back(rd);
    return rd;
}

JsonRequestDataV0 *NetAPIV0::startJsonRequest()
{
    JsonRequestDataV0 *rd = new JsonRequestDataV0(&getDefaultRequestData());
    rd->setRequestID(getNextRequestID());
    m_requests.push_back(rd);
    return rd;
}

MultiPartRequestDataV0 *NetAPIV0::startMultiPartFormData()
{
    QHttpMultiPart* multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    MultiPartRequestDataV0* rd = new MultiPartRequestDataV0(&getDefaultRequestData(), &getDefaultMultiPartRequestData());
    rd->setRequestID(getNextRequestID());
    rd->setHttpMultiPart(multiPart);
    m_requests.push_back(rd);
    return rd;
}

void NetAPIV0::setBasicAuthentification(QNetworkRequest* netRequest_, RequestDataV0 *request_)
{
    // https://stackoverflow.com/questions/1512849/basic-authentication-with-qt-qnetworkaccessmanager
    QString user_password = request_->getLogin() + QString(":") + request_->getPass();
    QByteArray user_password_data = user_password.toLocal8Bit().toBase64();
    QString headerData = "Basic " + user_password_data;
    netRequest_->setRawHeader("Authorization", headerData.toLocal8Bit());
}

void NetAPIV0::get(const QString &urlStr_, RequestDataV0 *request_)
{
    Q_ASSERT(nullptr != request_);
    if(nullptr == request_)
    {
        return;
    }
    QUrl url(request_->getUrlBase() + urlStr_);
    QNetworkRequest request(url);

    setBasicAuthentification(&request, request_);

    if(!init())
    {
        return;
    }

    request_->setReply(m_networkManager->get(request));
}

void NetAPIV0::post(const QString &urlStr_, JsonRequestDataV0 *request_)
{
    Q_ASSERT(nullptr != request_);
    if(nullptr == request_)
    {
        return;
    }
    QUrl url(request_->getUrlBase() + urlStr_);
    QNetworkRequest request(url);

    setBasicAuthentification(&request, request_);

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

void NetAPIV0::patch(const QString &urlStr_, JsonRequestDataV0 *request_)
{
    Q_ASSERT(nullptr != request_);
    if(nullptr == request_)
    {
        return;
    }
    QUrl url(request_->getUrlBase() + urlStr_);
    QNetworkRequest request(url);

    setBasicAuthentification(&request, request_);

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

void NetAPIV0::del(const QString &urlStr_, JsonRequestDataV0 *request_)
{
    Q_ASSERT(nullptr != request_);
    if(nullptr == request_)
    {
        return;
    }
    QUrl url(request_->getUrlBase() + urlStr_);
    QNetworkRequest request(url);

    setBasicAuthentification(&request, request_);

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

    request_->setReply(m_networkManager->sendCustomRequest(request, "DELETE", jsonString));
}


void NetAPIV0::post(const QString &urlStr_, MultiPartRequestDataV0 *request_)
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

    setBasicAuthentification(&request, request_);

    if(!init())
    {
        return;
    }

    request_->setReply(m_networkManager->post(request, request_->getHttpMultiPart()));
    request_->getHttpMultiPart(true)->setParent(request_->getReply());
}

void NetAPIV0::replayFinished(QNetworkReply *reply_)
{
    const auto fit = std::find_if(
                m_requests.begin(),
                m_requests.end(), [reply_](RequestDataV0* r)->bool
    {
            return r->getReply()==reply_;
    });
    if(m_requests.end() == fit)
    {
        return;
    }
    RequestDataV0* rd = *fit;
    m_requests.erase(fit);
    if(!rd->getSkipRequest())
    {
        if(rd->isJsonReply())
        {
            QJsonDocument jsonReply = QJsonDocument::fromJson(reply_->readAll());
            if(reply_->error() != QNetworkReply::NoError)
            {
                qDebug() << "HTTP Error : " << reply_->error();
                qDebug() << jsonReply.toJson();

                if(!rd->processErrorInResponseHandle()) {
                    MastactvaAPI *mastactvaAPI = QMLMainObjects::getSingelton()->getMastactvaAPI();
                    if(nullptr != mastactvaAPI)
                    {
                        mastactvaAPI->showErrorMessage("",
                                                       QString("Error code: %1\nServer Answer:\n%2")
                                                       .arg(reply_->error())
                                                       .arg((QString)jsonReply.toJson())
                                                       );
                    }
                }
            }
            emit onJsonRequestFinished(int(reply_->error()), rd, jsonReply);
        }
        else
        {
            emit onRequestFinished(rd, reply_);
        }
    }
    delete rd;
    rd = nullptr;
}

NetAPIV0* NetAPIV0::apiSingelton = nullptr;

void NetAPIV0::createSingelton(QObject *parent_)
{
    Q_ASSERT(nullptr == apiSingelton);
    if(nullptr != apiSingelton)
    {
        return;
    }
    apiSingelton = new NetAPIV0(parent_);
}

NetAPIV0 *NetAPIV0::getSingelton()
{
    return apiSingelton;
}
