#ifndef QUIZQMLOBJECTS_H
#define QUIZQMLOBJECTS_H


#include "../MastactvaBase/qmlobjects.h"


class MastactvaAPI;


class QMLObjects : public QMLObjectsBase
{
public:
    QMLObjects() = default;

    virtual void setInitialized() override;

protected:
    virtual void searchObjects() override;

private:
    MastactvaAPI *m_mastactvaAPI = nullptr;
};


#endif // QUIZQMLOBJECTS_H
