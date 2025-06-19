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

TEST(recognize_a_basic_GET_request_line)
{
    requestLine = createFromValidRequestLine("GET / HTTP/1.1");

    assertRequestLine(GET, "/", "HTTP/1.1");
}

TEST(recognize_a_basic_POST_request_line)
{
    requestLine = createFromValidRequestLine("POST / HTTP/1.1");

    assertRequestLine(POST, "/", "HTTP/1.1");
}

TEST(recognize_a_basic_DELETE_request_line)
{
    requestLine = createFromValidRequestLine("DELETE / HTTP/1.1");

    assertRequestLine(DELETE, "/", "HTTP/1.1");
}

TEST(take_as_failure_a_case_insensitive_method)
{
    RequestTokenizer requestTokenizer("get / HTTP/1.1");
    RequestParser requestParser(requestTokenizer);

    Result<RequestLineParams_t> result = requestParser.parseRequestLine();

    ASSERT_TRUE(result.isFailure());
    ASSERT_EQUALS("501 Not Implemented", result.getError());
}

TEST(take_as_failure_a_not_implemented_method_consisted_only_of_alphabetic_characters)
{
    RequestTokenizer requestTokenizer("INVALID / HTTP/1.1");
    RequestParser requestParser(requestTokenizer);

    Result<RequestLineParams_t> result = requestParser.parseRequestLine();

    ASSERT_TRUE(result.isFailure());
    ASSERT_EQUALS("501 Not Implemented", result.getError());
}

TEST(take_as_failure_a_not_implemented_method_consisted_of_tchars)
{
    std::string tchars = "!#$%&'*+-.^_`|~0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::string requestLineString = tchars + " / HTTP/1.1";
    RequestTokenizer requestTokenizer(requestLineString);
    RequestParser requestParser(requestTokenizer);

    Result<RequestLineParams_t> result = requestParser.parseRequestLine();

    ASSERT_TRUE(result.isFailure());
    ASSERT_EQUALS("501 Not Implemented", result.getError());
}
