#ifndef DRAWINGDATA_EFFECT_H
#define DRAWINGDATA_EFFECT_H


#include "../MastactvaModels/effect_data.h"
#include "../MastactvaBase/drawingdata_utils.h"
#include "../MastactvaBase/quizimagedrawingdata.h"


class DrawingDataEffect :
        public EffectData,
        public IDefaultData<drawing_data::QuizImageObjects>
{
public:
    DrawingDataEffect() = default;
    DrawingDataEffect(EffectData &&data_);

    // DIP initializer
    void init(std::shared_ptr<drawingdata::IFileSource> filesources_);

    // method to form QuizImageObjects data
    void initialize(drawing_data::QuizImageObjects &data_) const override;

private:
    drawingdata::Details m_details;
};


#endif // DRAWINGDATA_EFFECT_H
