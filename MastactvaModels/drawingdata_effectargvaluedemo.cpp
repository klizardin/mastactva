#include "drawingdata_effectargvaluedemo.h"
#include "../MastactvaModels/drawingdata_effectargdemo.h"


DrawingDataEffectArgValue::DrawingDataEffectArgValue(EffectArgValueData &&data_)
    : EffectArgValueData(std::move(data_))
{
    drawingdata::utils::rebuild(m_effectArgsData, static_cast<DrawingDataEffectArg*>(nullptr));
}
