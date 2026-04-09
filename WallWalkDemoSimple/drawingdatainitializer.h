#ifndef DRAWINGDATAINITIALIZER_H
#define DRAWINGDATAINITIALIZER_H


#include <QDir>
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
        void initData(const QVector<QPair<QString, QString>>& filenames_,
                      const QVector<QPair<std::vector<QVector4D>, std::vector<QVector4D>>>& coordinates_,
                      const QDir& sourceImageDir_);
        void initialize(drawing_data::QuizImageObjects &data_, int argsSetIndex_ = 0) const override;
        std::pair<const char *, const char *> getDescription() const override;

    private:
        QDir m_sourceImageDir;
        std::shared_ptr<MapFileSource> m_filesource;
        QVector<QPair<QString, QString>> m_filenames;
        QVector<QPair<std::vector<QVector4D>, std::vector<QVector4D>>> m_coordinates;
    };
}

using DrawingDataInitializer = drawing_objects::WalkEffectOnePass;


#endif // DRAWINGDATAINITIALIZER_H
