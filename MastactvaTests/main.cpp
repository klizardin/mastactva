#include "tst_testcanasserttrue.h"
#include "dbutils_unittests.h"
#include "format_unittests.h"

#include <gtest/gtest.h>

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
