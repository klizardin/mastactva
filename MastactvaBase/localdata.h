#ifndef LOCALDATA_H
#define LOCALDATA_H

#include <QObject>

class LocalData : public QObject
{
    Q_OBJECT
public:
    explicit LocalData(QObject *parent_ = nullptr);

signals:

};

#endif // LOCALDATA_H
