#ifndef DRAWING_TESTS_H
#define DRAWING_TESTS_H


#include "../MastactvaBase/quizimageopengldrawingdata.h"


namespace test
{
void createGeometry(
        std::vector<QVector3D> &vertices,
        std::vector<QVector3D> &normals
        );
}

namespace drawing_data
{
    class TestMinimalDrawQuizImageObject : public IDefaultData<QuizImageObjects>
    {
    public:
        virtual void initialize(QuizImageObjects &data_) const override;
    };

    class TestMinimal2PassDrawQuizImageObject : public IDefaultData<QuizImageObjects>
    {
    public:
        virtual void initialize(QuizImageObjects &data_) const override;
    };

    class Test0QuizImageObject : public IDefaultData<QuizImageObjects>
    {
    public:
        virtual void initialize(QuizImageObjects &data_) const override;
    };

    class Test1QuizImageObject : public IDefaultData<QuizImageObjects>
    {
    public:
        virtual void initialize(QuizImageObjects &data_) const override;
    };

    class Test2QuizImageObject : public IDefaultData<QuizImageObjects>
    {
    public:
        virtual void initialize(QuizImageObjects &data_) const override;
    };

    class Test3QuizImageObject : public IDefaultData<QuizImageObjects>
    {
    public:
        virtual void initialize(QuizImageObjects &data_) const override;
    };
}

//using TestCaseInitializer = drawing_data::Test3QuizImageObject;


#endif // DRAWING_TESTS_H
