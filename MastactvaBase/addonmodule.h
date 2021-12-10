#ifndef ADDONMODULE_H
#define ADDONMODULE_H


class IAddonModule
{
public:
    virtual const char * getName() const = 0;
    virtual const char * process(const char * arguments_) const = 0;
};


#endif // ADDONMODULE_H
