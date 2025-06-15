#include "test/test.hpp"
#include "test/assert/assert.hpp"
#include "utils/Result/Result.hpp"

#define ANY_VALUE 5

TEST(create_successful_result)
{
    std::string expectedError = std::string("");
    int expectedValue = ANY_VALUE;

    Result<int> sut = Result<int>::ok(expectedValue);

    ASSERT_TRUE(sut.isSuccess());
    ASSERT_FALSE(sut.isFailure());
    ASSERT_EQUALS(expectedError, sut.getError());
    ASSERT_EQUALS(expectedValue, sut.getValue());
}

TEST(create_failed_result)
{
    std::string expectedError = std::string("any error");

    Result<int> sut = Result<int>::fail(expectedError);

    ASSERT_FALSE(sut.isSuccess());
    ASSERT_TRUE(sut.isFailure());
    ASSERT_EQUALS(expectedError, sut.getError());
}
