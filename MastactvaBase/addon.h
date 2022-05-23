#ifndef ADDON_H
#define ADDON_H


#include <QString>

/*
    addons interface for effects sub-system
*/
class IEffectAddOn
{
public:
    virtual void getAddonNames(QStringList &names_) const = 0;
    virtual void runAddons(const QStringList &names_) const = 0;
};

#endif // ADDON_H
