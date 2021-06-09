#ifndef OBJECT_TESTS_H
#define OBJECT_TESTS_H


#include "../MastactvaBase/drawingdata_utils.h"
#include "../MastactvaBase/quizimagedrawingdata.h"
#include "../MastactvaBase/utils.h"


class MapFileSource : public drawingdata::IFileSource
{
public:
    MapFileSource() = default;
    ~MapFileSource() override = default;

    void add(const QString &filename_, const QString &text_);
    void add(const QString &filename_, const QImage &image_);

    QString getText(const FileSource &filename_) const override;
    QImage getImage(const FileSource &filename_) const override;

private:
    QHash<QString, QString> m_texts;
    QHash<QString, QImage> m_images;
};


namespace drawing_objects
{
    class BaseTest : public IDefaultData<drawing_data::QuizImageObjects>
    {
    public:
        void initialize(drawing_data::QuizImageObjects &data_) const override;
    };
}


//using TestCaseInitializer = drawing_objects::BaseTest;


#endif // OBJECT_TESTS_H
