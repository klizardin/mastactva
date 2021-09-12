#include "drawingdata_effectargset.h"
#include "../MastactvaModels/drawingdata_effectargvalue.h"


DrawingDataEffectArgSet::DrawingDataEffectArgSet(EffectArgSetData &&data_)
    : EffectArgSetData(std::move(data_))
{
    drawingdata::utils::rebuild(m_effectArgValuesData, static_cast<DrawingDataEffectArgValue *>(nullptr));
}
