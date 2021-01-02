#ifndef QUIZQMLOBJECTS_H
#define QUIZQMLOBJECTS_H


#include <QJsonDocument>
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/qmlobjects.h"


class MastactvaAPI;


class QMLObjects : public QMLObjectsBase
{
public:
    QMLObjects() = default;

    virtual void setInitialized() override;
    virtual void modelError(int errorCode_, const QString &errorCodeStr_, const QJsonDocument &reply_) override;

protected:
    virtual void searchObjects() override;

private:
    MastactvaAPI *m_mastactvaAPI = nullptr;
};


#endif // QUIZQMLOBJECTS_H
