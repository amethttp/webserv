#include "test/test.hpp"
#include "test/assert/assert.hpp"
#include "WebServer/Client/Request/RequestFactory/RequestFactory.hpp"

Request_t request;

static Request_t createFromValidRequest(const std::string &requestString)
{
    Result<RequestInfo_t> result = RequestFactory::create(requestString);
    
    return result.getValue().request;
}

static void assertRequestLine(method_t method, const std::string &target, const std::string &version)
{
    ASSERT_EQUALS(method, request.method);
    ASSERT_EQUALS(target, request.target);
    ASSERT_EQUALS(version, request.httpVersion);    
}

static void assertHeaderSize(size_t size)
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

TEST(recognize_basic_HTTP_request_without_OWS_inside_headers)
{
    request = createFromValidRequest("POST / HTTP/1.1\r\nHost:localhost\r\nContent-Length:0\r\n\r\n");

    assertRequestLine(POST, "/", "HTTP/1.1");
    assertHeaderSize(2);
    assertHeader("Host", "localhost");
    assertHeader("Content-Length", "0");
    assertBodyIsEmpty();
}
