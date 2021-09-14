#include "drawingdata_effectargvalue.h"
#include "../MastactvaModels/drawingdata_effectarg.h"


DrawingDataEffectArgValue::DrawingDataEffectArgValue(EffectArgValueData &&data_)
    : EffectArgValueData(std::move(data_))
{
    drawingdata::utils::rebuild(m_effectArgValuesData, m_effectArgData);
}
