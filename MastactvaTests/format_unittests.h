#ifndef FORMAT_UNITTESTS_H
#define FORMAT_UNITTESTS_H


#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <QString>
#include <QVariant>
#include <QStringList>
#include <QList>
#include <QVector>
#include "../MastactvaBase/format.h"
#include "test_utils.h"


using namespace testing;


TEST(Format, toString_simpleTypes)
{
    ASSERT_TRUE(equal(fmt::toString(0), sum("0")));
    ASSERT_TRUE(equal(fmt::toString(1), sum("1")));
    ASSERT_TRUE(equal(fmt::toString(QString("2")), sum("2")));
    ASSERT_TRUE(equal(fmt::toString(2.5), sum("2.5")));
}

TEST(Format, toString_stringConverteble)
{
    ASSERT_TRUE(equal(fmt::toString("2"), sum("2")));
}

class Name : protected QString
{
public:
    using QString::QString;

    QString toString() const
    {
        return *this;
    }
};

TEST(Format, toString_compound)
{
    ASSERT_TRUE(equal(fmt::toString(Name("str")), sum("str")));
}

class NameQString : public QString
{
public:
    using QString::QString;
};

TEST(Format, toString_compoundWithQStringConversion)
{
    ASSERT_TRUE(equal(fmt::toString(NameQString("str")), sum("str")));
}

TEST(Format, classFormat)
{
    ASSERT_TRUE(equal(
                    fmt::toString(fmt::Private::Format<>("str")),
                    sum("str")
                    )
                );
    ASSERT_TRUE(equal(
                    fmt::toString(fmt::format("str1")),
                    sum("str1")
                    )
                );
}

TEST(Format, formatArguments)
{
    ASSERT_TRUE(equal(
                    fmt::toString(fmt::format("%1=%2", 1, "1")),
                    sum("1", "=" "1")
                    )
                );
    ASSERT_TRUE(equal(
                    fmt::toString(fmt::format("%1=%2", Name("a"), NameQString("2"))),
                    sum("a", "=" "2")
                    )
                );
}

TEST(Format, classList)
{
    ASSERT_TRUE(equal(
                    fmt::toString(fmt::Private::List<int, QVector>({1,2,3}, ",")),
                    sum("1", "," "2", ",", "3")
                    )
                );
    ASSERT_TRUE(equal(
                    fmt::toString(fmt::list(QVector<int>({1,2,3}), " , ")),
                    sum("1", " , " "2", " , ", "3")
                    )
                );
    ASSERT_TRUE(equal(
                    fmt::toString(fmt::list(QVector<float>({1.0,2.5,3.5}), " , ")),
                    sum("1", " , " "2.5", " , ", "3.5")
                    )
                );
}

namespace fmt
{

template<> inline
int toType(const Name &val_, const int &)
{
    return int(QVariant::fromValue(val_.toString()).toDouble());
}

template<> inline
float toType(const Name &val_, const float &defaultValue_)
{
    const float conv = QVariant::fromValue(val_.toString()).toDouble();
    if(conv < 0)
    {
        return defaultValue_;
    }
    else
    {
        return conv;
    }
}

}

TEST(Format, toType)
{
    ASSERT_EQ(fmt::toType(Name("1"), int{2}), 1);
}

TEST(Format, formatList)
{
    ASSERT_TRUE(equal(
                    fmt::toString(
                        fmt::list(
                            fmt::format("%1=int(%2)", int{1}, float{2.1}),
                            QVector<Name>({ "2.0", "12.5", "1.5" }),
                            " , "
                        )
                    ),
                    sum("2=int(2)", " , " "12=int(12.5)", " , ", "1=int(1.5)")
                    )
                );

    ASSERT_TRUE(equal(
                    fmt::toString(
                        fmt::list(
                            fmt::format("%1=f(%2)", int{2}, float{3.5}),
                            QVector<Name>({ "2.0", "-12.5", "1.5" }),
                            " , "
                        )
                    ),
                    sum("2=f(2)", " , " "-12=f(3.5)", " , ", "1=f(1.5)")
                    )
                );
}

using IntAndFloatPair = QPair<int, float>;

namespace fmt
{

template<> inline
IntAndFloatPair toType(const Name &val_, const IntAndFloatPair &)
{
    const float v = QVariant::fromValue(val_.toString()).toDouble();
    return IntAndFloatPair{int(v), v};
}

}

TEST(Format, toTypeList)
{
    const QList<Name> values({{"1"}, {"2"}});
    const QList<int> result({1, 2});
    ASSERT_EQ(fmt::toType(values, int{2}), result);

    const QVector<Name> values1({{"3"}, {"4"}});
    const QVector<int> result1({3, 4});
    ASSERT_EQ(fmt::toType(values1, int{2}), result1);

    const QList<Name> values2({{"1.5"}, {"2.1"}});
    const QList<IntAndFloatPair> result2({{1, 1.5}, {2, 2.1}});
    ASSERT_EQ(fmt::toType(values2, IntAndFloatPair{0, 0.0}), result2);
}

namespace fmt
{

template<> inline
Name toType(const QString &val_, const Name &)
{
    return Name(val_.toUtf8());
}

}


TEST(Format, formatListToType)
{
    ASSERT_TRUE(equal(
                    fmt::toString(
                        fmt::list(
                            fmt::format("%1=f(%2)", int{2}, float{3.5}),
                            fmt::toType(QStringList({ "2.0", "-12.5", "1.5" }), Name{}),
                            " , "
                        )
                    ),
                    sum("2=f(2)", " , " "-12=f(3.5)", " , ", "1=f(1.5)")
                    )
                );
    ASSERT_TRUE(equal(
                    fmt::toString(
                        fmt::list(
                            fmt::format("%1=f(%2)", int{2}, float{3.5}),
                            fmt::toTypeList(Name{}, QStringList({ "2.0", "-12.5", "1.5" })),
                            " , "
                        )
                    ),
                    sum("2=f(2)", " , " "-12=f(3.5)", " , ", "1=f(1.5)")
                    )
                );
}


TEST(Format, formatListToStringList)
{
    ASSERT_EQ(
                fmt::list(
                    fmt::format("%1=f(%2)", int{2}, float{3.5}),
                    fmt::toTypeList(Name{}, QStringList({ "2.0", "-12.5", "1.5" })),
                    QString()
                ).toStringList(),
                QStringList({"2=f(2)", "-12=f(3.5)", "1=f(1.5)"})
                );
    ASSERT_EQ(fmt::toTypeList(
                    QString{},
                    fmt::list(
                        fmt::format("%1=f(%2)", int{2}, float{3.5}),
                        fmt::toTypeList(Name{}, QStringList({ "2.0", "-12.5", "1.5" })),
                        QString()
                    ).toStringList()
                ),
                QList<QString>({"2=f(2)", "-12=f(3.5)", "1=f(1.5)"})
                );
}

TEST(Format, formatListToMerge)
{
    ASSERT_EQ(
                fmt::merge(
                    fmt::toTypeList(
                        QString{},
                        fmt::list(
                            fmt::format("%1=f(%2)", int{2}, float{3.5}),
                            fmt::toTypeList(Name{}, QStringList({ "2.0", "-12.5", "1.5" })),
                            QString()
                        ).toStringList()
                    ),
                    fmt::toTypeList(
                        QString{},
                        fmt::list(
                            fmt::format("%1=f(%2)", int{2}, float{3.5}),
                            fmt::toTypeList(Name{}, QStringList({ "2.0", "-12.5", "1.5" })),
                            QString()
                        ).toStringList()
                    )
                ),
              QList<QString>({"2=f(2)", "-12=f(3.5)", "1=f(1.5)", "2=f(2)", "-12=f(3.5)", "1=f(1.5)"})
        );
    ASSERT_EQ(
                fmt::merge(
                    fmt::toTypeList(
                        QString{},
                        fmt::list(
                            fmt::format("%1=f(%2)", int{2}, float{3.5}),
                            fmt::toTypeList(Name{}, QStringList({ "2.0", "-12.5", "1.5" })),
                            QString()
                        )
                    ),
                    fmt::toTypeList(
                        QString{},
                        fmt::list(
                            fmt::format("%1=f(%2)", int{2}, float{3.5}),
                            fmt::toTypeList(Name{}, QStringList({ "2.0", "-12.5", "1.5" })),
                            QString()
                        )
                    )
                ),
              QList<QString>({"2=f(2)", "-12=f(3.5)", "1=f(1.5)", "2=f(2)", "-12=f(3.5)", "1=f(1.5)"})
        );
}

TEST(Format, formatListConstant)
{
    const auto fmtlist = fmt::list(
        fmt::format("%1=f(%3) %2", int{2}, fmt::constant(int{10}), float{3.5}),
        fmt::toTypeList(Name{}, QStringList({ "2.0", "-12.5", "1.5" })),
        " , "
    );
    ASSERT_TRUE(equal(
                    fmt::toString(fmtlist),
                    sum("2=f(2) 10", " , " "-12=f(3.5) 10", " , ", "1=f(1.5) 10")
                    )
                );
    ASSERT_TRUE(equal(
                    fmtlist,
                    sum("2=f(2) 10", " , " "-12=f(3.5) 10", " , ", "1=f(1.5) 10")
                    )
                );
}

TEST(Format, sum)
{
    const auto fmtsum = fmt::sum(
                Name{"2.0"},
                fmt::constant(int{10}),
                float{3.5}
            );
    ASSERT_TRUE(equal(
                    fmt::toString(fmtsum),
                    sum("2.0", "10", "3.5")
                    )
                );
    ASSERT_TRUE(equal(
                    fmtsum,
                    sum("2.0", "10", "3.5")
                    )
                );
}


#endif // FORMAT_UNITTESTS_H
