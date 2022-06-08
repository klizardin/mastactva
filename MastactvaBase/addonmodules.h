#ifndef ADDONMODULES_H
#define ADDONMODULES_H


#include <QStringList>
#include <QJsonDocument>
#include <QString>


/*
 * inteface to call addons from the dynamic libraries
 * the utility abstaction for lua scripting and so on
*/
class IAddonModules
{
public:
    virtual QStringList getNames() const = 0;
    virtual QJsonDocument call(const QString &name_, const QJsonDocument &arguments_) const = 0;
};


#endif // ADDONMODULES_H
