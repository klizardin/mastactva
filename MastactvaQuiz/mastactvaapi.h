#ifndef MASTACTVAAPI_H
#define MASTACTVAAPI_H


#include <QObject>
#include <QQuickItem>
#include <QDateTime>


class MastactvaAPI : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    MastactvaAPI(QObject *parent_ = nullptr);

    Q_INVOKABLE QString readMore(const QString &str_, int maxSymbols_, const QString &rmt_);
    Q_INVOKABLE QDateTime now() const;
    Q_INVOKABLE void showModelError(int errorCode_, const QString & errorCodeStr_, const QString & description_);
    Q_INVOKABLE QString calculateHash(const QString &fileUrl_);
    Q_INVOKABLE QString dateTimeToISOStr(const QDateTime &dt_);
    Q_INVOKABLE bool isLocalDataAPI();
    Q_INVOKABLE int getLocalDataAPIUserId();

    void setInitialized();

signals:
    void initialized();
    void errorMessage(const QString &header_, const QString &description_);
};


#endif // MASTACTVAAPI_H
