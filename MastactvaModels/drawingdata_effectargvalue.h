#ifndef DRAWINGDATA_EFFECTARGVALUE_H
#define DRAWINGDATA_EFFECTARGVALUE_H


#include <memory>
#include "../MastactvaModels/effectargvalue_data.h"
#include "../MastactvaModels/drawingdata_artefactarg.h"
#include "../MastactvaBase/drawingdata_utils.h"


class DrawingDataEffectArgValue :
        public EffectArgValueData
{
public:
    DrawingDataEffectArgValue() = default;
    DrawingDataEffectArgValue(EffectArgValueData &&data_);
};


#endif // DRAWINGDATA_EFFECTARGVALUE_H
