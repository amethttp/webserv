#include "test/test.hpp"
#include "test/assert/assert.hpp"
#include "WebServer/Client/Request/RequestParser/RequestParser.hpp"

TEST(should_recognize_a_basic_GET_request_line)
{
    RequestTokenizer requestTokenizer("GET / HTTP/1.1");
    RequestParser requestParser(requestTokenizer);

    Result<RequestLineParams_t> result = requestParser.parseRequestLine();
    RequestLineParams_t requestLineParams = result.getValue();

    ASSERT_EQUALS(GET, requestLineParams.method);
    ASSERT_EQUALS("/", requestLineParams.target);
    ASSERT_EQUALS("HTTP/1.1", requestLineParams.httpVersion);
}
