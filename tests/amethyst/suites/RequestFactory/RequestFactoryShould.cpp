#include "test/test.hpp"
#include "test/assert/assert.hpp"
#include "WebServer/Client/Request/RequestFactory/RequestFactory.hpp"

static Request_t createRequest(const std::string &requestString)
{
    Result<RequestInfo_t> result = RequestFactory::create(requestString);
    
    return result.getValue().request;
}

static void assertRequestLine(Request_t &request, method_t method, const std::string &target, const std::string &version)
{
    ASSERT_EQUALS(method, request.method);
    ASSERT_EQUALS(target, request.target);
    ASSERT_EQUALS(version, request.httpVersion);    
}

static void assertHeader(Request_t &request, const std::string &key, const std::string &value)
{
    ASSERT_EQUALS(value, request.headers.at(key)); 
}

static void assertBodyIsEmpty(Request_t &request)
{
    ASSERT_EQUALS("", request.body);
}

TEST(recognize_basic_HTTP_GET_request)
{
    Request_t request = createRequest("GET / HTTP/1.1\r\nHost: localhost\r\n\r\n");

    assertRequestLine(request, GET, "/", "HTTP/1.1");
    ASSERT_EQUALS(1, request.headers.size());
    assertHeader(request, "Host", "localhost");
    assertBodyIsEmpty(request);
}

TEST(recognize_basic_HTTP_DELETE_request)
{
    Request_t request = createRequest("DELETE / HTTP/1.1\r\nHost: localhost\r\n\r\n");

    assertRequestLine(request, DELETE, "/", "HTTP/1.1");
    ASSERT_EQUALS(1, request.headers.size());
    assertHeader(request, "Host", "localhost");
    assertBodyIsEmpty(request);
}

TEST(recognize_basic_HTTP_POST_request)
{
    Request_t request = createRequest("POST / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 0\r\n\r\n");

    assertRequestLine(request, POST, "/", "HTTP/1.1");
    ASSERT_EQUALS(2, request.headers.size());
    assertHeader(request, "Host", "localhost");
    assertHeader(request, "Content-Length", "0");
    assertBodyIsEmpty(request);
}
