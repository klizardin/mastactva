#ifndef DRAWINGDATA_EFFECTARG_H
#define DRAWINGDATA_EFFECTARG_H


#include "../MastactvaModels/effectarg_data.h"
#include "../MastactvaBase/drawingdata_utils.h"


class DrawingDataEffectArg :
        public EffectArgData
{
public:
    DrawingDataEffectArg() = default;
    DrawingDataEffectArg(EffectArgData &&data_);
};


#endif // DRAWINGDATA_EFFECTARG_H
