#ifndef LOCALDATA_H
#define LOCALDATA_H


#include "../MastactvaBase/localdataapinocache.h"
#include "../MastactvaBase/localdataapicache.h"


#if defined(LOCALDATAAPICACHE)
using LocalDataAPI = LocalDataAPICache;
#else
using LocalDataAPI = LocalDataAPINoCache;
#endif
/*
 * NB
 *
 * please use following:
 *
 * class LocalDataAPINoCache;
 * using LocalDataAPI = LocalDataAPINoCache;
 *
 * not this:
 *
 * class LocalDataAPI;
 *
*/


#endif // LOCALDATA_H
