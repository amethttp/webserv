#include "test/test.hpp"
#include "test/assert/assert.hpp"
#include "utils/Result/Result.hpp"

#define ANY_VALUE 5

TEST(create_successful_result)
{
    t_httpCode expectedError = 0;
    int expectedValue = ANY_VALUE;

    Result<int> sut = Result<int>::ok(expectedValue);

    ASSERT_TRUE(sut.isSuccess());
    ASSERT_FALSE(sut.isFailure());
    ASSERT_EQUALS(expectedError, sut.getError());
    ASSERT_EQUALS(expectedValue, sut.getValue());
}

TEST(create_failed_result)
{
    t_httpCode expectedError = BAD_REQUEST;

    Result<int> sut = Result<int>::fail(expectedError);

    ASSERT_FALSE(sut.isSuccess());
    ASSERT_TRUE(sut.isFailure());
    ASSERT_EQUALS(expectedError, sut.getError());
}

TEST(throw_invalid_argument_exception_when_creating_failed_result_with_error_code_equals_to_zero)
{
    t_httpCode invalidError = 0;

    ASSERT_THROWS(Result<int>::fail(invalidError), std::invalid_argument);
}

TEST(throw_logic_error_exception_when_accessing_value_in_a_failed_result)
{
    t_httpCode anyError = BAD_REQUEST;

    Result<int> sut = Result<int>::fail(anyError);

    ASSERT_THROWS(sut.getValue(), std::logic_error);
}
