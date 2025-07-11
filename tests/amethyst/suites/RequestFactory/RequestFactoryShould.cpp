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

static Request_t createRequestFromValidHeaders(const std::string &headersString)
{
    const std::string requestString = "GET / HTTP/1.1\r\n" + headersString + "\r\n\r\n";

    return createFromValidRequest(requestString);
}

static Request_t createRequestFromValidBody(const std::string &bodyTypeHeader, const std::string &body)
{
    const std::string requestString = "GET / HTTP/1.1\r\nHost: localhost\r\n" + bodyTypeHeader + "\r\n\r\n" + body;

    return createFromValidRequest(requestString);
}

static void assertTargetComponents(const std::string &path, const std::string &query)
{
    ASSERT_EQUALS(path, request.requestLine.target.path);
    ASSERT_EQUALS(query, request.requestLine.target.query);
}

static void assertRequestLine(method_t method, const std::string &targetUri, const std::string &version)
{
    ASSERT_EQUALS(method, request.requestLine.method);
    ASSERT_EQUALS(targetUri, request.requestLine.target.uri);
    ASSERT_EQUALS(version, request.requestLine.httpVersion);
}

static void assertHeaderSize(const size_t size)
{
    ASSERT_EQUALS(size, request.headers.getAmountOfHeaders());
}

static void assertHeader(const std::string &key, const std::string &value)
{
    ASSERT_EQUALS(value, request.headers.getHeader(key).getValue());
}

static void assertBodyIsEmpty()
{
    ASSERT_EQUALS("", request.body);
}

static void assertBody(const std::string &body)
{
    ASSERT_EQUALS(body, request.body);
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

static void assertRequestIsInvalidFromHeaders(const std::string &invalidHeadersString, const std::string &errorMessage)
{
    const std::string invalidRequestString = "GET / HTTP/1.1\r\n" + invalidHeadersString + "\r\n\r\n";

    assertRequestIsInvalid(invalidRequestString, errorMessage);
}


/* BASIC REQUEST LINE TESTS */
TEST(recognize_basic_HTTP_GET_request)
{
    request = createFromValidRequest("GET / HTTP/1.1\r\nHost: localhost\r\n\r\n");

    assertTargetComponents("/", "");
    assertRequestLine(GET, "/", "HTTP/1.1");
    assertHeaderSize(1);
    assertHeader("Host", "localhost");
    assertBodyIsEmpty();
}

TEST(recognize_basic_HTTP_DELETE_request)
{
    request = createFromValidRequest("DELETE / HTTP/1.1\r\nHost: localhost\r\n\r\n");

    assertTargetComponents("/", "");
    assertRequestLine(DELETE, "/", "HTTP/1.1");
    assertHeaderSize(1);
    assertHeader("Host", "localhost");
    assertBodyIsEmpty();
}

TEST(recognize_basic_HTTP_POST_request)
{
    request = createFromValidRequest("POST / HTTP/1.1\r\nHost: localhost\r\n\r\n");

    assertTargetComponents("/", "");
    assertRequestLine(POST, "/", "HTTP/1.1");
    assertHeaderSize(1);
    assertHeader("Host", "localhost");
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


/* REQUEST TARGET PROCESSING TESTS */
TEST(process_a_request_target_separation_and_decoding_and_normalization)
{
    const std::string unprocessedPath = "//../%2e%2E/about/./%2e/%2F//index.html_%3c_%3C_%3e_%3Fvoid";
    const std::string unprocessedQuery = "query?/../var_%3c_%3C_%3e";
    const std::string unprocessedTarget = unprocessedPath + '?' + unprocessedQuery;

    request = createRequestFromValidRequestLine("GET " + unprocessedTarget + " HTTP/1.1");

    assertTargetComponents("/about/index.html_<_<_>_?void", unprocessedQuery);
    assertRequestLine(GET, unprocessedTarget, "HTTP/1.1");
}

TEST(take_as_failure_a_target_with_pct_encoded_control_chars)
{
    assertRequestIsInvalidFromRequestLine("GET /index/%0d HTTP/1.1", "400 Bad Request");
    assertRequestIsInvalidFromRequestLine("GET /index/%0A HTTP/1.1", "400 Bad Request");
    assertRequestIsInvalidFromRequestLine("GET /index/?%00 HTTP/1.1", "400 Bad Request");
    assertRequestIsInvalidFromRequestLine("GET /index/?%7f HTTP/1.1", "400 Bad Request");
}


/* REQUEST HEADERS TESTS */
TEST(take_as_failure_a_request_without_host_header)
{
    assertRequestIsInvalidFromHeaders("Connection: keep-alive", "400 Bad Request");
}

TEST(take_as_failure_a_request_with_an_empty_host_header)
{
    assertRequestIsInvalidFromHeaders("Host:", "400 Bad Request");
}

TEST(take_as_failure_a_request_with_a_host_header_consisted_of_OWS)
{
    assertRequestIsInvalidFromHeaders("Host:  \t  \t\t  ", "400 Bad Request");
}

TEST(recognize_a_request_with_a_host_header_consisted_of_valid_characters)
{
    const std::string validCharacters = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz-._~!$&'()*+,;=";

    request = createRequestFromValidHeaders("Host: " + validCharacters);

    assertHeaderSize(1);
    assertHeader("Host", validCharacters);
}

TEST(take_as_failure_a_request_with_a_host_header_containing_wrong_pct_encoded_chars)
{
    assertRequestIsInvalidFromHeaders("Host: localhost_%xx", "400 Bad Request");
    assertRequestIsInvalidFromHeaders("Host: localhost_%f", "400 Bad Request");
    assertRequestIsInvalidFromHeaders("Host: localhost_%", "400 Bad Request");
}

TEST(take_as_failure_a_request_with_a_host_header_containing_invalid_characters)
{
    assertRequestIsInvalidFromHeaders("Host: loc alh ost", "400 Bad Request");
    assertRequestIsInvalidFromHeaders("Host: loc\talh\tost", "400 Bad Request");
    assertRequestIsInvalidFromHeaders("Host: loc{alh[ost", "400 Bad Request");
}

TEST(recognize_a_request_with_a_host_header_with_port)
{
    request = createRequestFromValidHeaders("Host: localhost:8080");

    assertHeaderSize(1);
    assertHeader("Host", "localhost:8080");
}

TEST(take_as_failure_a_host_header_with_only_port)
{
    assertRequestIsInvalidFromHeaders("Host::8000\r\n\r\n", "400 Bad Request");
    assertRequestIsInvalidFromHeaders("Host: :8000\r\n\r\n", "400 Bad Request");
    assertRequestIsInvalidFromHeaders("Host:  \t  \t\t  :8000\r\n\r\n", "400 Bad Request");
}

TEST(recognize_a_request_with_a_host_header_consisted_of_valid_characters_and_port)
{
    const std::string validCharacters = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz%20-._~!$&'()*+,;=";

    request = createRequestFromValidHeaders("Host: " + validCharacters + ":8080");
}

TEST(take_as_failure_a_request_with_a_host_header_with_a_non_numeric_port)
{
    assertRequestIsInvalidFromHeaders("Host: localhost:", "400 Bad Request");
    assertRequestIsInvalidFromHeaders("Host: localhost:invalid", "400 Bad Request");
    assertRequestIsInvalidFromHeaders("Host: localhost:31.023", "400 Bad Request");
}

TEST(take_as_failure_a_request_with_a_host_header_with_multiple_ports)
{
    assertRequestIsInvalidFromHeaders("Host: localhost:31:30", "400 Bad Request");
}

TEST(take_as_failure_a_request_with_a_host_header_with_port_out_of_range)
{
    assertRequestIsInvalidFromHeaders("Host: localhost:-8419", "400 Bad Request");
    assertRequestIsInvalidFromHeaders("Host: localhost:1000000", "400 Bad Request");
}

TEST(recognize_a_request_with_a_host_header_with_a_port_with_leading_zeros)
{
    request = createRequestFromValidHeaders("Host: localhost:0000000000008080");

    assertHeaderSize(1);
    assertHeader("Host", "localhost:0000000000008080");
}

TEST(take_as_failure_a_request_with_multiple_host_headers)
{
    assertRequestIsInvalidFromHeaders("Host: localhost\r\nHost: my.domain.com", "400 Bad Request");
}

TEST(recognize_a_request_with_case_insensitive_headers)
{
    request = createRequestFromValidHeaders("hOSt: localhost\r\nCoNNeCtIon: keep-alive\r\ncOntENT-leNGTH: 0");

    assertHeaderSize(3);
    assertHeader("Host", "localhost");
    assertHeader("Connection", "keep-alive");
    assertHeader("Content-Length", "0");
}

TEST(recognize_a_request_with_valid_content_length_header_and_value_equal_to_zero)
{
    request = createRequestFromValidHeaders("Host: localhost\r\nContent-Length: 0");

    assertHeaderSize(2);
    assertHeader("Host", "localhost");
    assertHeader("Content-Length", "0");
}

TEST(recognize_a_request_with_valid_content_length_header_and_value_greater_than_zero)
{
    request = createRequestFromValidBody("Content-Length: 10", "Valid body");

    assertHeaderSize(2);
    assertHeader("Host", "localhost");
    assertHeader("Content-Length", "10");
}

TEST(take_as_failure_a_request_with_a_content_length_header_with_non_numeric_value)
{
    assertRequestIsInvalidFromHeaders("Host: localhost\r\nContent-Length:", "400 Bad Request");
    assertRequestIsInvalidFromHeaders("Host: localhost\r\nContent-Length: -23", "400 Bad Request");
    assertRequestIsInvalidFromHeaders("Host: localhost\r\nContent-Length: 2.03", "400 Bad Request");
    assertRequestIsInvalidFromHeaders("Host: localhost\r\nContent-Length: invalid", "400 Bad Request");
}

TEST(take_as_failure_a_request_with_multiple_content_length_headers)
{
    assertRequestIsInvalidFromHeaders("Host: localhost\r\nContent-Length: 0\r\nContent-Length: 0", "400 Bad Request");
}

TEST(recognize_a_request_with_valid_transfer_encoding_header)
{
    request = createRequestFromValidBody("Transfer-Encoding: chunked", "0\r\n\r\n");

    assertHeaderSize(2);
    assertHeader("Host", "localhost");
    assertHeader("Transfer-Encoding", "chunked");
}

TEST(take_as_failure_a_request_with_a_transfer_encoding_header_with_invalid_value)
{
    assertRequestIsInvalidFromHeaders("Host: localhost\r\nTransfer-Encoding:", "400 Bad Request");
    assertRequestIsInvalidFromHeaders("Host: localhost\r\nTransfer-Encoding: invalid", "400 Bad Request");
}

TEST(take_as_failure_a_request_with_multiple_transfer_encoding_headers)
{
    assertRequestIsInvalidFromHeaders("Host: localhost\r\nTransfer-Encoding: chunked\r\nTransfer-Encoding: chunked", "400 Bad Request");
}

TEST(take_as_failure_a_request_with_content_length_and_transfer_encoding_headers)
{
    assertRequestIsInvalidFromHeaders("Host: localhost\r\nContent-Length: 0\r\nTransfer-Encoding: chunked", "400 Bad Request");
}

TEST(recognize_a_request_with_valid_connection_header_with_keep_alive_value)
{
    request = createRequestFromValidHeaders("Host: localhost\r\nConnection: keep-alive");

    assertHeaderSize(2);
    assertHeader("Host", "localhost");
    assertHeader("Connection", "keep-alive");
}

TEST(recognize_a_request_with_valid_connection_header_with_close_value)
{
    request = createRequestFromValidHeaders("Host: localhost\r\nConnection: close");

    assertHeaderSize(2);
    assertHeader("Host", "localhost");
    assertHeader("Connection", "close");
}

TEST(take_as_failure_a_request_with_a_connection_header_with_invalid_value)
{
    assertRequestIsInvalidFromHeaders("Host: localhost\r\nConnection:", "400 Bad Request");
    assertRequestIsInvalidFromHeaders("Host: localhost\r\nConnection: invalid", "400 Bad Request");
}

TEST(take_as_failure_a_request_with_multiple_connection_headers)
{
    assertRequestIsInvalidFromHeaders("Host: localhost\r\nConnection: close\r\nConnection: close", "400 Bad Request");
}


/* REQUEST HEADERS PROCESSING TESTS */
TEST(recognize_and_decode_a_request_with_a_host_header_containing_valid_pct_encoded_chars)
{
    request = createRequestFromValidHeaders("Host: localhost_%20_%5b_%7b");

    assertHeaderSize(1);
    assertHeader("Host", "localhost_ _[_{");
}

TEST(take_as_failure_a_host_name_with_pct_encoded_control_characters)
{
    assertRequestIsInvalidFromHeaders("Host: localhost_%00_%7f", "400 Bad Request");
}

TEST(recognize_a_request_with_valid_case_insensitive_transfer_encoding_header)
{
    request = createRequestFromValidBody("Transfer-Encoding: cHUnKeD", "0\r\n\r\n");

    assertHeaderSize(2);
    assertHeader("Host", "localhost");
    assertHeader("Transfer-Encoding", "chunked");
}

TEST(recognize_a_request_with_valid_connection_header_with_case_insensitive_keep_alive_value)
{
    request = createRequestFromValidHeaders("Host: localhost\r\nConnection: KeEP-ALivE");

    assertHeaderSize(2);
    assertHeader("Host", "localhost");
    assertHeader("Connection", "keep-alive");
}

TEST(recognize_a_request_with_valid_connection_header_with_case_insensitive_close_value)
{
    request = createRequestFromValidHeaders("Host: localhost\r\nConnection: cLOSe");

    assertHeaderSize(2);
    assertHeader("Host", "localhost");
    assertHeader("Connection", "close");
}


/* REQUEST FULL BODY TESTS */
TEST(recognize_a_request_with_an_empty_body)
{
    request = createRequestFromValidBody("Content-Length: 0", "");

    assertBodyIsEmpty();
}

TEST(recognize_a_request_with_a_non_empty_body)
{
    request = createRequestFromValidBody("Content-Length: 10", "Valid body");

    assertBody("Valid body");
}

TEST(recognize_a_request_with_a_body_with_all_octets)
{
    const std::string controlChars = "\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0b\x0C\x0d\x0E\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1B\x1c\x1D\x1e\x1F\x7f";
    const std::string printableChars = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\x00";
    const std::string octets = controlChars + printableChars;

    request = createRequestFromValidBody("Content-Length: 127", octets);

    assertBody(octets);
}
