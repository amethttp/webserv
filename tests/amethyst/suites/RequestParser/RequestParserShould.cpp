#include "test/test.hpp"
#include "test/assert/assert.hpp"
#include "WebServer/Client/Request/RequestParser/RequestParser.hpp"

namespace
{
    Request_t request;
    RequestParser sut;
}

static SimpleResult tryParseRequestLine(const std::string &requestLine)
{
    return sut.parseRequestLine(request, requestLine);
}

static void assertSuccessfulResult(SimpleResult &result)
{
    ASSERT_TRUE(result.isSuccess());
}

static void assertRequestLine(method_t method, const std::string &target, const std::string &version)
{
    ASSERT_EQUALS(method, request.method);
    ASSERT_EQUALS(target, request.target);
    ASSERT_EQUALS(version, request.httpVersion);
}

TEST(parse_basic_GET_request_line)
{
    SimpleResult result = tryParseRequestLine("GET / HTTP/1.1");

    assertSuccessfulResult(result);
    assertRequestLine(GET, "/", "HTTP/1.1");
}

TEST(parse_basic_POST_request_line)
{
    SimpleResult result = tryParseRequestLine("POST / HTTP/1.1");

    assertSuccessfulResult(result);
    assertRequestLine(POST, "/", "HTTP/1.1");
}

TEST(parse_basic_DELETE_request_line)
{
    SimpleResult result = tryParseRequestLine("DELETE / HTTP/1.1");

    assertSuccessfulResult(result);
    assertRequestLine(DELETE, "/", "HTTP/1.1");
}

TEST(take_as_failure_a_request_line_with_invalid_method)
{
    SimpleResult result = tryParseRequestLine("INVALID / HTTP/1.1");

    ASSERT_TRUE(result.isFailure());
}
