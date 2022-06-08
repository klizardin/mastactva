#ifndef ADDON_H
#define ADDON_H


#include <QString>

/*
    addons interface for effects sub-system
    it is required to start addons of the effect
    so that we can apply addons to the effect data
    it is an inteface that is used by GUI
    so it returns addons aligned to the effect
    and can apply banch of addons to the effect
*/
class IEffectAddOn
{
public:
    virtual void getAddonNames(QStringList &names_) const = 0;
    virtual void runAddons(const QStringList &names_) const = 0;
};

#endif // ADDON_H
