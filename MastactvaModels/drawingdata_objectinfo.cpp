#include "drawingdata_objectinfo.h"


DrawingDataObjectInfo::DrawingDataObjectInfo(ObjectInfoData &&data_)
    : ObjectInfoData(std::move(data_))
{
}

ObjectInfoData * DrawingDataObjectInfo::getData()
{
    return this;
}
