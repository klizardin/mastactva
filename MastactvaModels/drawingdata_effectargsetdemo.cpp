#include "drawingdata_effectargsetdemo.h"
#include "../MastactvaModels/drawingdata_effectargvaluedemo.h"


DrawingDataEffectArgSet::DrawingDataEffectArgSet(EffectArgSetData &&data_)
    : EffectArgSetData(std::move(data_))
{
    drawingdata::utils::rebuild(m_effectArgValuesData, static_cast<DrawingDataEffectArgValue *>(nullptr));
}
