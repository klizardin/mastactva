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
    class TestsBase
    {
    public:
        TestsBase();
        void initData(const QVector<QPair<QString, QString>>& filenames_,
                      const QVector<QPair<std::vector<QVector4D>, std::vector<QVector4D>>>& coordinates_,
                      const QDir& sourceImageDir_);
    protected:
        QDir m_sourceImageDir;
        std::shared_ptr<MapFileSource> m_filesource;
        QVector<QPair<QString, QString>> m_filenames;
        QVector<QPair<std::vector<QVector4D>, std::vector<QVector4D>>> m_coordinates;
    };

    class BaseTest : public IDefaultData<drawing_data::QuizImageObjects>, public TestsBase
    {
    public:
        void initialize(drawing_data::QuizImageObjects &data_, int argsSetIndex_ = 0) const override;
        std::pair<const char *, const char *> getDescription() const override;
    };

    class WalkEffectOnePass : public IDefaultData<drawing_data::QuizImageObjects>, public TestsBase
    {
    public:
        void initialize(drawing_data::QuizImageObjects &data_, int argsSetIndex_ = 0) const override;
        std::pair<const char *, const char *> getDescription() const override;
    };
}

//using DrawingDataInitializer = drawing_objects::WalkEffectOnePass;
using DrawingDataInitializer = drawing_objects::BaseTest;


#endif // DRAWINGDATAINITIALIZER_H
