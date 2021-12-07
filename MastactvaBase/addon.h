#ifndef ADDON_H
#define ADDON_H


#include <QString>


class IEffectAddOn
{
public:
    virtual void getAddonNames(QStringList &names_) const = 0;
    virtual void runAddon(const QStringList &names_) const = 0;
};

#endif // ADDON_H
