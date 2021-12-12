#include "addonmodulelist.h"
#if defined(unix)
    #include <dlfcn.h>
#endif
#include <stdlib.h>


static const char * g_addonModuleGetName = "addonModuleGetName";
static const char * g_addonModuleProcess = "addonModuleProcess";
static const char * g_echo = "echo";


QJsonDocument IAddonModule::process(const QJsonDocument &arguments_) const
{
    const QByteArray ba = arguments_.toJson();
    const QString result = process(QString::fromUtf8(ba));
    return QJsonDocument::fromJson(result.toUtf8());
}


AddonModule::~AddonModule()
{
    free();
}

bool AddonModule::create(const QFileInfo& dynamicLibraryName_)
{
#if defined(unix)
    resetErrors();
    m_dynamicLibraryHandle = dlopen(dynamicLibraryName_.absoluteFilePath().toUtf8().data(), RTLD_NOW);
    if(!m_dynamicLibraryHandle
            || checkErrors())
    {
        free();
        return false;
    }
    m_getNameFuncPtr = reinterpret_cast<addonModuleGetNameFuncPtr>(
                dlsym(
                    m_dynamicLibraryHandle,
                    g_addonModuleGetName
                    )
                );
    if(!m_getNameFuncPtr
            || checkErrors())
    {
        free();
        return false;
    }
    m_processFuncPtr = reinterpret_cast<addonModuleProcessFuncPtr>(
                dlsym(
                    m_dynamicLibraryHandle,
                    g_addonModuleProcess
                    )
                );
    if(!m_processFuncPtr
            || checkErrors())
    {
        free();
        return false;
    }
    return true;
#else
    Q_UNUSED(dynamicLibraryName_);
    return false;
#endif
}

void AddonModule::free()
{
#if defined(unix)
    if(m_dynamicLibraryHandle)
    {
        dlclose(m_dynamicLibraryHandle);
    }
    m_dynamicLibraryHandle = nullptr;
    m_getNameFuncPtr = nullptr;
    m_processFuncPtr = nullptr;
#else
    m_getNameFuncPtr = nullptr;
    m_processFuncPtr = nullptr;
#endif
}

void AddonModule::resetErrors()
{
#if defined(unix)
    dlerror();
#else
#endif
}

bool AddonModule::checkErrors() const
{
#if defined(unix)
    const char * libraryErrorSym = dlerror();
    qDebug() << QString::fromUtf8(libraryErrorSym);
    return libraryErrorSym;
#else
    return false;
#endif
}

AddonModule::operator bool () const
{
#if defined(unix)
    return m_dynamicLibraryHandle;
#else
    return false;
#endif
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


bool AddonModules::create(const QDir &addonsPath_)
{
    m_addons.clear();

    const QFileInfoList files = addonsPath_.entryInfoList(QStringList{} << "*.so", QDir::Files);
    for(const QFileInfo &fi_ : files)
    {
        std::unique_ptr<AddonModule> module = std::make_unique<AddonModule>();
        if(!module->create(fi_)
                || !*module
                )
        {
            continue;
        }

        const QString name = module->getName();
        const auto fit = std::find_if(
                    std::begin(m_addons),
                    std::end(m_addons),
                    [&name](const std::unique_ptr<IAddonModule> &module_)->bool
        {
            return name == module_->getName();
        });
        if(std::end(m_addons) == fit)
        {
            m_addons.push_back(std::move(module));
        }
    };
    m_defaultName = g_echo;
    return !m_addons.empty();
}

bool AddonModules::setDefault(const QString &name_)
{
    m_defaultName = name_;
    return std::any_of(
                std::begin(m_addons),
                std::end(m_addons),
                [this](const std::unique_ptr<IAddonModule> &module_)->bool
    {
        return m_defaultName == module_->getName();
    });
}

QStringList AddonModules::getNames() const
{
    QStringList result;
    std::transform(
                std::begin(m_addons),
                std::end(m_addons),
                std::back_inserter(result),
                [](const std::unique_ptr<IAddonModule> &module_)->QString
    {
        return module_->getName();
    });
    return result;
}

QJsonDocument AddonModules::call(
        const QString &name_,
        const QJsonDocument &arguments_
        ) const
{
    const auto fit = std::find_if(
                std::begin(m_addons),
                std::end(m_addons),
                [&name_](const std::unique_ptr<IAddonModule> &module_)->bool
    {
        return name_ == module_->getName();
    });
    if(std::end(m_addons) != fit)
    {
        return (*fit)->process(arguments_);
    }

    const auto fitDefault = std::find_if(
                std::begin(m_addons),
                std::end(m_addons),
                [this](const std::unique_ptr<IAddonModule> &module_)->bool
    {
        return m_defaultName == module_->getName();
    });
    if(std::end(m_addons) != fitDefault)
    {
        return (*fitDefault)->process(arguments_);
    }
    else
    {
        return QJsonDocument{};
    }
}
