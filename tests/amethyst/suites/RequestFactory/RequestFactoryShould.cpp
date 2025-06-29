#include "test/test.hpp"
#include "test/assert/assert.hpp"
#include "WebServer/Client/Request/RequestFactory/RequestFactory.hpp"
#include "WebServer/Client/Request/RequestParser/RequestParser.hpp"

namespace
{
    Request_t request;
}

static Request_t createFromValidRequest(const std::string &requestString)
{
    Result<Request_t> result = RequestFactory::create(requestString);

    return result.getValue();
}

static Request_t createRequestFromValidRequestLine(const std::string &requestLineString)
{
    const std::string requestString = requestLineString + "\r\nHost: localhost\r\n\r\n";

    return createFromValidRequest(requestString);
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

static void assertRequestIsInvalid(const std::string &invalidRequestString, const std::string &errorMessage)
{
    Result<Request_t> result = RequestFactory::create(invalidRequestString);

    ASSERT_TRUE(result.isFailure());
    ASSERT_EQUALS(errorMessage, result.getError());
}

static void assertRequestIsInvalidFromRequestLine(const std::string &invalidRequestLineString, const std::string &errorMessage)
{
    const std::string invalidRequestString = invalidRequestLineString + "\r\nHost: localhost\r\n\r\n";

    assertRequestIsInvalid(invalidRequestString, errorMessage);
}


/* BASIC REQUEST LINE TESTS */
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


/* REQUEST LINE FAILURE TESTS */
TEST(take_as_failure_an_invalid_request_line)
{
    assertRequestIsInvalidFromRequestLine("INVALID", "400 Bad Request");
}

TEST(take_as_failure_a_non_implemented_HTTP_method)
{
    assertRequestIsInvalidFromRequestLine("NOT_IMPLEMENTED / HTTP/1.1", "501 Not Implemented");
}

TEST(take_as_failure_an_uri_longer_than_max_length)
{
    const char anyCharacter = 'A';
    const std::string invalidTarget = "/" + std::string(MAX_URI_LENGTH, anyCharacter);
    const std::string requestString = "GET " + invalidTarget + " HTTP/1.1";

    assertRequestIsInvalidFromRequestLine(requestString, "414 URI Too Long");
}

TEST(take_as_failure_a_non_supported_HTTP_version)
{
    assertRequestIsInvalidFromRequestLine("GET / HTTP/2.1", "505 HTTP Version Not Supported");
    assertRequestIsInvalidFromRequestLine("GET / HTTP/1.0", "505 HTTP Version Not Supported");
}


/* REQUEST TARGET PCT-DECODING TESTS */
TEST(leave_the_same_file_target_if_it_does_not_contain_pct_encoded_pchars)
{
    request = createRequestFromValidRequestLine("GET /index.html HTTP/1.1");

    assertRequestLine(GET, "/index.html", "HTTP/1.1");
}

TEST(leave_the_same_directory_target_if_it_does_not_contain_pct_encoded_pchars)
{
    request = createRequestFromValidRequestLine("GET /courses/biology/ HTTP/1.1");

    assertRequestLine(GET, "/courses/biology/", "HTTP/1.1");
}

TEST(decode_valid_pct_encoded_pchars)
{
    request = createRequestFromValidRequestLine("GET /index/%20_%22_%25_%3c_%3e_%5b_%5c_%5d_%7b_%7d HTTP/1.1");

    assertRequestLine(GET, "/index/ _\"_%_<_>_[_\\_]_{_}", "HTTP/1.1");
}

/* REQUEST HEADERS TESTS */
TEST(recognize_basic_HTTP_request_without_OWS_inside_headers)
{
    request = createFromValidRequest("POST / HTTP/1.1\r\nHost:localhost\r\nContent-Length:0\r\n\r\n");

    assertRequestLine(POST, "/", "HTTP/1.1");
    assertHeaderSize(2);
    assertHeader("Host", "localhost");
    assertHeader("Content-Length", "0");
    assertBodyIsEmpty();
}
