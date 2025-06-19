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

TEST(should_recognize_a_basic_GET_request_line)
{
    requestLine = createFromValidRequestLine("GET / HTTP/1.1");

    assertRequestLine(GET, "/", "HTTP/1.1");
}

TEST(should_recognize_a_basic_POST_request_line)
{
    requestLine = createFromValidRequestLine("POST / HTTP/1.1");

    assertRequestLine(POST, "/", "HTTP/1.1");
}

TEST(should_recognize_a_basic_DELETE_request_line)
{
    requestLine = createFromValidRequestLine("DELETE / HTTP/1.1");

    assertRequestLine(DELETE, "/", "HTTP/1.1");
}
