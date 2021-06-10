#ifndef DRAWINGDATA_EFFECT_H
#define DRAWINGDATA_EFFECT_H


#include "../MastactvaModels/effect_data.h"


class DrawingDataEffect : private EffectData
{
public:
    DrawingDataEffect() = default;
    DrawingDataEffect(EffectData &&data_);

    EffectData * getData();
};


#endif // DRAWINGDATA_EFFECT_H
