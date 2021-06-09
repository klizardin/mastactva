#ifndef DRAWINGDATA_EFFECTOBJECTS_H
#define DRAWINGDATA_EFFECTOBJECTS_H


#include <memory>
#include "../MastactvaBase/drawingdata_utils.h"
#include "../MastactvaModels/effectobjects_data.h"


class DrawingDataEffectObjects : private EffectObjectsData
{
public:
    DrawingDataEffectObjects() = default;
    DrawingDataEffectObjects(EffectObjectsData &&data_);

    EffectObjectsData * getData();

    void init(std::shared_ptr<drawingdata::IFileSource> filesources_);

private:
    std::shared_ptr<drawingdata::IFileSource> m_filesources;
};


#endif // DRAWINGDATA_EFFECTOBJECTS_H
