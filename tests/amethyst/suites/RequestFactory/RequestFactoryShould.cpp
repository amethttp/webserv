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
    const Result<Request_t> result = RequestFactory::create(requestString);

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
    ASSERT_EQUALS(path, request.requestLine.getTargetPath());
    ASSERT_EQUALS(query, request.requestLine.getTargetQuery());
}

static void assertRequestLine(method_t method, const std::string &targetUri, const std::string &version)
{
    ASSERT_EQUALS(method, request.requestLine.getMethod());
    ASSERT_EQUALS(targetUri, request.requestLine.getTargetUri());
    ASSERT_EQUALS(version, request.requestLine.getHttpVersion());
}

static void assertHeaderSize(const size_t size)
{
    ASSERT_EQUALS(size, request.headers.getAmountOfHeaders());
}

static void assertHeader(const std::string &key, const std::string &value)
{
    ASSERT_EQUALS(value, request.headers.getHeaderValue(key));
}

static void assertBodyIsEmpty()
{
    ASSERT_EQUALS("", request.body.getMessage());
}

static void assertBody(const std::string &body)
{
    ASSERT_EQUALS(body, request.body.getMessage());
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

static void assertRequestIsInvalidFromBody(const std::string &bodyTypeHeader, const std::string &body, const std::string &errorMessage)
{
    const std::string invalidRequestString = "GET / HTTP/1.1\r\nHost:localhost\r\n" + bodyTypeHeader + "\r\n\r\n" + body;

    assertRequestIsInvalid(invalidRequestString, errorMessage);
}

static void assertCanCreateAResponseIsTrue(const std::string &requestString)
{
    const bool canCreateAResponse = RequestFactory::canCreateAResponse(requestString);

    ASSERT_TRUE(canCreateAResponse);
}

static void assertCanCreateAResponseIsFalse(const std::string &requestString)
{
    const bool canCreateAResponse = RequestFactory::canCreateAResponse(requestString);

    ASSERT_FALSE(canCreateAResponse);
}


/* BASIC REQUEST TESTS */
TEST(recognize_basic_HTTP_GET_request)
{
    request = createFromValidRequest("GET / HTTP/1.1\r\nHost: localhost\r\n\r\n");

    assertTargetComponents("/", "");
    assertRequestLine(GET, "/", "HTTP/1.1");
    assertHeaderSize(1);
    assertHeader(HOST, "localhost");
    assertBodyIsEmpty();
}

TEST(recognize_basic_HTTP_DELETE_request)
{
    request = createFromValidRequest("DELETE / HTTP/1.1\r\nHost: localhost\r\n\r\n");

    assertTargetComponents("/", "");
    assertRequestLine(DELETE, "/", "HTTP/1.1");
    assertHeaderSize(1);
    assertHeader(HOST, "localhost");
    assertBodyIsEmpty();
}

TEST(recognize_basic_HTTP_POST_request)
{
    request = createFromValidRequest("POST / HTTP/1.1\r\nHost: localhost\r\n\r\n");

    assertTargetComponents("/", "");
    assertRequestLine(POST, "/", "HTTP/1.1");
    assertHeaderSize(1);
    assertHeader(HOST, "localhost");
    assertBodyIsEmpty();
}


/* REQUEST LINE TESTS */
TEST(recognize_a_request_with_valid_request_line)
{
    request = createRequestFromValidRequestLine("GET /about/index/-._~%50%2c!$&().html?param=value?*+,;=%3c HTTP/1.1");

    assertRequestLine(GET, "/about/index/-._~%50%2c!$&().html?param=value?*+,;=%3c", "HTTP/1.1");
}

TEST(take_as_failure_an_invalid_request_line)
{
    assertRequestIsInvalidFromRequestLine("INVALID", BAD_REQUEST_ERR);
}

TEST(take_as_failure_a_non_implemented_HTTP_method)
{
    assertRequestIsInvalidFromRequestLine("NOT_IMPLEMENTED / HTTP/1.1", NOT_IMPLEMENTED_ERR);
}

TEST(take_as_failure_an_uri_longer_than_max_length)
{
    const char anyCharacter = 'A';
    const std::string invalidTarget = "/" + std::string(MAX_URI_LENGTH, anyCharacter);
    const std::string requestString = "GET " + invalidTarget + " HTTP/1.1";

    assertRequestIsInvalidFromRequestLine(requestString, URI_TOO_LONG_ERR);
}

TEST(take_as_failure_a_non_supported_HTTP_version)
{
    assertRequestIsInvalidFromRequestLine("GET / HTTP/2.1", HTTP_VERSION_NOT_SUPPORTED_ERR);
    assertRequestIsInvalidFromRequestLine("GET / HTTP/1.0", HTTP_VERSION_NOT_SUPPORTED_ERR);
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
    assertRequestIsInvalidFromRequestLine("GET /index/%0d HTTP/1.1", BAD_REQUEST_ERR);
    assertRequestIsInvalidFromRequestLine("GET /index/%0A HTTP/1.1", BAD_REQUEST_ERR);
    assertRequestIsInvalidFromRequestLine("GET /index/?%00 HTTP/1.1", BAD_REQUEST_ERR);
    assertRequestIsInvalidFromRequestLine("GET /index/?%7f HTTP/1.1", BAD_REQUEST_ERR);
}


/* REQUEST HEADERS TESTS */
TEST(take_as_failure_a_request_without_host_header)
{
    assertRequestIsInvalidFromHeaders("Connection: keep-alive", BAD_REQUEST_ERR);
}

TEST(take_as_failure_a_request_with_an_empty_host_header)
{
    assertRequestIsInvalidFromHeaders("Host:", BAD_REQUEST_ERR);
}

TEST(take_as_failure_a_request_with_a_host_header_consisted_of_OWS)
{
    assertRequestIsInvalidFromHeaders("Host:  \t  \t\t  ", BAD_REQUEST_ERR);
}

TEST(recognize_a_request_with_a_host_header_consisted_of_valid_characters)
{
    const std::string validCharacters = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz-._~!$&'()*+,;=";

    request = createRequestFromValidHeaders("Host: " + validCharacters);

    assertHeaderSize(1);
    assertHeader(HOST, validCharacters);
}

TEST(take_as_failure_a_request_with_a_host_header_containing_wrong_pct_encoded_chars)
{
    assertRequestIsInvalidFromHeaders("Host: localhost_%xx", BAD_REQUEST_ERR);
    assertRequestIsInvalidFromHeaders("Host: localhost_%f", BAD_REQUEST_ERR);
    assertRequestIsInvalidFromHeaders("Host: localhost_%", BAD_REQUEST_ERR);
}

TEST(take_as_failure_a_request_with_a_host_header_containing_invalid_characters)
{
    assertRequestIsInvalidFromHeaders("Host: loc alh ost", BAD_REQUEST_ERR);
    assertRequestIsInvalidFromHeaders("Host: loc\talh\tost", BAD_REQUEST_ERR);
    assertRequestIsInvalidFromHeaders("Host: loc{alh[ost", BAD_REQUEST_ERR);
}

TEST(recognize_a_request_with_a_host_header_with_port)
{
    request = createRequestFromValidHeaders("Host: localhost:8080");

    assertHeaderSize(1);
    assertHeader(HOST, "localhost:8080");
}

TEST(take_as_failure_a_host_header_with_only_port)
{
    assertRequestIsInvalidFromHeaders("Host::8000\r\n\r\n", BAD_REQUEST_ERR);
    assertRequestIsInvalidFromHeaders("Host: :8000\r\n\r\n", BAD_REQUEST_ERR);
    assertRequestIsInvalidFromHeaders("Host:  \t  \t\t  :8000\r\n\r\n", BAD_REQUEST_ERR);
}

TEST(recognize_a_request_with_a_host_header_consisted_of_valid_characters_and_port)
{
    const std::string validCharacters = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz%20-._~!$&'()*+,;=";
    const std::string hostFinalValue = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz -._~!$&'()*+,;=";

    request = createRequestFromValidHeaders("Host: " + validCharacters + ":8080");

    assertHeaderSize(1);
    assertHeader(HOST, hostFinalValue + ":8080");
}

TEST(take_as_failure_a_request_with_a_host_header_with_a_non_numeric_port)
{
    assertRequestIsInvalidFromHeaders("Host: localhost:", BAD_REQUEST_ERR);
    assertRequestIsInvalidFromHeaders("Host: localhost:invalid", BAD_REQUEST_ERR);
    assertRequestIsInvalidFromHeaders("Host: localhost:31.023", BAD_REQUEST_ERR);
}

TEST(take_as_failure_a_request_with_a_host_header_with_multiple_ports)
{
    assertRequestIsInvalidFromHeaders("Host: localhost:31:30", BAD_REQUEST_ERR);
}

TEST(take_as_failure_a_request_with_a_host_header_with_port_out_of_range)
{
    assertRequestIsInvalidFromHeaders("Host: localhost:-8419", BAD_REQUEST_ERR);
    assertRequestIsInvalidFromHeaders("Host: localhost:1000000", BAD_REQUEST_ERR);
}

TEST(recognize_a_request_with_a_host_header_with_a_port_with_leading_zeros)
{
    request = createRequestFromValidHeaders("Host: localhost:0000000000008080");

    assertHeaderSize(1);
    assertHeader(HOST, "localhost:0000000000008080");
}

TEST(take_as_failure_a_request_with_multiple_host_headers)
{
    assertRequestIsInvalidFromHeaders("Host: localhost\r\nHost: my.domain.com", BAD_REQUEST_ERR);
}

TEST(recognize_a_request_with_case_insensitive_headers)
{
    request = createRequestFromValidHeaders("hOSt: localhost\r\nCoNNeCtIon: keep-alive\r\ncOntENT-leNGTH: 0");

    assertHeaderSize(3);
    assertHeader(HOST, "localhost");
    assertHeader(CONNECTION, "keep-alive");
    assertHeader(CONTENT_LENGTH, "0");
}

TEST(recognize_a_request_with_valid_content_length_header_and_value_equal_to_zero)
{
    request = createRequestFromValidHeaders("Host: localhost\r\nContent-Length: 0");

    assertHeaderSize(2);
    assertHeader(HOST, "localhost");
    assertHeader(CONTENT_LENGTH, "0");
}

TEST(recognize_a_request_with_valid_content_length_header_and_value_greater_than_zero)
{
    request = createRequestFromValidBody("Content-Length: 10", "Valid body");

    assertHeaderSize(2);
    assertHeader(HOST, "localhost");
    assertHeader(CONTENT_LENGTH, "10");
}

TEST(take_as_failure_a_request_with_a_content_length_header_with_non_numeric_value)
{
    assertRequestIsInvalidFromHeaders("Host: localhost\r\nContent-Length:", BAD_REQUEST_ERR);
    assertRequestIsInvalidFromHeaders("Host: localhost\r\nContent-Length: -23", BAD_REQUEST_ERR);
    assertRequestIsInvalidFromHeaders("Host: localhost\r\nContent-Length: 2.03", BAD_REQUEST_ERR);
    assertRequestIsInvalidFromHeaders("Host: localhost\r\nContent-Length: invalid", BAD_REQUEST_ERR);
}

TEST(take_as_failure_a_request_with_multiple_content_length_headers)
{
    assertRequestIsInvalidFromHeaders("Host: localhost\r\nContent-Length: 0\r\nContent-Length: 0", BAD_REQUEST_ERR);
}

TEST(recognize_a_request_with_valid_transfer_encoding_header)
{
    request = createRequestFromValidBody("Transfer-Encoding: chunked", "0\r\n\r\n");

    assertHeaderSize(2);
    assertHeader(HOST, "localhost");
    assertHeader(TRANSFER_ENCODING, "chunked");
}

TEST(take_as_failure_a_request_with_a_transfer_encoding_header_with_invalid_value)
{
    assertRequestIsInvalidFromHeaders("Host: localhost\r\nTransfer-Encoding:", BAD_REQUEST_ERR);
    assertRequestIsInvalidFromHeaders("Host: localhost\r\nTransfer-Encoding: invalid", BAD_REQUEST_ERR);
}

TEST(take_as_failure_a_request_with_multiple_transfer_encoding_headers)
{
    assertRequestIsInvalidFromHeaders("Host: localhost\r\nTransfer-Encoding: chunked\r\nTransfer-Encoding: chunked", BAD_REQUEST_ERR);
}

TEST(take_as_failure_a_request_with_content_length_and_transfer_encoding_headers)
{
    assertRequestIsInvalidFromHeaders("Host: localhost\r\nContent-Length: 0\r\nTransfer-Encoding: chunked", BAD_REQUEST_ERR);
}

TEST(recognize_a_request_with_valid_connection_header_with_keep_alive_value)
{
    request = createRequestFromValidHeaders("Host: localhost\r\nConnection: keep-alive");

    assertHeaderSize(2);
    assertHeader(HOST, "localhost");
    assertHeader(CONNECTION, "keep-alive");
}

TEST(recognize_a_request_with_valid_connection_header_with_close_value)
{
    request = createRequestFromValidHeaders("Host: localhost\r\nConnection: close");

    assertHeaderSize(2);
    assertHeader(HOST, "localhost");
    assertHeader(CONNECTION, "close");
}

TEST(take_as_failure_a_request_with_a_connection_header_with_invalid_value)
{
    assertRequestIsInvalidFromHeaders("Host: localhost\r\nConnection:", BAD_REQUEST_ERR);
    assertRequestIsInvalidFromHeaders("Host: localhost\r\nConnection: invalid", BAD_REQUEST_ERR);
}

TEST(take_as_failure_a_request_with_multiple_connection_headers)
{
    assertRequestIsInvalidFromHeaders("Host: localhost\r\nConnection: close\r\nConnection: close", BAD_REQUEST_ERR);
}


/* REQUEST HEADERS PROCESSING TESTS */
TEST(recognize_and_decode_a_request_with_a_host_header_containing_valid_pct_encoded_chars)
{
    request = createRequestFromValidHeaders("Host: localhost_%20_%5b_%7b");

    assertHeaderSize(1);
    assertHeader(HOST, "localhost_ _[_{");
}

TEST(take_as_failure_a_host_name_with_pct_encoded_control_characters)
{
    assertRequestIsInvalidFromHeaders("Host: localhost_%00_%7f", BAD_REQUEST_ERR);
}

TEST(recognize_a_request_with_valid_case_insensitive_transfer_encoding_header)
{
    request = createRequestFromValidBody("Transfer-Encoding: cHUnKeD", "0\r\n\r\n");

    assertHeaderSize(2);
    assertHeader(HOST, "localhost");
    assertHeader(TRANSFER_ENCODING, "chunked");
}

TEST(recognize_a_request_with_valid_connection_header_with_case_insensitive_keep_alive_value)
{
    request = createRequestFromValidHeaders("Host: localhost\r\nConnection: KeEP-ALivE");

    assertHeaderSize(2);
    assertHeader(HOST, "localhost");
    assertHeader(CONNECTION, "keep-alive");
}

TEST(recognize_a_request_with_valid_connection_header_with_case_insensitive_close_value)
{
    request = createRequestFromValidHeaders("Host: localhost\r\nConnection: cLOSe");

    assertHeaderSize(2);
    assertHeader(HOST, "localhost");
    assertHeader(CONNECTION, "close");
}


/* REQUEST FULL BODY TESTS */
TEST(recognize_a_request_with_a_full_body)
{
    request = createRequestFromValidBody("Content-Length: 11", "Valid\r\nBody");

    assertBody("Valid\r\nBody");
}

TEST(recognize_a_request_with_an_empty_body_without_content_length_header)
{
    request = createRequestFromValidBody("No-Header: no header", "");

    assertBodyIsEmpty();
}

TEST(take_as_failure_a_request_with_a_body_length_superior_than_content_length_header_size)
{
    assertRequestIsInvalidFromBody("Content-Length: 0", "Invalid body", BAD_REQUEST_ERR);
    assertRequestIsInvalidFromBody("Content-Length: 10", "Invalid body", BAD_REQUEST_ERR);
    assertRequestIsInvalidFromBody("Content-Length: 0", " \t \t \t", BAD_REQUEST_ERR);
}

TEST(take_as_failure_a_request_with_body_without_content_length_nor_transfer_encoding_headers)
{
    assertRequestIsInvalidFromBody("No-Length: specified", "Invalid body", LENGTH_REQUIRED_ERR);
}


/* REQUEST CHUNKED BODY TESTS */
TEST(recognize_a_request_with_a_chunked_body)
{
    const std::string firstChunk = "a;ext=val;!ext2_=\"Ext: \\\"Valid\\\"\"\r\nValid body\r\n";
    const std::string secondChunk = "0000000002;extension=!_~$#\r\n: \r\n";
    const std::string thirdChunk = "00b;!_~$#=!_~$#;a=b;ggg=\"\";lol=\"\t val \t\"\r\nCompleted!!\r\n";
    const std::string lastChunk = "0;lastExt=\"\t \\\" \\\\ \\\\ \t\"\r\n";
    const std::string trailerFields = "Trailer: fields\r\nNew: header\r\nDynamic: values\r\n";
    const std::string validBody = firstChunk + secondChunk + thirdChunk + lastChunk + trailerFields;

    request = createRequestFromValidBody("Transfer-Encoding: chunked", validBody);

    assertBody("Valid body: Completed!!");
}

TEST(take_as_failure_a_request_with_an_invalid_body)
{
    assertRequestIsInvalidFromBody("Transfer-Encoding: chunked", "INVALID BODY", BAD_REQUEST_ERR);
}


/* CAN CREATE A RESPONSE TESTS*/
TEST(return_true_to_a_valid_and_complete_request)
{
    assertCanCreateAResponseIsTrue("GET / HTTP/1.1\r\nHost: localhost\r\n\r\n");
}

TEST(return_true_to_a_complex_and_valid_and_complete_request)
{
    assertCanCreateAResponseIsTrue("POST /index.html%AA?%AA? HTTP/1.1\r\nHost: localhost_%FD\r\nConnection: close\r\n\r\n");
}

TEST(return_false_to_a_request_without_the_double_CRLF_separator)
{
    assertCanCreateAResponseIsFalse("GET / HTTP/1.1\r\n");
    assertCanCreateAResponseIsFalse("GET / HTTP/1.1\r\nHost: localhost\r");
    assertCanCreateAResponseIsFalse("GET / HTTP/1.1\r\nHost: localhost\r\n");
    assertCanCreateAResponseIsFalse("GET / HTTP/1.1\r\nHost: localhost\r\n\r");
}

TEST(return_true_to_an_invalid_but_complete_request)
{
    assertCanCreateAResponseIsTrue("DELETE /index.html HTTP/1.1\r\nINVALID\r\n\r\n");
    assertCanCreateAResponseIsTrue("INVALID\r\n\r\n");
    assertCanCreateAResponseIsTrue("\x01\x05\x07\b\r\v\r\n\r\n");
    assertCanCreateAResponseIsTrue("\r\n\r\n");
}

TEST(return_true_to_a_complete_request_with_an_invalid_body)
{
    assertCanCreateAResponseIsTrue("GET / HTTP/1.1\r\nHost: localhost\r\n\r\nInvalid But Complete Body");
}

TEST(return_true_to_a_complete_request_with_a_valid_full_body)
{
    assertCanCreateAResponseIsTrue("GET / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 0\r\n\r\n");
    assertCanCreateAResponseIsTrue("GET / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 10\r\n\r\nValid body");
}

TEST(return_true_to_a_complete_request_with_an_invalid_full_body)
{
    assertCanCreateAResponseIsTrue("GET / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 0\r\n\r\nInvalid body");
    assertCanCreateAResponseIsTrue("GET / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 10\r\n\r\nInvalid body");
}

TEST(return_false_to_a_request_with_an_incomplete_full_body)
{
    assertCanCreateAResponseIsFalse("GET / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 10\r\n\r\n");
    assertCanCreateAResponseIsFalse("GET / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 15\r\n\r\nIncomplete");
}
