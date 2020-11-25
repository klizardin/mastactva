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

    void setInitialized();

signals:
    void initialized();

};

#endif // MASTACTVAAPI_H
