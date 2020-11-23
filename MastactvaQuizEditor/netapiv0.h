#ifndef NETAPIV0_H
#define NETAPIV0_H

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
class NetAPIV0;
class MultiPartRequestDataV0;

class RequestDataV0
{
protected:
    bool m_skip = false;
    bool m_jsonReply = true;
    bool m_processErrorInResposeHandle = false;
    int m_requestID = 0;
    QString m_urlBase;
    QString m_login;
    QString m_pass;
    QNetworkReply *m_reply = nullptr;

public:
    explicit RequestDataV0(const RequestDataV0 *defaultRequestData = nullptr);
    virtual ~RequestDataV0() = default;

    int getRequestID() const;
    void setSkipRequest(bool skipRequest);
    void setUrlBase(const QString& urlBase_);
    void setAuth(const QString &login_, const QString &pass_);
    void setJsonReply(bool jsonReply_);
    const QString& getUrlBase() const;
    bool processErrorInResponseHandle() const;
    void setProcessErrorInResponseHandle(bool processErrorInResponseHandle_);
    const QString &getLogin() const;
    const QString &getPass() const;

protected:
    void setRequestID(int requestID_);
    bool getSkipRequest() const;
    void setReply(QNetworkReply *reply_);
    QNetworkReply *getReply();
    bool isJsonReply() const;

    friend class NetAPIV0;
};

class MultiPartRequestDataV0 : public RequestDataV0
{
protected:
    QHttpMultiPart* m_multiPart = nullptr;
    bool m_multiPartOwn = true;
    QList<QHttpPart> m_httpParts;

public:
    MultiPartRequestDataV0(const RequestDataV0 *defaultRequestData = nullptr,
                         const MultiPartRequestDataV0* defaultMultiPartRequestData = nullptr);
    virtual ~MultiPartRequestDataV0() override;

    void addPart(const QString &header_, const QByteArray &data_);
    void addPart(const QString &header_, QFile *data_, bool takeOwnship_=true);

protected:
    QHttpMultiPart *getHttpMultiPart(bool releaseOwnship = false);
    void setHttpMultiPart(QHttpMultiPart *multiPart_);
    void appendParts();

    friend class NetAPIV0;
};

class JsonRequestDataV0 : public RequestDataV0
{
protected:
    QJsonDocument m_jsondoc;

public:
    explicit JsonRequestDataV0(const RequestDataV0 *defaultRequestData = nullptr);
    virtual ~JsonRequestDataV0() override;

    void setDocument(const QJsonDocument &jsondoc_);

protected:
    void getDocument(QByteArray& byteArray_);
    void getDocumentLength(QByteArray& byteArray_);

    friend class NetAPIV0;
};

class NetAPIV0 : public QObject
{
    Q_OBJECT

public:
    explicit NetAPIV0(QObject *parent_);
    virtual ~NetAPIV0() override;

    RequestDataV0 &getDefaultRequestData();
    MultiPartRequestDataV0 &getDefaultMultiPartRequestData();

    RequestDataV0 *startRequest();
    JsonRequestDataV0 *startJsonRequest();
    MultiPartRequestDataV0 *startMultiPartFormData();
    void get(const QString &urlStr_, RequestDataV0 *request_);
    void post(const QString &urlStr_, JsonRequestDataV0 *request_);
    void patch(const QString &urlStr_, JsonRequestDataV0 *request_);
    void del(const QString &urlStr_, JsonRequestDataV0 *request_);
    void post(const QString &urlStr_, MultiPartRequestDataV0 *request_);

    static void createSingelton(QObject *parent_);
    static NetAPIV0 *getSingelton();

protected:
    bool init();
    int getNextRequestID();
    void setBasicAuthentification(QNetworkRequest* netRequest_, RequestDataV0 *request_);

signals:
    void onRequestFinished(RequestDataV0 *request_, QNetworkReply *reply_);
    void onJsonRequestFinished(int errorCode_, RequestDataV0 *request_, const QJsonDocument &reply_);

private slots:
    void replayFinished(QNetworkReply *reply_);

protected:
    RequestDataV0 m_defaultRequestData;
    MultiPartRequestDataV0 m_defaultMultiPartRequestData;
    QNetworkAccessManager *m_networkManager = nullptr;
    int m_nextRequestID = 0;
    QList<RequestDataV0*> m_requests;

    static NetAPIV0* apiSingelton;
};


#endif // NETAPIV0_H
