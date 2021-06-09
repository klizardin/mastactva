#ifndef DRAWINGDATA_OBJECTINFO_H
#define DRAWINGDATA_OBJECTINFO_H


#include "../MastactvaModels/objectinfo_data.h"


class DrawingDataObjectInfo : private ObjectInfoData
{
public:
    DrawingDataObjectInfo() = default;
    DrawingDataObjectInfo(ObjectInfoData &&data_);

    ObjectInfoData * getBase();
};


#endif // DRAWINGDATA_OBJECTINFO_H
