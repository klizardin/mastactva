#ifndef DEFINE_LUA_TYPES_H
#define DEFINE_LUA_TYPES_H

#include "../MastactvaBase/lua_utils.h"


struct DataTestData
{
    int a = 0;
    double b = 0.0;
    QString c;

    DataTestData(int a_ = 0, double b_ = 0.0, const QString &c_ = QString{})
        : a(a_), b(b_), c(c_)
    {
    }

    QString name() const
    {
        return "DataTestData";
    }

    bool operator == (const DataTestData &data_) const
    {
        return data_.a == a
                && fabs(data_.b - b) <= std::max(fabs(data_.b), fabs(b)) * 1e-6
                && data_.c == c
                ;
    }
};

static const auto g_DataTestDataLayout = makeDataLayout(
            makeFieldLayout("a", &DataTestData::a),
            makeFieldLayout("b", &DataTestData::b),
            makeFieldLayout("c", &DataTestData::c)
            );

DECLARE_DATA_LAYOUT(DataTestData, g_DataTestDataLayout);


class AddonTestData
{
public:
    QString key;
    QString random;

    QString name() const
    {
        return "AddonTestData";
    }

    bool operator == (const AddonTestData &data_) const
    {
        return data_.key == key
                && data_.random == random
                ;
    }
};

static const auto g_AddonTestDataLayout = makeDataLayout(
            makeFieldLayout("key", &AddonTestData::key),
            makeFieldLayout("random", &AddonTestData::random)
            );

DECLARE_DATA_LAYOUT(AddonTestData, g_AddonTestDataLayout);


class AddonTestData2
{
public:
    bool value = false;

    QString name() const
    {
        return "AddonTestData2";
    }

    bool operator == (const AddonTestData2 &data_) const
    {
        return data_.value == value
                ;
    }
};

static const auto g_AddonTestData2Layout = makeDataLayout(
            makeFieldLayout("value", &AddonTestData2::value)
            );

DECLARE_DATA_LAYOUT(AddonTestData2, g_AddonTestData2Layout);


class AddonTestDataComplex
{
public:
    AddonTestData data1;
    AddonTestData2 data2;

    QString name() const
    {
        return "AddonTestDataComplex";
    }

    bool operator == (const AddonTestDataComplex &data_) const
    {
        return data_.data1 == data1
                && data_.data2 == data2
                ;
    }
};

static const auto g_AddonTestDataComplexLayout = makeDataLayout(
            makeFieldLayout("data1", &AddonTestDataComplex::data1),
            makeFieldLayout("data2", &AddonTestDataComplex::data2)
            );

DECLARE_DATA_LAYOUT(AddonTestDataComplex, g_AddonTestDataComplexLayout);


#endif // DEFINE_LUA_TYPES_H
