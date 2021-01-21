#ifndef NETAPPCONSTS_H
#define NETAPPCONSTS_H


#include <QString>
#include <QVector>


class NetAppConsts
{
public:
    QString m_serverURL;
    QString m_playLogin;
    QString m_playPassword;

protected:
    NetAppConsts();

public:
    static const NetAppConsts &getInstance();
    static void set(const QString &serverURL_,
                    const QString &playLogin_,
                    const QString &playPassword_);
};


#endif // NETAPPCONSTS_H
