#include "test/test.hpp"
#include "test/assert/assert.hpp"
#include "WebServer/Client/Request/RequestParser/RequestParser.hpp"

TEST(parse_basic_GET_request_line)
{
    Request_t request;
    RequestParser sut;

    SimpleResult result = sut.parseRequestLine(request, "GET / HTTP/1.1");

    ASSERT_TRUE(result.isSuccess());
    ASSERT_EQUALS(GET, request.method);
    ASSERT_EQUALS("/", request.target);
    ASSERT_EQUALS("HTTP/1.1", request.httpVersion);
}

TEST(parse_basic_POST_request_line)
{
    Request_t request;
    RequestParser sut;

    SimpleResult result = sut.parseRequestLine(request, "POST / HTTP/1.1");

    ASSERT_TRUE(result.isSuccess());
    ASSERT_EQUALS(POST, request.method);
    ASSERT_EQUALS("/", request.target);
    ASSERT_EQUALS("HTTP/1.1", request.httpVersion);
}
