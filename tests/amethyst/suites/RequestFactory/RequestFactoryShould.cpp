#include "test/test.hpp"
#include "test/assert/assert.hpp"
#include "WebServer/Client/Request/RequestFactory/RequestFactory.hpp"

TEST(recognize_basic_HTTP_GET_request)
{
    Result<RequestInfo_t> actual = RequestFactory::create("GET / HTTP/1.1\r\nHost: localhost\r\n\r\n");

    ASSERT_EQUALS(GET, actual.getValue().request.method);
    ASSERT_EQUALS("/", actual.getValue().request.target);
    ASSERT_EQUALS("HTTP/1.1", actual.getValue().request.httpVersion);
    ASSERT_EQUALS(1, actual.getValue().request.headers.size());
    ASSERT_EQUALS("localhost", actual.getValue().request.headers.at("Host"));
    ASSERT_EQUALS("", actual.getValue().request.body);
}
