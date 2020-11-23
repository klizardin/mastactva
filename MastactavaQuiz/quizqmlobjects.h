#ifndef QUIZQMLOBJECTS_H
#define QUIZQMLOBJECTS_H


#include "qmlobjects.h"


class QMLObjects : public QMLObjectsBase
{
public:
    QMLObjects() = default;

protected:
    virtual void searchObjects() override;
};


#endif // QUIZQMLOBJECTS_H
