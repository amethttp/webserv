#include "test/test.hpp"
#include "test/assert/assert.hpp"
#include "utils/Result/Result.hpp"

TEST(create_succesful_result)
{
    std::string expectedError = std::string("");
    int expectedValue = 5;

    Result<int> sut = Result<int>::ok(expectedValue);

    ASSERT_TRUE(sut.isSuccess());
    ASSERT_FALSE(sut.isFailure());
    ASSERT_EQUALS(expectedError, sut.getError());
    ASSERT_EQUALS(expectedValue, sut.getValue());
}
