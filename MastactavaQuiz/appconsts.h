#ifndef APPCONSTS_H
#define APPCONSTS_H


#include <QString>
#include <QVector>


class AppConsts
{
public:
    QString m_serverURL;
    QString m_playLogin;
    QString m_playPassword;


protected:
    AppConsts();

public:
    static const AppConsts &getInstance();

};

#endif // APPCONSTS_H
