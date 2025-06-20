#include "test/test.hpp"
#include "test/assert/assert.hpp"
#include "WebServer/Client/Request/RequestParser/RequestParser.hpp"

namespace
{
    RequestLineParams_t requestLine;
}

static RequestLineParams_t createFromValidRequestLine(const std::string &requestLineString)
{
    RequestTokenizer requestTokenizer(requestLineString);
    RequestParser sut(requestTokenizer);

    Result<RequestLineParams_t> result = sut.parseRequestLine();

    return result.getValue();
}

static void assertRequestLine(method_t method, const std::string &target, const std::string &version)
{
    ASSERT_EQUALS(method, requestLine.method);
    ASSERT_EQUALS(target, requestLine.target);
    ASSERT_EQUALS(version, requestLine.httpVersion);
}

static void assertRequestLineIsInvalid(const std::string &invalidRequestString, const std::string &errorMessage)
{
    RequestTokenizer requestTokenizer(invalidRequestString);
    RequestParser sut(requestTokenizer);

    Result<RequestLineParams_t> result = sut.parseRequestLine();

    ASSERT_TRUE(result.isFailure());
    ASSERT_EQUALS(errorMessage, result.getError());
}

TEST(recognize_a_basic_GET_request_line)
{
    requestLine = createFromValidRequestLine("GET / HTTP/1.1");

    assertRequestLine(GET, "/", "HTTP/1.1");
}

TEST(recognize_a_basic_POST_request_line)
{
    requestLine = createFromValidRequestLine("POST / HTTP/1.1");

    assertRequestLine(POST, "/", "HTTP/1.1");
}

TEST(recognize_a_basic_DELETE_request_line)
{
    requestLine = createFromValidRequestLine("DELETE / HTTP/1.1");

    assertRequestLine(DELETE, "/", "HTTP/1.1");
}

TEST(take_as_failure_a_case_insensitive_method)
{
    assertRequestLineIsInvalid("get / HTTP/1.1", "501 Not Implemented");
}

TEST(take_as_failure_a_not_implemented_method_consisted_only_of_alphabetic_characters)
{
    assertRequestLineIsInvalid("INVALID / HTTP/1.1", "501 Not Implemented");
}

TEST(take_as_failure_a_not_implemented_method_consisted_of_tchars)
{
    const std::string tchars = "!#$%&'*+-.^_`|~0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    const std::string invalidRequestLineString = tchars + " / HTTP/1.1";

    assertRequestLineIsInvalid(invalidRequestLineString, "501 Not Implemented");
}

TEST(take_as_failure_a_method_consisted_of_invalid_characters)
{
    assertRequestLineIsInvalid("(),/:;\r<=>\b?@\f[\\]\t{}\n / HTTP/1.1", "400 Bad Request");
}

TEST(take_as_failure_a_method_that_contains_invalid_characters)
{
    assertRequestLineIsInvalid("DEL(),/:;\r<=>\b?@\f[\\]\t{}\nTE / HTTP/1.1", "400 Bad Request");
}

TEST(take_as_failure_an_empty_method)
{
    assertRequestLineIsInvalid(" / HTTP/1.1", "400 Bad Request");
}

TEST(take_as_failure_a_non_existant_method)
{
    assertRequestLineIsInvalid("/ HTTP/1.1", "400 Bad Request");
}

TEST(take_as_failure_a_request_preceded_by_SP)
{
    assertRequestLineIsInvalid(" GET / HTTP/1.1", "400 Bad Request");
}

TEST(take_as_failure_a_request_followed_by_SP)
{
    assertRequestLineIsInvalid("GET / HTTP/1.1 ", "400 Bad Request");
}
