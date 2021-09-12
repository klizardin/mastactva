#include "drawingdata_effectarg.h"


DrawingDataEffectArg::DrawingDataEffectArg(EffectArgData &&data_)
    : EffectArgData(std::move(data_))
{

}
