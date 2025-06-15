#include "test/test.hpp"
#include "test/assert/assert.hpp"
#include "WebServer/Client/Request/RequestParser/RequestParser.hpp"

namespace
{
    Request_t request;
}

TEST(parse_basic_GET_request_line)
{
    RequestParser sut;

    SimpleResult result = sut.parseRequestLine(request, "GET / HTTP/1.1");

    ASSERT_TRUE(result.isSuccess());
    ASSERT_EQUALS(GET, request.method);
    ASSERT_EQUALS("/", request.target);
    ASSERT_EQUALS("HTTP/1.1", request.httpVersion);
}

TEST(parse_basic_POST_request_line)
{
    RequestParser sut;

    SimpleResult result = sut.parseRequestLine(request, "POST / HTTP/1.1");

    ASSERT_TRUE(result.isSuccess());
    ASSERT_EQUALS(POST, request.method);
    ASSERT_EQUALS("/", request.target);
    ASSERT_EQUALS("HTTP/1.1", request.httpVersion);
}

TEST(parse_basic_DELETE_request_line)
{
    RequestParser sut;

    SimpleResult result = sut.parseRequestLine(request, "DELETE / HTTP/1.1");

    ASSERT_TRUE(result.isSuccess());
    ASSERT_EQUALS(DELETE, request.method);
    ASSERT_EQUALS("/", request.target);
    ASSERT_EQUALS("HTTP/1.1", request.httpVersion);
}
