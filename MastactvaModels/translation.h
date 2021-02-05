#ifndef TRANSLATION_H
#define TRANSLATION_H


#include <QObject>


class Translation : public QObject
{
    Q_OBJECT
public:
    explicit Translation(QObject *parent_ = nullptr);

signals:

};


#endif // TRANSLATION_H
