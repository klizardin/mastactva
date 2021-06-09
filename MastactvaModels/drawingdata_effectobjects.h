#ifndef DRAWINGDATA_EFFECTOBJECTS_H
#define DRAWINGDATA_EFFECTOBJECTS_H


#include "../MastactvaModels/effectobjects_data.h"


class DrawingDataEffectObjects : private EffectObjectsData
{
public:
    DrawingDataEffectObjects() = default;
    DrawingDataEffectObjects(EffectObjectsData &&data_);

    EffectObjectsData * getData();
};


#endif // DRAWINGDATA_EFFECTOBJECTS_H
