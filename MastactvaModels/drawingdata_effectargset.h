#ifndef DRAWINGDATA_EFFECTARGSET_H
#define DRAWINGDATA_EFFECTARGSET_H


#include "../MastactvaModels/effectargset_data.h"
#include "../MastactvaBase/drawingdata_utils.h"
#include "../MastactvaModels/drawingdata_effectarg.h"


/*
 * it is used only as data container
*/
class DrawingDataEffectArgSet :
        public EffectArgSetData
{
public:
    DrawingDataEffectArgSet() = default;
    DrawingDataEffectArgSet(EffectArgSetData &&data_);
};


#endif // DRAWINGDATA_EFFECTARGSET_H
