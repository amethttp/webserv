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

TEST(throw_invalid_argument_exception_when_creating_failed_result_without_error)
{
    std::string invalidError = std::string("");

    ASSERT_THROWS(Result<int>::fail(invalidError), std::invalid_argument);
}
