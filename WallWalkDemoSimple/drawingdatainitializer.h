#ifndef DRAWINGDATAINITIALIZER_H
#define DRAWINGDATAINITIALIZER_H


#include <memory>
#include "../MastactvaBase/drawingdata_utilsdemo.h"
#include "../MastactvaBase/quizimagedrawingdatademo.h"
#include "../MastactvaBase/utils.h"
#include "drawingdatainitializer_utils.h"


namespace drawing_objects
{
    class WalkEffectOnePass : public IDefaultData<drawing_data::QuizImageObjects>
    {
    public:
        WalkEffectOnePass();
        void initialize(drawing_data::QuizImageObjects &data_, int argsSetIndex_ = 0) const override;
        std::pair<const char *, const char *> getDescription() const override;

    private:
        std::shared_ptr<MapFileSource> m_filesource;
        const QPair<QString,QString> m_filenames;
        const QPair<std::vector<QVector4D>, std::vector<QVector4D>> m_coordinates;
    };
}

using DrawingDataInitializer = drawing_objects::WalkEffectOnePass;


#endif // DRAWINGDATAINITIALIZER_H
