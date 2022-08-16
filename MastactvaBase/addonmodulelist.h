/*
    Copyright 2021

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef ADDONMODULELIST_H
#define ADDONMODULELIST_H


#include <vector>
#include <QString>
#include <QJsonDocument>
#include <QDir>
#include "../MastactvaBase/addonmodule.h"
#include "../MastactvaBase/addonmodules.h"


/*
 * abstract interface of an addon module
 * with utility function to pass and return QJsonDocument to a module
*/
class IAddonModule
{
public:
    virtual ~IAddonModule() = default;
    virtual QString getName() const = 0;
    virtual QString process(const QString &arguments_) const = 0;
    QJsonDocument process(const QJsonDocument &arguments_) const;
};


/*
 * implementation of the IAddonModule interface
*/
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


/*
 * to load addons in the specific path
 * and do calls to the addons
*/
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
