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

    class MultipleObjectsTest : public IDefaultData<drawing_data::QuizImageObjects>
    {
    public:
        void initialize(drawing_data::QuizImageObjects &data_) const override;
    };

    class DefaultTest : public IDefaultData<drawing_data::QuizImageObjects>
    {
    public:
        void initialize(drawing_data::QuizImageObjects &data_) const override;
    };

    class DataTestBase : public IDefaultData<drawing_data::QuizImageObjects>
    {
    public:
        void initialize(drawing_data::QuizImageObjects &data_) const override;
    };

    class DataTestPosition : public IDefaultData<drawing_data::QuizImageObjects>
    {
    public:
        void initialize(drawing_data::QuizImageObjects &data_) const override;
    };

    class DataTestObjectsList : public IDefaultData<drawing_data::QuizImageObjects>
    {
    public:
        void initialize(drawing_data::QuizImageObjects &data_) const override;
    };

    class DataTest3DObjectSwift : public IDefaultData<drawing_data::QuizImageObjects>
    {
    public:
        void initialize(drawing_data::QuizImageObjects &data_) const override;
    };

    class DataTest3DObjectCube : public IDefaultData<drawing_data::QuizImageObjects>
    {
    public:
        void initialize(drawing_data::QuizImageObjects &data_) const override;
    };

    class DataTestAlias : public IDefaultData<drawing_data::QuizImageObjects>
    {
    public:
        void initialize(drawing_data::QuizImageObjects &data_) const override;
    };

    class LuaScriptTestNewVariable : public IDefaultData<drawing_data::QuizImageObjects>
    {
    public:
        void initialize(drawing_data::QuizImageObjects &data_) const override;
    };

    class LuaScriptTestSetVariable : public IDefaultData<drawing_data::QuizImageObjects>
    {
    public:
        void initialize(drawing_data::QuizImageObjects &data_) const override;
    };

    class LuaScriptTestRuntime : public IDefaultData<drawing_data::QuizImageObjects>
    {
    public:
        void initialize(drawing_data::QuizImageObjects &data_) const override;
    };
}

// using TestCaseInitializer = drawing_objects::BaseTest;
// using TestCaseInitializer = drawing_objects::MultipleObjectsTest;
// using TestCaseInitializer = drawing_objects::DefaultTest;
// using TestCaseInitializer = drawing_objects::DataTestBase;
// using TestCaseInitializer = drawing_objects::DataTestPosition;
// using TestCaseInitializer = drawing_objects::DataTestObjectsList;
// using TestCaseInitializer = drawing_objects::DataTest3DObjectSwift;
// using TestCaseInitializer = drawing_objects::DataTest3DObjectCube;
// using TestCaseInitializer = drawing_objects::DataTestAlias;
// using TestCaseInitializer = drawing_objects::LuaScriptTestNewVariable;
// using TestCaseInitializer = drawing_objects::LuaScriptTestSetVariable;
using TestCaseInitializer = drawing_objects::LuaScriptTestRuntime;


#endif // OBJECT_TESTS_H
