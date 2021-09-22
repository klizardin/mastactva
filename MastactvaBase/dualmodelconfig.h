#ifndef DUALMODELCONFIG_H
#define DUALMODELCONFIG_H

#include <type_traits>
#include <QString>
#include <QVariant>
#include <QHash>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/localdata.h"
#include "../MastactvaBase/qmlobjects.h"


class LocalDataAPINoCache;
class LocalDataAPICache;
class RequestData;
#if defined(LOCALDATAAPICACHE)
using LocalDataAPI = LocalDataAPICache;
using LocalDataAPI2 = LocalDataAPINoCache;
#else
using LocalDataAPI = LocalDataAPINoCache;
using LocalDataAPI2 = LocalDataAPICache;
#endif


#endif // DUALMODELCONFIG_H
