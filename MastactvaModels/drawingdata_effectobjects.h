#ifndef DRAWINGDATA_EFFECTOBJECTS_H
#define DRAWINGDATA_EFFECTOBJECTS_H


#include <memory>
#include "../MastactvaModels/effectobjects_data.h"
#include "../MastactvaBase/utils.h"


class DrawingDataEffectObjects :
        public EffectObjectsData
{
public:
    DrawingDataEffectObjects() = default;
    DrawingDataEffectObjects(EffectObjectsData &&data_);

    void addObjects(drawing_data::QuizImageObjects &data_, const drawingdata::Details &details_) const;
    bool isMain() const;

private:
    void setupPosition(const drawingdata::Details &details_) const;
};


#endif // DRAWINGDATA_EFFECTOBJECTS_H
