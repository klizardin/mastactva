#include "addonmodulelist.h"
#include <stdlib.h>


bool AddonModule::create(const QFileInfo& dynamicLibraryName_)
{
    Q_UNUSED(dynamicLibraryName_);
    return false;
}

AddonModule::operator bool () const
{
    return false;
}

bool AddonModule::operator ! () const
{
    return !operator bool();
}

QString AddonModule::getName() const
{
    if(operator!()
            || !m_getNameFuncPtr)
    {
        return QString{};
    }

    const char * resultStr = m_getNameFuncPtr();
    QString result = QString::fromUtf8(resultStr);
    std::free((void*)resultStr);
    return result;
}

QString AddonModule::process(const QString &arguments_) const
{
    if(operator!()
            || !m_processFuncPtr)
    {
        return QString{};
    }

    const QByteArray ba = arguments_.toUtf8();
    const char * resultStr = m_processFuncPtr(ba.data());
    QString result = QString::fromUtf8(resultStr);
    std::free((void*)resultStr);
    return result;
}
