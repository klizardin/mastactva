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

#ifndef DRAWING_TESTS_H
#define DRAWING_TESTS_H


#include "../MastactvaBase/quizimageopengldrawingdata.h"


namespace test
{

void createQTLogoGeometry(
        std::vector<QVector3D> &vertices,
        std::vector<QVector3D> &normals
        );
}

namespace drawing_data
{
    class TestMinimalDrawQTLogoQuizImageObject : public IDefaultData<QuizImageObjects>
    {
    public:
        virtual void initialize(QuizImageObjects &data_, int argsSetIndex_ = 0) const override;
    };

    class TestMinimal2PassDrawQTLogoQuizImageObject : public IDefaultData<QuizImageObjects>
    {
    public:
        virtual void initialize(QuizImageObjects &data_, int argsSetIndex_ = 0) const override;
    };

    class Test0QuizImageObject : public IDefaultData<QuizImageObjects>
    {
    public:
        virtual void initialize(QuizImageObjects &data_, int argsSetIndex_ = 0) const override;
    };

    class Test1QuizImageObject : public IDefaultData<QuizImageObjects>
    {
    public:
        virtual void initialize(QuizImageObjects &data_, int argsSetIndex_ = 0) const override;
    };

    class Test2QuizImageObject : public IDefaultData<QuizImageObjects>
    {
    public:
        virtual void initialize(QuizImageObjects &data_, int argsSetIndex_ = 0) const override;
    };

    class Test3QuizImageObject : public IDefaultData<QuizImageObjects>
    {
    public:
        virtual void initialize(QuizImageObjects &data_, int argsSetIndex_ = 0) const override;
    };

    class Test4QuizImageObject : public IDefaultData<QuizImageObjects>
    {
    public:
        virtual void initialize(QuizImageObjects &data_, int argsSetIndex_ = 0) const override;
    private:
        std::unique_ptr<QuizImageObject> createObjectWithgDefaultShaderAndPeriod(
                double tFrom_,
                double tTo_,
                const std::vector<Texture> &textures_,
                const QString &vertexShaderFileName_,
                const QString &fragmentShaderFileName_
                ) const;
    };
}


// using TestCaseInitializer = drawing_data::TestMinimalDrawQTLogoQuizImageObject;
// using TestCaseInitializer = drawing_data::TestMinimal2PassDrawQTLogoQuizImageObject;
// using TestCaseInitializer = drawing_data::Test0QuizImageObject;
// using TestCaseInitializer = drawing_data::Test1QuizImageObject;
// using TestCaseInitializer = drawing_data::Test2QuizImageObject; // do not work
// using TestCaseInitializer = drawing_data::Test3QuizImageObject;
// using TestCaseInitializer = drawing_data::Test4QuizImageObject;


#endif // DRAWING_TESTS_H
