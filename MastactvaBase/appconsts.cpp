#include "appconsts.h"

NetAppConsts::NetAppConsts()
{
    m_serverURL = "http://localhost:8000/";
    m_playLogin = "player";
    m_playPassword = "qBv24MMpcWskrNr";
}

const NetAppConsts &NetAppConsts::getInstance()
{
    static NetAppConsts instance;
    return instance;
}

void NetAppConsts::set(const QString &serverURL_, const QString &playLogin_, const QString &playPassword_)
{
    NetAppConsts & v = const_cast<NetAppConsts &>(getInstance());
    v.m_serverURL = serverURL_;
    v.m_playLogin = playLogin_;
    v.m_playPassword = playPassword_;
}
