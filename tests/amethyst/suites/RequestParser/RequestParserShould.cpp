#include "test/test.hpp"
#include "test/assert/assert.hpp"
#include "WebServer/Client/Request/RequestParser/RequestParser.hpp"

namespace
{
    RequestLineParams_t requestLineParams;
}

TEST(should_recognize_a_basic_GET_request_line)
{
    RequestTokenizer requestTokenizer("GET / HTTP/1.1");
    RequestParser sut(requestTokenizer);

    Result<RequestLineParams_t> result = sut.parseRequestLine();
    requestLineParams = result.getValue();

    ASSERT_EQUALS(GET, requestLineParams.method);
    ASSERT_EQUALS("/", requestLineParams.target);
    ASSERT_EQUALS("HTTP/1.1", requestLineParams.httpVersion);
}

TEST(should_recognize_a_basic_POST_request_line)
{
    RequestTokenizer requestTokenizer("POST / HTTP/1.1");
    RequestParser sut(requestTokenizer);

    Result<RequestLineParams_t> result = sut.parseRequestLine();
    requestLineParams = result.getValue();

    ASSERT_EQUALS(POST, requestLineParams.method);
    ASSERT_EQUALS("/", requestLineParams.target);
    ASSERT_EQUALS("HTTP/1.1", requestLineParams.httpVersion);
}

TEST(should_recognize_a_basic_DELETE_request_line)
{
    RequestTokenizer requestTokenizer("DELETE / HTTP/1.1");
    RequestParser sut(requestTokenizer);

    Result<RequestLineParams_t> result = sut.parseRequestLine();
    requestLineParams = result.getValue();

    ASSERT_EQUALS(DELETE, requestLineParams.method);
    ASSERT_EQUALS("/", requestLineParams.target);
    ASSERT_EQUALS("HTTP/1.1", requestLineParams.httpVersion);
}
