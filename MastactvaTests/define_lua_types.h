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


#endif // DEFINE_LUA_TYPES_H
