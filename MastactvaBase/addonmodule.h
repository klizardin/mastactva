#ifndef ADDONMODULE_H
#define ADDONMODULE_H


/*
 * return addon module name
*/
extern "C" const char * addonModuleGetName();

/*
 * process input data and generate output data by addon
*/
extern "C" const char * addonModuleProcess(const char * );


#endif // ADDONMODULE_H
