#include "test/test.hpp"
#include "test/assert/assert.hpp"
#include "WebServer/Client/Request/RequestFactory/RequestFactory.hpp"

TEST(recognize_basic_HTTP_GET_request)
{
    Result<RequestInfo_t> actual = RequestFactory::create("GET / HTTP/1.1\r\nHost: localhost\r\n\r\n");
    
    Request_t request = actual.getValue().request;
    ASSERT_EQUALS(GET, request.method);
    ASSERT_EQUALS("/", request.target);
    ASSERT_EQUALS("HTTP/1.1", request.httpVersion);
    ASSERT_EQUALS(1, request.headers.size());
    ASSERT_EQUALS("localhost", request.headers.at("Host"));
    ASSERT_EQUALS("", request.body);
}

TEST(recognize_basic_HTTP_DELETE_request)
{
    Result<RequestInfo_t> actual = RequestFactory::create("DELETE / HTTP/1.1\r\nHost: localhost\r\n\r\n");
    
    Request_t request = actual.getValue().request;
    ASSERT_EQUALS(DELETE, request.method);
    ASSERT_EQUALS("/", request.target);
    ASSERT_EQUALS("HTTP/1.1", request.httpVersion);
    ASSERT_EQUALS(1, request.headers.size());
    ASSERT_EQUALS("localhost", request.headers.at("Host"));
    ASSERT_EQUALS("", request.body);
}
