#ifndef MASTACTVAAPI_H
#define MASTACTVAAPI_H

#include <QObject>
#include <QQuickItem>

class MastactvaAPI : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    MastactvaAPI(QObject *parent_ = nullptr);

    Q_INVOKABLE QString readMore(const QString &str_, int maxSymbols_, const QString &rmt_);

signals:

};

#endif // MASTACTVAAPI_H
