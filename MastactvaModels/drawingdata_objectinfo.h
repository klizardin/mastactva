#ifndef DRAWINGDATA_OBJECTINFO_H
#define DRAWINGDATA_OBJECTINFO_H


#include "../MastactvaModels/objectinfo_data.h"


class DrawingDataObjectInfo : public ObjectInfoData
{
public:
    DrawingDataObjectInfo() = default;
    DrawingDataObjectInfo(ObjectInfoData &&data_);
};


#endif // DRAWINGDATA_OBJECTINFO_H
