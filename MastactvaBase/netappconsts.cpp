#include "netappconsts.h"


NetAppConsts::NetAppConsts()
{
}

const NetAppConsts &NetAppConsts::getInstance()
{
    static NetAppConsts instance;
    return instance;
}

void NetAppConsts::set(const QString &serverURL_,
                       const QString &playLogin_,
                       const QString &playPassword_)
{
    NetAppConsts & v = const_cast<NetAppConsts &>(getInstance());
    v.m_serverURL = serverURL_;
    v.m_playLogin = playLogin_;
    v.m_playPassword = playPassword_;
}
