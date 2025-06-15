#include "test/test.hpp"
#include "test/assert/assert.hpp"
#include "WebServer/Client/Request/RequestParser/RequestParser.hpp"

namespace
{
    Request_t request;
    RequestParser sut;
}

static void parseValidRequestLine(const std::string &requestLine)
{
    sut.parseRequestLine(request, requestLine);
}

static void assertRequestLineIsInvalid(const std::string &requestLine, const std::string &errorMessage)
{
    SimpleResult result = sut.parseRequestLine(request, requestLine);

    ASSERT_TRUE(result.isFailure());
    ASSERT_EQUALS(errorMessage, result.getError());
}

static void assertRequestLine(method_t method, const std::string &target, const std::string &version)
{
    ASSERT_EQUALS(method, request.method);
    ASSERT_EQUALS(target, request.target);
    ASSERT_EQUALS(version, request.httpVersion);
}

TEST(parse_basic_GET_request_line)
{
    parseValidRequestLine("GET / HTTP/1.1");

    assertRequestLine(GET, "/", "HTTP/1.1");
}

TEST(parse_basic_POST_request_line)
{
    parseValidRequestLine("POST / HTTP/1.1");

    assertRequestLine(POST, "/", "HTTP/1.1");
}

TEST(parse_basic_DELETE_request_line)
{
    parseValidRequestLine("DELETE / HTTP/1.1");

    assertRequestLine(DELETE, "/", "HTTP/1.1");
}

TEST(take_as_failure_a_request_line_with_invalid_method)
{
    assertRequestLineIsInvalid("INVALID / HTTP/1.1", "501 Not Implemented");
}
