#include "echo.h"


#include <string.h>
#include <QString>


static const char * g_name = "echo";


const char * addonModuleGetName()
{
    return g_name;
}

const char * addonModuleProcess(const char * arguments_)
{
    const QString str = QString::fromUtf8(arguments_);
    const QByteArray ba = str.toUtf8();
    char * buf = reinterpret_cast<char *>(malloc(ba.length() + 1));
    buf[0] = '\0';
    if(arguments_)
    {
        memcpy(buf, ba.data(), ba.length() + 1);
    }
    return buf;
}
