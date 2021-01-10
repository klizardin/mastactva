#ifndef LOCALDATAAPICACHE_H
#define LOCALDATAAPICACHE_H


#include <QObject>


class LocalDataAPICache : public QObject
{
    Q_OBJECT
public:
    explicit LocalDataAPICache(QObject *parent_ = nullptr);

signals:

};


#endif // LOCALDATAAPICACHE_H
