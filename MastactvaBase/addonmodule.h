#ifndef ADDONMODULE_H
#define ADDONMODULE_H


/*
 * it is minimal required implementation of addon module in the separate dynamic library
*/

/*
 * return addon module name
*/
extern "C" const char * addonModuleGetName();

/*
 * process input data and generate output data by addon
*/
extern "C" const char * addonModuleProcess(const char * );


#endif // ADDONMODULE_H
