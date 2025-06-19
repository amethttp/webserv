#include "test/test.hpp"
#include "test/assert/assert.hpp"
#include "WebServer/Client/Request/RequestParser/RequestParser.hpp"

namespace
{
    RequestLineParams_t requestLineParams;
}

static void assertRequestLine(method_t method, const std::string &target, const std::string &version)
{
    ASSERT_EQUALS(method, requestLineParams.method);
    ASSERT_EQUALS(target, requestLineParams.target);
    ASSERT_EQUALS(version, requestLineParams.httpVersion);
}

TEST(should_recognize_a_basic_GET_request_line)
{
    RequestTokenizer requestTokenizer("GET / HTTP/1.1");
    RequestParser sut(requestTokenizer);

    Result<RequestLineParams_t> result = sut.parseRequestLine();
    requestLineParams = result.getValue();

    assertRequestLine(GET, "/", "HTTP/1.1");
}

TEST(should_recognize_a_basic_POST_request_line)
{
    RequestTokenizer requestTokenizer("POST / HTTP/1.1");
    RequestParser sut(requestTokenizer);

    Result<RequestLineParams_t> result = sut.parseRequestLine();
    requestLineParams = result.getValue();

    assertRequestLine(POST, "/", "HTTP/1.1");
}

TEST(should_recognize_a_basic_DELETE_request_line)
{
    RequestTokenizer requestTokenizer("DELETE / HTTP/1.1");
    RequestParser sut(requestTokenizer);

    Result<RequestLineParams_t> result = sut.parseRequestLine();
    requestLineParams = result.getValue();

    assertRequestLine(DELETE, "/", "HTTP/1.1");
}
