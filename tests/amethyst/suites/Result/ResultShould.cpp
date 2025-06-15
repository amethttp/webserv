#include "test/test.hpp"
#include "test/assert/assert.hpp"
#include "utils/Result/Result.hpp"

TEST(created_with_ok)
{
    Result<int> sut = Result<int>::ok(5);

    ASSERT_TRUE(sut.isSuccess());
    ASSERT_FALSE(sut.isFailure());
    ASSERT_EQUALS(std::string(""), sut.getError());
    ASSERT_EQUALS(5, sut.getValue());
}
