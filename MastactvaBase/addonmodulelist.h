#ifndef ADDONMODULELIST_H
#define ADDONMODULELIST_H


#include <QString>
#include <QJsonDocument>
#include <QDir>
#include "../MastactvaBase/addonmodule.h"


class IAddonModule
{
public:
    virtual QString getName() const = 0;
    virtual QString process(const QString &arguments_) const = 0;
};


class AddonModule : public IAddonModule
{
public:
    AddonModule() = default;

    bool create(const QFileInfo& dynamicLibraryName_);
    operator bool () const;
    bool operator ! () const;

    QString getName() const override;
    QString process(const QString &arguments_) const override;

public:
    typedef const char * (*addonModuleGetNameFuncPtr)();
    typedef const char * (*addonModuleProcessFuncPtr)(const char * );

    addonModuleGetNameFuncPtr m_getNameFuncPtr = nullptr;
    addonModuleProcessFuncPtr m_processFuncPtr = nullptr;
};


class AddonModules
{
public:
    AddonModules() = default;
    bool create(const QDir &addonsPath_);
    bool setDefault(const QString &name_);

    QJsonDocument call(const QString &name, const QJsonDocument &arguments) const;

private:
    QVector<AddonModule> m_addons;
};


#endif // ADDONMODULELIST_H
