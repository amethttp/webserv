#include "test/test.hpp"
#include "test/assert/assert.hpp"

TEST(TestAddition)
{
    int result = 2 + 2;
    ASSERT_EQUALS(5, result);
}

TEST(TestBoolean)
{
    bool value = false;
    ASSERT_TRUE(value);
}

TEST(TestPass)
{
    ASSERT_TRUE(10 > 1);
}
