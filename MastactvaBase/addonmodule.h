#ifndef ADDONMODULE_H
#define ADDONMODULE_H


class IAddonModule
{
public:
    virtual const char * getName() const = 0;
    virtual const char * process(const char * arguments_) const = 0;
};


extern "C" const char * addonModuleGetName();
extern "C" const char * addonModuleProcess(const char * );


#endif // ADDONMODULE_H
