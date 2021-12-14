#ifndef ADDONMODULELIST_H
#define ADDONMODULELIST_H


#include <vector>
#include <QString>
#include <QJsonDocument>
#include <QDir>
#include "../MastactvaBase/addonmodule.h"
#include "../MastactvaBase/addonmodules.h"


class IAddonModule
{
public:
    virtual ~IAddonModule() = default;
    virtual QString getName() const = 0;
    virtual QString process(const QString &arguments_) const = 0;
    QJsonDocument process(const QJsonDocument &arguments_) const;
};


class AddonModule : public IAddonModule
{
public:
    AddonModule() = default;
    virtual ~AddonModule();

    bool create(const QFileInfo& dynamicLibraryName_);
    operator bool () const;
    bool operator ! () const;

    QString getName() const override;
    QString process(const QString &arguments_) const override;

private:
    void free();
    void resetErrors();
    bool checkErrors() const;

private:
    typedef const char * (*addonModuleGetNameFuncPtr)();
    typedef const char * (*addonModuleProcessFuncPtr)(const char * );

    addonModuleGetNameFuncPtr m_getNameFuncPtr = nullptr;
    addonModuleProcessFuncPtr m_processFuncPtr = nullptr;
#if defined(unix)
    void *m_dynamicLibraryHandle = nullptr;
#endif
};


class AddonModules : public IAddonModules
{
public:
    AddonModules() = default;
    bool create(const QDir &addonsPath_);
    bool setDefault(const QString &name_);

    QStringList getNames() const override;
    QJsonDocument call(const QString &name_, const QJsonDocument &arguments_) const override;

private:
    std::vector<std::unique_ptr<IAddonModule>> m_addons;
    QString m_defaultName;
};


#endif // ADDONMODULELIST_H
