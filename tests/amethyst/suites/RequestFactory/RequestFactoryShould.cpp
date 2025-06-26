#include "test/test.hpp"
#include "test/assert/assert.hpp"
#include "WebServer/Client/Request/RequestFactory/RequestFactory.hpp"

namespace
{
    Request_t request;
}

static Request_t createFromValidRequest(const std::string &requestString)
{
    Result<Request_t> result = RequestFactory::create(requestString);

    return result.getValue();
}

static void assertRequestLine(method_t method, const std::string &target, const std::string &version)
{
    ASSERT_EQUALS(method, request.requestLine.method);
    ASSERT_EQUALS(target, request.requestLine.target);
    ASSERT_EQUALS(version, request.requestLine.httpVersion);
}

static void assertHeaderSize(const size_t size)
{
    ASSERT_EQUALS(size, request.headers.size());
}

static void assertHeader(const std::string &key, const std::string &value)
{
    ASSERT_EQUALS(value, request.headers.at(key));
}

static void assertBodyIsEmpty()
{
    ASSERT_EQUALS("", request.body);
}

static void assertRequestStringIsInvalid(const std::string &invalidRequestString, const std::string &errorMessage)
{
    Result<Request_t> result = RequestFactory::create(invalidRequestString);

    ASSERT_TRUE(result.isFailure());
    ASSERT_EQUALS(errorMessage, result.getError());
}


TEST(recognize_basic_HTTP_GET_request)
{
    request = createFromValidRequest("GET / HTTP/1.1\r\nHost: localhost\r\n\r\n");

    assertRequestLine(GET, "/", "HTTP/1.1");
    assertHeaderSize(1);
    assertHeader("Host", "localhost");
    assertBodyIsEmpty();
}

TEST(recognize_basic_HTTP_DELETE_request)
{
    request = createFromValidRequest("DELETE / HTTP/1.1\r\nHost: localhost\r\n\r\n");

    assertRequestLine(DELETE, "/", "HTTP/1.1");
    assertHeaderSize(1);
    assertHeader("Host", "localhost");
    assertBodyIsEmpty();
}

TEST(recognize_basic_HTTP_POST_request)
{
    request = createFromValidRequest("POST / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 0\r\n\r\n");

    assertRequestLine(POST, "/", "HTTP/1.1");
    assertHeaderSize(2);
    assertHeader("Host", "localhost");
    assertHeader("Content-Length", "0");
    assertBodyIsEmpty();
}

TEST(take_as_failure_a_not_implemented_HTTP_method)
{
    assertRequestStringIsInvalid("INVALID / HTTP/1.1\r\nHost: localhost\r\n\r\n", "501 Not Implemented");
}

TEST(take_as_failure_an_empty_HTTP_method)
{
    assertRequestStringIsInvalid(" / HTTP/1.1\r\nHost: localhost\r\n\r\n", "400 Bad Request");
}

TEST(take_as_failure_a_HTTP_request_without_method)
{
    assertRequestStringIsInvalid("/ HTTP/1.1\r\nHost: localhost\r\n\r\n", "400 Bad Request");
}

TEST(take_as_failure_an_invalid_request_line)
{
    assertRequestStringIsInvalid("INVALID\r\nHost: localhost\r\n\r\n", "400 Bad Request");
}

TEST(take_as_failure_a_request_line_with_only_a_SP)
{
    assertRequestStringIsInvalid(" \r\nHost: localhost\r\n\r\n", "400 Bad Request");
}

TEST(take_as_failure_an_empty_request_line)
{
    assertRequestStringIsInvalid("\r\nHost: localhost\r\n\r\n", "400 Bad Request");
}

TEST(recognize_basic_HTTP_request_without_OWS_inside_headers)
{
    request = createFromValidRequest("POST / HTTP/1.1\r\nHost:localhost\r\nContent-Length:0\r\n\r\n");

    assertRequestLine(POST, "/", "HTTP/1.1");
    assertHeaderSize(2);
    assertHeader("Host", "localhost");
    assertHeader("Content-Length", "0");
    assertBodyIsEmpty();
}
