#include "drawingdata_effectargdemo.h"


DrawingDataEffectArg::DrawingDataEffectArg(EffectArgData &&data_)
    : EffectArgData(std::move(data_))
{

}
