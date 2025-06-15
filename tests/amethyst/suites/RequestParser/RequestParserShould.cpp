#include "test/test.hpp"
#include "test/assert/assert.hpp"
#include "WebServer/Client/Request/RequestParser/RequestParser.hpp"

namespace
{
    Request_t request;
    RequestParser sut;
}

static void assertRequestLine(method_t method, const std::string &target, const std::string &version)
{
    ASSERT_EQUALS(method, request.method);
    ASSERT_EQUALS(target, request.target);
    ASSERT_EQUALS(version, request.httpVersion);
}

TEST(parse_basic_GET_request_line)
{
    SimpleResult result = sut.parseRequestLine(request, "GET / HTTP/1.1");

    ASSERT_TRUE(result.isSuccess());
    assertRequestLine(GET, "/", "HTTP/1.1");
}

TEST(parse_basic_POST_request_line)
{
    SimpleResult result = sut.parseRequestLine(request, "POST / HTTP/1.1");

    ASSERT_TRUE(result.isSuccess());
    assertRequestLine(POST, "/", "HTTP/1.1");
}

TEST(parse_basic_DELETE_request_line)
{
    SimpleResult result = sut.parseRequestLine(request, "DELETE / HTTP/1.1");

    ASSERT_TRUE(result.isSuccess());
    assertRequestLine(DELETE, "/", "HTTP/1.1");
}
