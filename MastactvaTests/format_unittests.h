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


using namespace testing;


TEST(Format, test0)
{
    ASSERT_STRCASEEQ(fmt::toString(0).toUtf8().constData(), "0");
}


#endif // FORMAT_UNITTESTS_H
