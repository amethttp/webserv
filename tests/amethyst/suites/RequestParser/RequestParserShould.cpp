#include "test/test.hpp"
#include "test/assert/assert.hpp"
#include "WebServer/Client/Request/RequestParser/RequestParser.hpp"

namespace
{
    RequestParser sut;
    RequestLineParams_t requestLineParams;
}

static RequestLineParams_t createFromValidRequestLine(const std::string &requestLine)
{
    Result<RequestLineParams_t> result = sut.parseRequestLine(requestLine);

    return result.getValue();
}

static void assertRequestLineIsInvalid(const std::string &requestLine, const std::string &errorMessage)
{
    Result<RequestLineParams_t> result = sut.parseRequestLine(requestLine);

    ASSERT_TRUE(result.isFailure());
    ASSERT_EQUALS(errorMessage, result.getError());
}

static void assertRequestLine(method_t method, const std::string &target, const std::string &version)
{
    ASSERT_EQUALS(method, requestLineParams.method);
    ASSERT_EQUALS(target, requestLineParams.target);
    ASSERT_EQUALS(version, requestLineParams.httpVersion);
}

TEST(parse_basic_GET_request_line)
{
    requestLineParams = createFromValidRequestLine("GET / HTTP/1.1");

    assertRequestLine(GET, "/", "HTTP/1.1");
}

TEST(parse_basic_POST_request_line)
{
    requestLineParams = createFromValidRequestLine("POST / HTTP/1.1");

    assertRequestLine(POST, "/", "HTTP/1.1");
}

TEST(parse_basic_DELETE_request_line)
{
    requestLineParams = createFromValidRequestLine("DELETE / HTTP/1.1");

    assertRequestLine(DELETE, "/", "HTTP/1.1");
}

TEST(take_as_failure_a_request_line_with_invalid_method)
{
    assertRequestLineIsInvalid("INVALID / HTTP/1.1", "501 Not Implemented");
}

TEST(take_as_failure_a_request_line_with_empty_method)
{
    assertRequestLineIsInvalid(" / HTTP/1.1", "400 Bad Request");
}

TEST(take_as_failure_a_request_line_without_method)
{
    assertRequestLineIsInvalid("/ HTTP/1.1", "400 Bad Request");
}
