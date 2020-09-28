#ifndef NETAPI_H
#define NETAPI_H

#include <QObject>
#include <QString>
#include <QList>
#include <QHttpPart>
#include <QJsonDocument>


class QNetworkReply;
class QHttpMultiPart;
class QByteArray;
class QFile;
class QNetworkAccessManager;
class NetAPI;
class MultiPartRequestData;

class RequestData
{
protected:
    bool m_skip = false;
    bool m_jsonReply = true;
    int m_requestID = 0;
    QString m_urlBase;
    QString m_login;
    QString m_pass;
    QNetworkReply *m_reply = nullptr;

public:
    explicit RequestData(const RequestData *defaultRequestData = nullptr);
    virtual ~RequestData() = default;

    int getRequestID() const;
    void setSkipRequest(bool skipRequest);
    void setUrlBase(const QString& urlBase_);
    void setAuth(const QString &login_, const QString &pass_);
    void setJsonReply(bool jsonReply_);

protected:
    void setRequestID(int requestID_);
    bool getSkipRequest() const;
    const QString& getUrlBase() const;
    const QString &getLogin() const;
    const QString &getPass() const;
    void setReply(QNetworkReply *reply_);
    QNetworkReply *getReply();
    bool isJsonReply() const;

    friend class NetAPI;
};

class MultiPartRequestData : public RequestData
{
protected:
    QHttpMultiPart* m_multiPart = nullptr;
    bool m_multiPartOwn = true;
    QList<QHttpPart> m_httpParts;

public:
    MultiPartRequestData(const RequestData *defaultRequestData = nullptr,
                         const MultiPartRequestData* defaultMultiPartRequestData = nullptr);
    virtual ~MultiPartRequestData() override;

    void addPart(const QString &header_, const QByteArray &data_);
    void addPart(const QString &header_, QFile *data_, bool takeOwnship_=true);

protected:
    QHttpMultiPart *getHttpMultiPart(bool releaseOwnship = false);
    void setHttpMultiPart(QHttpMultiPart *multiPart_);
    void appendParts();

    friend class NetAPI;
};

class JsonRequestData : public RequestData
{
protected:
    QJsonDocument m_jsondoc;

public:
    explicit JsonRequestData(const RequestData *defaultRequestData = nullptr);
    virtual ~JsonRequestData() override;

    void setDocument(const QJsonDocument &jsondoc_);

protected:
    void getDocument(QByteArray& byteArray_);
    void getDocumentLength(QByteArray& byteArray_);

    friend class NetAPI;
};

class NetAPI : public QObject
{
    Q_OBJECT

public:
    explicit NetAPI(QObject *parent_);
    virtual ~NetAPI() override;

    RequestData &getDefaultRequestData();
    MultiPartRequestData &getDefaultMultiPartRequestData();

    RequestData *startRequest();
    JsonRequestData *startJsonRequest();
    MultiPartRequestData *startMultiPartFormData();
    void get(const QString &urlStr_, RequestData *request_);
    void post(const QString &urlStr_, JsonRequestData *request_);
    void patch(const QString &urlStr_, JsonRequestData *request_);
    void del(const QString &urlStr_, JsonRequestData *request_);
    void post(const QString &urlStr_, MultiPartRequestData *request_);

    static void createSingelton(QObject *parent_);
    static NetAPI *getSingelton();

protected:
    bool init();
    int getNextRequestID();
    void setBasicAuthentification(QNetworkRequest* netRequest_, RequestData *request_);

signals:
    void onRequestFinished(RequestData *request_, QNetworkReply *reply_);
    void onJsonRequestFinished(RequestData *request_, const QJsonDocument &reply_);

private slots:
    void replayFinished(QNetworkReply *reply_);

protected:
    RequestData m_defaultRequestData;
    MultiPartRequestData m_defaultMultiPartRequestData;
    QNetworkAccessManager *m_networkManager = nullptr;
    int m_nextRequestID = 0;
    QList<RequestData*> m_requests;

    static NetAPI* apiSingelton;
};


#endif // NETAPI_H
