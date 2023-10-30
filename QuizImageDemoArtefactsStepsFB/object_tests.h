/*
    Copyright 2021

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef OBJECT_TESTS_H
#define OBJECT_TESTS_H


#include "../MastactvaBase/drawingdata_utils.h"
#include "../MastactvaBase/quizimagedrawingdata.h"
#include "../MastactvaBase/utils.h"


/*
 * simple IFileSource implementation
 * (for test purpose)
*/
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


/*
 * DrawingDataEffect tests
 * for testing pipeline of initialization of the QuizImageObjects with DrawingDataEffect data
 * (data from the storage)
*/
namespace drawing_objects
{
    class BaseTest : public IDefaultData<drawing_data::QuizImageObjects>
    {
    public:
        void initialize(drawing_data::QuizImageObjects &data_, int argsSetIndex_ = 0) const override;
        std::pair<const char *, const char *> getDescription() const override;
    };

    class MultipleObjectsTest : public IDefaultData<drawing_data::QuizImageObjects>
    {
    public:
        void initialize(drawing_data::QuizImageObjects &data_, int argsSetIndex_ = 0) const override;
        std::pair<const char *, const char *> getDescription() const override;
    };

    class DefaultTest : public IDefaultData<drawing_data::QuizImageObjects>
    {
    public:
        void initialize(drawing_data::QuizImageObjects &data_, int argsSetIndex_ = 0) const override;
        std::pair<const char *, const char *> getDescription() const override;
    };

    class DataTestBase : public IDefaultData<drawing_data::QuizImageObjects>
    {
    public:
        void initialize(drawing_data::QuizImageObjects &data_, int argsSetIndex_ = 0) const override;
        std::pair<const char *, const char *> getDescription() const override;
    };

    class DataTestPosition : public IDefaultData<drawing_data::QuizImageObjects>
    {
    public:
        void initialize(drawing_data::QuizImageObjects &data_, int argsSetIndex_ = 0) const override;
        std::pair<const char *, const char *> getDescription() const override;
    };

    class DataTestObjectsList : public IDefaultData<drawing_data::QuizImageObjects>
    {
    public:
        void initialize(drawing_data::QuizImageObjects &data_, int argsSetIndex_ = 0) const override;
        std::pair<const char *, const char *> getDescription() const override;
    };

    class DataTest3DObjectSwift : public IDefaultData<drawing_data::QuizImageObjects>
    {
    public:
        void initialize(drawing_data::QuizImageObjects &data_, int argsSetIndex_ = 0) const override;
        std::pair<const char *, const char *> getDescription() const override;
    };

    class DataTest3DObjectCube : public IDefaultData<drawing_data::QuizImageObjects>
    {
    public:
        void initialize(drawing_data::QuizImageObjects &data_, int argsSetIndex_ = 0) const override;
        std::pair<const char *, const char *> getDescription() const override;
    };

    class DataTestAlias : public IDefaultData<drawing_data::QuizImageObjects>
    {
    public:
        void initialize(drawing_data::QuizImageObjects &data_, int argsSetIndex_ = 0) const override;
        std::pair<const char *, const char *> getDescription() const override;
    };

    class LuaScriptTestNewVariable : public IDefaultData<drawing_data::QuizImageObjects>
    {
    public:
        void initialize(drawing_data::QuizImageObjects &data_, int argsSetIndex_ = 0) const override;
        std::pair<const char *, const char *> getDescription() const override;
    };

    class LuaScriptTestSetVariable : public IDefaultData<drawing_data::QuizImageObjects>
    {
    public:
        void initialize(drawing_data::QuizImageObjects &data_, int argsSetIndex_ = 0) const override;
        std::pair<const char *, const char *> getDescription() const override;
    };

    class LuaScriptTestRuntime : public IDefaultData<drawing_data::QuizImageObjects>
    {
    public:
        void initialize(drawing_data::QuizImageObjects &data_, int argsSetIndex_ = 0) const override;
        std::pair<const char *, const char *> getDescription() const override;
    };

    class LuaScriptArgTest0 : public IDefaultData<drawing_data::QuizImageObjects>
    {
    public:
        void initialize(drawing_data::QuizImageObjects &data_, int argsSetIndex_ = 0) const override;
        std::pair<const char *, const char *> getDescription() const override;
    };

    class LuaScriptArgTest1 : public IDefaultData<drawing_data::QuizImageObjects>
    {
    public:
        void initialize(drawing_data::QuizImageObjects &data_, int argsSetIndex_ = 0) const override;
        std::pair<const char *, const char *> getDescription() const override;
    };

    class ArgSetBaseTest : public IDefaultData<drawing_data::QuizImageObjects>
    {
    public:
        void initialize(drawing_data::QuizImageObjects &data_, int argsSetIndex_ = 0) const override;
        std::pair<const char *, const char *> getDescription() const override;
    };

    class AlphaBlendingMultipleObjectsTest : public IDefaultData<drawing_data::QuizImageObjects>
    {
    public:
        void initialize(drawing_data::QuizImageObjects &data_, int argsSetIndex_ = 0) const override;
        std::pair<const char *, const char *> getDescription() const override;
    };

    class WalkEffectTest : public IDefaultData<drawing_data::QuizImageObjects>
    {
    public:
        void initialize(drawing_data::QuizImageObjects &data_, int argsSetIndex_ = 0) const override;
        std::pair<const char *, const char *> getDescription() const override;
    };

    class WalkEffectMultiTextureStepsTest : public IDefaultData<drawing_data::QuizImageObjects>
    {
    public:
        void initialize(drawing_data::QuizImageObjects &data_, int argsSetIndex_ = 0) const override;
        std::pair<const char *, const char *> getDescription() const override;
    };

    class WalkEffectDrawingBufferTest : public IDefaultData<drawing_data::QuizImageObjects>
    {
    public:
        void initialize(drawing_data::QuizImageObjects &data_, int argsSetIndex_ = 0) const override;
        std::pair<const char *, const char *> getDescription() const override;
    };
}

// using TestCaseInitializer = drawing_objects::BaseTest;
// using TestCaseInitializer = drawing_objects::MultipleObjectsTest;
// using TestCaseInitializer = drawing_objects::DefaultTest;
// using TestCaseInitializer = drawing_objects::DataTestBase;
// using TestCaseInitializer = drawing_objects::DataTestPosition;
// using TestCaseInitializer = drawing_objects::DataTestObjectsList;
// using TestCaseInitializer = drawing_objects::DataTest3DObjectSwift;
// using TestCaseInitializer = drawing_objects::DataTest3DObjectCube; // do not work
// using TestCaseInitializer = drawing_objects::DataTestAlias;
// using TestCaseInitializer = drawing_objects::LuaScriptTestNewVariable;
// using TestCaseInitializer = drawing_objects::LuaScriptTestSetVariable;
// using TestCaseInitializer = drawing_objects::LuaScriptTestRuntime;
// using TestCaseInitializer = drawing_objects::LuaScriptArgTest0;
// using TestCaseInitializer = drawing_objects::LuaScriptArgTest1;
// using TestCaseInitializer = drawing_objects::ArgSetBaseTest;
// using TestCaseInitializer = drawing_objects::AlphaBlendingMultipleObjectsTest;
// using TestCaseInitializer = drawing_objects::WalkEffectTest;
using TestCaseInitializer = drawing_objects::WalkEffectDrawingBufferTest;
// using TestCaseInitializer = drawing_objects::WalkEffectMultiTextureStepsTest;


#endif // OBJECT_TESTS_H
