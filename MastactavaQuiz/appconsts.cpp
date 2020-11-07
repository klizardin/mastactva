#include "appconsts.h"

AppConsts::AppConsts()
{
    m_serverURL = "http://localhost:8000/";
    m_playLogin = "player";
    m_playPassword = "qBv24MMpcWskrNr";
}

const AppConsts &AppConsts::getInstance()
{
    static AppConsts instance;
    return instance;
}
