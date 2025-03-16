#include "drawingdatainitializer.h"


void WalkEffectOnePassTest::initialize(
        drawing_data::QuizImageObjects &data_,
        int argsSetIndex_ /*= 0*/
        ) const
{
    Q_UNUSED(argsSetIndex_);
    Q_UNUSED(data_);

    auto filesource = createMapFileSource();
    auto effectObjectsData = createWalkEffectOnePassTestData();
    auto drawingDataEffect = std::make_unique<::DrawingDataEffect>(std::move(*effectObjectsData));
    drawingDataEffect->init(filesource);
    drawingDataEffect->initialize(data_);
}

std::pair<const char *, const char *> WalkEffectOnePassTest::getDescription() const
{
    return {"WalkEffectOnPathTest", "walk effect on path version"};
}

