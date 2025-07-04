#include "test/test.hpp"
#include "test/assert/assert.hpp"
#include "WebServer/Client/Request/RequestParser/RequestParser.hpp"

namespace
{
    RequestLineParams_t requestLine;
    headers_t headers;
}

static RequestLineParams_t createFromValidRequestLine(const std::string &requestLineString)
{
    const RequestTokenizer requestTokenizer(requestLineString);
    RequestParser sut(requestTokenizer);

    const Result<RequestLineParams_t> result = sut.parseRequestLine();

    return result.getValue();
}

static headers_t createFromValidHeaders(const std::string &requestHeaders)
{
    const RequestTokenizer requestTokenizer(requestHeaders);
    RequestParser sut(requestTokenizer);

    const Result<headers_t> result = sut.parseHeaders();

    return result.getValue();
}

static void assertRequestLine(method_t method, const std::string &targetUri, const std::string &version)
{
    ASSERT_EQUALS(method, requestLine.method);
    ASSERT_EQUALS(targetUri, requestLine.target.uri);
    ASSERT_EQUALS(version, requestLine.httpVersion);
}

static void assertRequestLineIsInvalid(const std::string &invalidRequestString, const std::string &errorMessage)
{
    const RequestTokenizer requestTokenizer(invalidRequestString);
    RequestParser sut(requestTokenizer);

    const Result<RequestLineParams_t> result = sut.parseRequestLine();

    ASSERT_TRUE(result.isFailure());
    ASSERT_EQUALS(errorMessage, result.getError());
}

static void assertRequestHeaderIsInvalid(const std::string &invalidHeader, const std::string &errorMessage)
{
    const RequestTokenizer requestTokenizer(invalidHeader);
    RequestParser sut(requestTokenizer);

    const Result<headers_t> result = sut.parseHeaders();

    ASSERT_TRUE(result.isFailure());
    ASSERT_EQUALS(errorMessage, result.getError());
}

static void assertHeaderSize(const size_t size)
{
    ASSERT_EQUALS(size, headers.size());
}

static void assertHeader(const std::string &key, const std::string &value)
{
    ASSERT_EQUALS(value, headers.at(key));
}


/* BASIC REQUEST LINE */
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


/* REQUEST LINE METHOD CRITERIA */
TEST(recognize_as_not_implemented_a_case_insensitive_method)
{
    requestLine = createFromValidRequestLine("get / HTTP/1.1");

    assertRequestLine(NOT_IMPLEMENTED, "/", "HTTP/1.1");
}

TEST(recognize_as_not_implemented_a_non_implemented_method_consisted_only_of_alphabetic_characters)
{
    requestLine = createFromValidRequestLine("NotImplemented / HTTP/1.1");

    assertRequestLine(NOT_IMPLEMENTED, "/", "HTTP/1.1");
}

TEST(recognize_as_not_implemented_a_non_implemented_method_consisted_of_tchars)
{
    const std::string tchars = "!#$%&'*+-.^_`|~0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    const std::string nonImplementedRequestLineString = tchars + " / HTTP/1.1";

    requestLine = createFromValidRequestLine(nonImplementedRequestLineString);

    assertRequestLine(NOT_IMPLEMENTED, "/", "HTTP/1.1");
}

TEST(take_as_failure_a_method_consisted_of_invalid_characters)
{
    assertRequestLineIsInvalid("(),/:;\r<=>\b?@\f[\\]\t{}\n / HTTP/1.1", "400 Bad Request");
}

TEST(take_as_failure_a_method_that_contains_invalid_characters)
{
    assertRequestLineIsInvalid("DEL(),/:;\r<=>\b?@\f[\\]\t{}\nTE / HTTP/1.1", "400 Bad Request");
}

TEST(take_as_failure_an_empty_method)
{
    assertRequestLineIsInvalid(" / HTTP/1.1", "400 Bad Request");
}

TEST(take_as_failure_a_non_existant_method)
{
    assertRequestLineIsInvalid("/ HTTP/1.1", "400 Bad Request");
}


/* REQUEST LINE FIRST SP CRITERIA */
TEST(take_as_failure_a_request_line_without_the_first_SP)
{
    assertRequestLineIsInvalid("GET/ HTTP/1.1", "400 Bad Request");
}

TEST(take_as_failure_a_request_line_with_another_character_instead_of_the_first_SP)
{
    assertRequestLineIsInvalid("GET?/ HTTP/1.1", "400 Bad Request");
}

TEST(take_as_failure_a_request_line_with_a_HTAB_instead_of_the_first_SP)
{
    assertRequestLineIsInvalid("GET\t/ HTTP/1.1", "400 Bad Request");
}

TEST(take_as_failure_a_request_line_with_a_HTAB_and_SP_instead_of_a_single_first_SP)
{
    assertRequestLineIsInvalid("GET\t / HTTP/1.1", "400 Bad Request");
}

TEST(take_as_failure_a_request_line_with_a_SP_and_HTAB_instead_of_a_single_first_SP)
{
    assertRequestLineIsInvalid("GET \t/ HTTP/1.1", "400 Bad Request");
}

TEST(take_as_failure_a_request_line_with_multiple_first_SP)
{
    assertRequestLineIsInvalid("GET   / HTTP/1.1", "400 Bad Request");
}


/* REQUEST LINE TARGET CRITERIA */
TEST(recognize_a_simple_alphabetic_target)
{
    requestLine = createFromValidRequestLine("GET /index HTTP/1.1");

    assertRequestLine(GET, "/index", "HTTP/1.1");
}

TEST(recognize_a_target_with_extension)
{
    requestLine = createFromValidRequestLine("GET /index.html HTTP/1.1");

    assertRequestLine(GET, "/index.html", "HTTP/1.1");
}

TEST(recognize_a_target_with_one_directory_level)
{
    requestLine = createFromValidRequestLine("GET /profile/contact.php HTTP/1.1");

    assertRequestLine(GET, "/profile/contact.php", "HTTP/1.1");
}

TEST(recognize_a_target_with_multiple_directory_levels)
{
    requestLine = createFromValidRequestLine("GET /courses/science/physics.py HTTP/1.1");

    assertRequestLine(GET, "/courses/science/physics.py", "HTTP/1.1");
}

TEST(recognize_a_directory_target)
{
    requestLine = createFromValidRequestLine("GET /about/ HTTP/1.1");

    assertRequestLine(GET, "/about/", "HTTP/1.1");
}

TEST(recognize_a_multiple_directory_target)
{
    requestLine = createFromValidRequestLine("GET /about/services/ HTTP/1.1");

    assertRequestLine(GET, "/about/services/", "HTTP/1.1");
}

TEST(take_as_failure_a_target_not_starting_with_slash)
{
    assertRequestLineIsInvalid("GET ./ HTTP/1.1", "400 Bad Request");
    assertRequestLineIsInvalid("GET INVALID HTTP/1.1", "400 Bad Request");
    assertRequestLineIsInvalid("GET INVALID/ HTTP/1.1", "400 Bad Request");
}

TEST(recognize_a_target_consisted_of_pchars)
{
    const std::string pchars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz%AA-._~!$&'()*+,;=:@";
    const std::string target = "/" + pchars;
    const std::string validRequestLine = "GET " + target + " HTTP/1.1";

    requestLine = createFromValidRequestLine(validRequestLine);

    assertRequestLine(GET, target, "HTTP/1.1");
}

TEST(take_as_failure_a_target_consisted_of_invalid_characters)
{
    assertRequestLineIsInvalid("GET /^{}[] HTTP/1.1", "400 Bad Request");
    assertRequestLineIsInvalid("GET /\b\"\n`\t HTTP/1.1", "400 Bad Request");
}

TEST(take_as_failure_a_target_that_contains_invalid_characters)
{
    assertRequestLineIsInvalid("GET /INVALID^{}[] HTTP/1.1", "400 Bad Request");
    assertRequestLineIsInvalid("GET /^{}[]INVALID HTTP/1.1", "400 Bad Request");
    assertRequestLineIsInvalid("GET /INVA^\r{}\"[]\t\bLID HTTP/1.1", "400 Bad Request");
}

TEST(recognize_a_target_with_valid_pct_encoded_pchars)
{
    const std::string target = "/%aa_%ff_%AA_%FF_%09_%A0_%9F_%a0_%9f_%Df_%dF";
    const std::string validRequestLine = "GET " + target + " HTTP/1.1";

    requestLine = createFromValidRequestLine(validRequestLine);

    assertRequestLine(GET, target, "HTTP/1.1");
}

TEST(take_as_failure_a_target_with_invalid_pct_encoded_pchars)
{
    assertRequestLineIsInvalid("GET /%gg HTTP/1.1", "400 Bad Request");
    assertRequestLineIsInvalid("GET /%GG HTTP/1.1", "400 Bad Request");
    assertRequestLineIsInvalid("GET /%-r HTTP/1.1", "400 Bad Request");
}

TEST(take_as_failure_a_target_with_incomplete_pct_encoded_pchars)
{
    assertRequestLineIsInvalid("GET /%a HTTP/1.1", "400 Bad Request");
    assertRequestLineIsInvalid("GET /% HTTP/1.1", "400 Bad Request");
}

TEST(recognize_a_target_with_an_empty_query)
{
    requestLine = createFromValidRequestLine("GET /VALID/PATH/? HTTP/1.1");

    assertRequestLine(GET, "/VALID/PATH/?", "HTTP/1.1");
}

TEST(recognize_a_query_with_an_empty_parameter)
{
    requestLine = createFromValidRequestLine("GET /VALID/PATH/?param HTTP/1.1");

    assertRequestLine(GET, "/VALID/PATH/?param", "HTTP/1.1");
}

TEST(recognize_a_query_with_a_full_parameter)
{
    requestLine = createFromValidRequestLine("GET /VALID/PATH/?param=anyValue HTTP/1.1");

    assertRequestLine(GET, "/VALID/PATH/?param=anyValue", "HTTP/1.1");
}

TEST(recognize_a_query_with_multiple_parameters)
{
    requestLine = createFromValidRequestLine("GET /VALID/PATH/?param=anyValue&mode= HTTP/1.1");

    assertRequestLine(GET, "/VALID/PATH/?param=anyValue&mode=", "HTTP/1.1");
}

TEST(recognize_a_query_consisted_of_valid_characters)
{
    const std::string validChars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz%AA-._~!$&'()*+,;=:@/?";
    const std::string target = "/VALID/PATH/?" + validChars;
    const std::string validRequestLine = "GET " + target + " HTTP/1.1";

    requestLine = createFromValidRequestLine(validRequestLine);

    assertRequestLine(GET, target, "HTTP/1.1");
}

TEST(take_as_failure_a_query_consisted_of_invalid_characters)
{
    assertRequestLineIsInvalid("GET /VALID/PATH?^{}[] HTTP/1.1", "400 Bad Request");
    assertRequestLineIsInvalid("GET /VALID/PATH?\b\"\n`\t HTTP/1.1", "400 Bad Request");
}

TEST(take_as_failure_a_query_that_contains_invalid_characters)
{
    assertRequestLineIsInvalid("GET /VALID/PATH?INVALID^{}[] HTTP/1.1", "400 Bad Request");
    assertRequestLineIsInvalid("GET /VALID/PATH?^{}[]INVALID HTTP/1.1", "400 Bad Request");
    assertRequestLineIsInvalid("GET /VALID/PATH?INVA^\r{}\"[]\t\bLID HTTP/1.1", "400 Bad Request");
}

TEST(recognize_a_query_with_valid_pct_encoded_pchars)
{
    const std::string target = "/VALID/PATH/?%aa_%ff_%AA_%FF_%09_%A0_%9F_%a0_%9f_%Df_%dF";
    const std::string validRequestLine = "GET " + target + " HTTP/1.1";

    requestLine = createFromValidRequestLine(validRequestLine);

    assertRequestLine(GET, target, "HTTP/1.1");
}

TEST(take_as_failure_a_query_with_invalid_pct_encoded_pchars)
{
    assertRequestLineIsInvalid("GET /VALID/PATH/?%gg HTTP/1.1", "400 Bad Request");
    assertRequestLineIsInvalid("GET /VALID/PATH/?%GG HTTP/1.1", "400 Bad Request");
    assertRequestLineIsInvalid("GET /VALID/PATH/?%-r HTTP/1.1", "400 Bad Request");
}

TEST(take_as_failure_a_query_with_incomplete_pct_encoded_pchars)
{
    assertRequestLineIsInvalid("GET /VALID/PATH/?%a HTTP/1.1", "400 Bad Request");
    assertRequestLineIsInvalid("GET /VALID/PATH/?% HTTP/1.1", "400 Bad Request");
    assertRequestLineIsInvalid("GET /VALID/PATH/%a? HTTP/1.1", "400 Bad Request");
    assertRequestLineIsInvalid("GET /VALID/PATH/%? HTTP/1.1", "400 Bad Request");
}

TEST(take_as_failure_a_target_with_only_query)
{
    assertRequestLineIsInvalid("GET ?invalidQuery=anyValue HTTP/1.1", "400 Bad Request");
}

TEST(recognize_a_target_longer_than_maximum_length)
{
    const char anyValidCharacter = 'A';
    const std::string target = "/" + std::string(MAX_URI_LENGTH, anyValidCharacter);
    const std::string targetTooLongRequestLine = "GET " + target + " HTTP/1.1";

    requestLine = createFromValidRequestLine(targetTooLongRequestLine);

    assertRequestLine(GET, target, "HTTP/1.1");
}

TEST(take_as_failure_a_target_mixed_with_HTTP_version)
{
    assertRequestLineIsInvalid("GET /index.html/HTTP/1.1", "400 Bad Request");
    assertRequestLineIsInvalid("GET /index.html?HTTP/1.1", "400 Bad Request");
}

TEST(recognize_a_not_normalized_pct_encoded_target)
{
    requestLine = createFromValidRequestLine("GET /%2e%2e/%2e%2e/%2e%2e/etc/passwd HTTP/1.1");

    assertRequestLine(GET, "/%2e%2e/%2e%2e/%2e%2e/etc/passwd", "HTTP/1.1");
}

TEST(recognize_a_query_with_a_single_question_mark)
{
    requestLine = createFromValidRequestLine("GET /?? HTTP/1.1");

    assertRequestLine(GET, "/??", "HTTP/1.1");
}

TEST(recognize_a_query_with_a_single_equal)
{
    requestLine = createFromValidRequestLine("GET /?= HTTP/1.1");

    assertRequestLine(GET, "/?=", "HTTP/1.1");
}

TEST(recognize_a_query_with_a_single_ampersand)
{
    requestLine = createFromValidRequestLine("GET /?& HTTP/1.1");

    assertRequestLine(GET, "/?&", "HTTP/1.1");
}

TEST(recognize_a_query_with_multiple_ampersands)
{
    requestLine = createFromValidRequestLine("GET /?&&&&&& HTTP/1.1");

    assertRequestLine(GET, "/?&&&&&&", "HTTP/1.1");
}

TEST(take_as_failure_a_target_with_a_fragment_section)
{
    assertRequestLineIsInvalid("GET /index.html# HTTP/1.1", "400 Bad Request");
    assertRequestLineIsInvalid("GET /index.html#section HTTP/1.1", "400 Bad Request");
    assertRequestLineIsInvalid("GET /index.html?param=anyValue#section HTTP/1.1", "400 Bad Request");
}

TEST(take_as_failure_an_empty_target)
{
    assertRequestLineIsInvalid("GET  HTTP/1.1", "400 Bad Request");
}

TEST(take_as_failure_a_non_existant_target)
{
    assertRequestLineIsInvalid("GET HTTP/1.1", "400 Bad Request");
}


/* REQUEST LINE LAST SP CRITERIA */
TEST(take_as_failure_a_request_line_without_the_last_SP)
{
    assertRequestLineIsInvalid("GET /HTTP/1.1", "400 Bad Request");
}

TEST(take_as_failure_a_request_line_with_another_character_instead_of_the_last_SP)
{
    assertRequestLineIsInvalid("GET /?HTTP/1.1", "400 Bad Request");
}

TEST(take_as_failure_a_request_line_with_a_HTAB_instead_of_the_last_SP)
{
    assertRequestLineIsInvalid("GET /\tHTTP/1.1", "400 Bad Request");
}

TEST(take_as_failure_a_request_line_with_a_HTAB_and_SP_instead_of_a_single_last_SP)
{
    assertRequestLineIsInvalid("GET /\t HTTP/1.1", "400 Bad Request");
}

TEST(take_as_failure_a_request_line_with_a_SP_and_HTAB_instead_of_a_single_last_SP)
{
    assertRequestLineIsInvalid("GET / \tHTTP/1.1", "400 Bad Request");
}

TEST(take_as_failure_a_request_line_with_multiple_last_SP)
{
    assertRequestLineIsInvalid("GET /   HTTP/1.1", "400 Bad Request");
}


/* REQUEST LINE HTTP-VERSION CRITERIA */
TEST(take_as_failure_a_case_insensitive_HTTP_name)
{
    assertRequestLineIsInvalid("GET / Http/1.1", "400 Bad Request");
    assertRequestLineIsInvalid("GET / http/1.1", "400 Bad Request");
}

TEST(take_as_failure_an_invalid_HTTP_name)
{
    assertRequestLineIsInvalid("GET / INVALID/1.1", "400 Bad Request");
    assertRequestLineIsInvalid("GET / HFDAJFK_INVALID/1.1", "400 Bad Request");
    assertRequestLineIsInvalid("GET / HTTPINVALID/1.1", "400 Bad Request");
}

TEST(take_as_failure_a_repeated_HTTP_name)
{
    assertRequestLineIsInvalid("GET / HTTPHTTP/1.1", "400 Bad Request");
}

TEST(take_as_failure_a_SP_instead_of_HTTP_name)
{
    assertRequestLineIsInvalid("GET /  /1.1", "400 Bad Request");
}

TEST(take_as_failure_a_HTAB_instead_of_HTTP_name)
{
    assertRequestLineIsInvalid("GET / \t/1.1", "400 Bad Request");
}

TEST(take_as_failure_an_empty_HTTP_name)
{
    assertRequestLineIsInvalid("GET / /1.1", "400 Bad Request");
}

TEST(take_as_failure_an_invalid_HTTP_version_slash)
{
    assertRequestLineIsInvalid("GET / HTTP?1.1", "400 Bad Request");
    assertRequestLineIsInvalid("GET / HTTP$1.1", "400 Bad Request");
    assertRequestLineIsInvalid("GET / HTTPINVALID1.1", "400 Bad Request");
}

TEST(take_as_failure_a_repeated_HTTP_version_slash)
{
    assertRequestLineIsInvalid("GET / HTTP//1.1", "400 Bad Request");
}

TEST(take_as_failure_a_SP_instead_of_HTTP_version_slash)
{
    assertRequestLineIsInvalid("GET / HTTP 1.1", "400 Bad Request");
}

TEST(take_as_failure_a_HTAB_instead_of_HTTP_version_slash)
{
    assertRequestLineIsInvalid("GET / HTTP\t1.1", "400 Bad Request");
}

TEST(take_as_failure_an_empty_HTTP_version_slash)
{
    assertRequestLineIsInvalid("GET / HTTP1.1", "400 Bad Request");
}

TEST(recognize_a_non_supported_major_version)
{
    requestLine = createFromValidRequestLine("GET / HTTP/2.1");

    assertRequestLine(GET, "/", "HTTP/2.1");
}

TEST(take_as_failure_an_invalid_major_version)
{
    assertRequestLineIsInvalid("GET / HTTP/?.1", "400 Bad Request");
    assertRequestLineIsInvalid("GET / HTTP/1fA($!.1", "400 Bad Request");
    assertRequestLineIsInvalid("GET / HTTP/fe\r\b1.1", "400 Bad Request");
}

TEST(take_as_failure_a_major_version_with_more_than_one_digit)
{
    assertRequestLineIsInvalid("GET / HTTP/11.1", "400 Bad Request");
}

TEST(take_as_failure_a_negative_major_version)
{
    assertRequestLineIsInvalid("GET / HTTP/-1.1", "400 Bad Request");
}

TEST(take_as_failure_a_SP_instead_of_major_version)
{
    assertRequestLineIsInvalid("GET / HTTP/ .1", "400 Bad Request");
}

TEST(take_as_failure_a_HTAB_instead_of_major_version)
{
    assertRequestLineIsInvalid("GET / HTTP/\t.1", "400 Bad Request");
}

TEST(take_as_failure_an_empty_major_version)
{
    assertRequestLineIsInvalid("GET / HTTP/.1", "400 Bad Request");
}

TEST(take_as_failure_an_invalid_HTTP_version_dot)
{
    assertRequestLineIsInvalid("GET / HTTP/1(1", "400 Bad Request");
    assertRequestLineIsInvalid("GET / HTTP/1_1", "400 Bad Request");
    assertRequestLineIsInvalid("GET / HTTP/1INVALID1", "400 Bad Request");
}

TEST(take_as_failure_a_repeated_HTTP_version_dot)
{
    assertRequestLineIsInvalid("GET / HTTP/1..1", "400 Bad Request");
}

TEST(take_as_failure_a_SP_instead_of_HTTP_version_dot)
{
    assertRequestLineIsInvalid("GET / HTTP/1 1", "400 Bad Request");
}

TEST(take_as_failure_a_HTAB_instead_of_HTTP_version_dot)
{
    assertRequestLineIsInvalid("GET / HTTP/1\t1", "400 Bad Request");
}

TEST(take_as_failure_an_empty_HTTP_version_dot)
{
    assertRequestLineIsInvalid("GET / HTTP/11", "400 Bad Request");
}

TEST(recognize_a_non_supported_minor_version)
{
    requestLine = createFromValidRequestLine("GET / HTTP/1.0");

    assertRequestLine(GET, "/", "HTTP/1.0");
}

TEST(take_as_failure_an_invalid_minor_version)
{
    assertRequestLineIsInvalid("GET / HTTP/1.?", "400 Bad Request");
    assertRequestLineIsInvalid("GET / HTTP/1.1fA($!", "400 Bad Request");
    assertRequestLineIsInvalid("GET / HTTP/1.fe\r\b1", "400 Bad Request");
}

TEST(take_as_failure_a_minor_version_with_more_than_one_digit)
{
    assertRequestLineIsInvalid("GET / HTTP/1.11", "400 Bad Request");
}

TEST(take_as_failure_a_negative_minor_version)
{
    assertRequestLineIsInvalid("GET / HTTP/1.-1", "400 Bad Request");
}

TEST(take_as_failure_a_SP_instead_of_minor_version)
{
    assertRequestLineIsInvalid("GET / HTTP/1. ", "400 Bad Request");
}

TEST(take_as_failure_a_HTAB_instead_of_minor_version)
{
    assertRequestLineIsInvalid("GET / HTTP/1.\t", "400 Bad Request");
}

TEST(take_as_failure_an_empty_minor_version)
{
    assertRequestLineIsInvalid("GET / HTTP/1.", "400 Bad Request");
}

TEST(take_as_failure_an_invalid_HTTP_version)
{
    assertRequestLineIsInvalid("GET / INVALID", "400 Bad Request");
}

TEST(take_as_failure_an_incomplete_HTTP_version)
{
    assertRequestLineIsInvalid("GET / 1.1", "400 Bad Request");
    assertRequestLineIsInvalid("GET / HTTP", "400 Bad Request");
    assertRequestLineIsInvalid("GET / HTTP/", "400 Bad Request");
    assertRequestLineIsInvalid("GET / HTTP/1", "400 Bad Request");
}

TEST(take_as_failure_an_HTTP_version_with_multiple_minor_versions)
{
    assertRequestLineIsInvalid("GET / HTTP/1.1.1", "400 Bad Request");
}

TEST(take_as_failure_an_empty_HTTP_version)
{
    assertRequestLineIsInvalid("GET / ", "400 Bad Request");
}

TEST(take_as_failure_a_non_existant_HTTP_version)
{
    assertRequestLineIsInvalid("GET /", "400 Bad Request");
}


/* MISC. REQUEST LINE CRITERIA */
TEST(take_as_failure_a_request_line_preceded_by_SP)
{
    assertRequestLineIsInvalid(" GET / HTTP/1.1", "400 Bad Request");
}

TEST(take_as_failure_a_request_line_followed_by_SP)
{
    assertRequestLineIsInvalid("GET / HTTP/1.1 ", "400 Bad Request");
}

TEST(take_as_failure_a_request_line_with_an_invalid_first_token_and_more_than_three_tokens)
{
    assertRequestLineIsInvalid("INVALID GET / HTTP/1.1", "400 Bad Request");
}

TEST(take_as_failure_a_request_line_with_an_invalid_second_token_and_more_than_three_tokens)
{
    assertRequestLineIsInvalid("GET INVALID / HTTP/1.1", "400 Bad Request");
}

TEST(take_as_failure_a_request_line_with_an_invalid_third_token_and_more_than_three_tokens)
{
    assertRequestLineIsInvalid("GET / INVALID HTTP/1.1", "400 Bad Request");
}

TEST(take_as_failure_a_request_line_with_an_invalid_fourth_token_and_more_than_three_tokens)
{
    assertRequestLineIsInvalid("GET / HTTP/1.1 INVALID", "400 Bad Request");
}

TEST(take_as_failure_an_invalid_request_line)
{
    assertRequestLineIsInvalid("INVALID", "400 Bad Request");
}

TEST(take_as_failure_a_request_line_with_only_a_SP)
{
    assertRequestLineIsInvalid(" ", "400 Bad Request");
}

TEST(take_as_failure_an_empty_request_line)
{
    assertRequestLineIsInvalid("", "400 Bad Request");
}


/* REQUEST HEADERS CRITERIA */
TEST(recognize_a_simple_header)
{
    headers = createFromValidHeaders("Host: localhost");

    assertHeaderSize(1);
    assertHeader("Host", "localhost");
}

TEST(recognize_a_complex_header)
{
    headers = createFromValidHeaders("Content-Length: 1312");

    assertHeaderSize(1);
    assertHeader("Content-Length", "1312");
}

TEST(recognize_a_case_insensitive_header)
{
    headers = createFromValidHeaders("Host: localhost");

    assertHeaderSize(1);
    assertHeader("Host", "localhost");
}

TEST(recognize_a_header_whose_key_is_consisted_of_tchars)
{
    const std::string validKey = "!#$%&'*+-.^_`|~0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    const std::string expectedKey = "!#$%&'*+-.^_`|~0123456789abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz";

    headers = createFromValidHeaders(validKey + ": localhost");

    assertHeaderSize(1);
    assertHeader(expectedKey, "localhost");
}

TEST(take_as_failure_a_header_whose_key_contains_non_tchars)
{
    assertRequestHeaderIsInvalid("(),/;\r<=>\b?@\f[\\]\t{}\n: localhost", "400 Bad Request");
    assertRequestHeaderIsInvalid("Ho(),/;\r<=>\b?@\f[\\]\t{}\nst: localhost", "400 Bad Request");
    assertRequestHeaderIsInvalid("Ho\x01\x14st: localhost", "400 Bad Request");
}

TEST(take_as_failure_a_header_whose_key_contains_SP)
{
    assertRequestHeaderIsInvalid("Host : localhost", "400 Bad Request");
    assertRequestHeaderIsInvalid(" Host: localhost", "400 Bad Request");
    assertRequestHeaderIsInvalid("Ho st: localhost", "400 Bad Request");
}

TEST(take_as_failure_a_header_whose_key_contains_HTAB)
{
    assertRequestHeaderIsInvalid("Host\t: localhost", "400 Bad Request");
    assertRequestHeaderIsInvalid("\tHost: localhost", "400 Bad Request");
    assertRequestHeaderIsInvalid("Ho\tst: localhost", "400 Bad Request");
}

TEST(take_as_failure_a_header_whose_key_is_SP)
{
    assertRequestHeaderIsInvalid(" : localhost", "400 Bad Request");
}

TEST(take_as_failure_a_header_whose_key_is_HTAB)
{
    assertRequestHeaderIsInvalid("\t: localhost", "400 Bad Request");
}

TEST(take_as_failure_a_header_whose_key_is_empty)
{
    assertRequestHeaderIsInvalid(": localhost", "400 Bad Request");
}

TEST(take_as_failure_a_header_missing_the_colon_separator)
{
    assertRequestHeaderIsInvalid("Host localhost", "400 Bad Request");
}

TEST(take_as_failure_a_header_that_has_any_other_character_instead_of_the_colon_separator)
{
    assertRequestHeaderIsInvalid("Host? localhost", "400 Bad Request");
    assertRequestHeaderIsInvalid("Host! localhost", "400 Bad Request");
    assertRequestHeaderIsInvalid("Hosta localhost", "400 Bad Request");
}

TEST(take_as_failure_a_header_that_has_SP_instead_of_the_colon_separator)
{
    assertRequestHeaderIsInvalid("Host  localhost", "400 Bad Request");
}

TEST(take_as_failure_a_header_that_has_HTAB_instead_of_the_colon_separator)
{
    assertRequestHeaderIsInvalid("Host\t localhost", "400 Bad Request");
}

TEST(recognize_a_header_with_repeated_colon_separator)
{
    headers = createFromValidHeaders("Host:: localhost");

    assertHeaderSize(1);
    assertHeader("Host", ": localhost");
}

TEST(recognize_a_header_without_the_first_OWS)
{
    headers = createFromValidHeaders("Host:localhost");

    assertHeaderSize(1);
    assertHeader("Host", "localhost");
}

TEST(recognize_a_header_with_a_single_SP_as_first_OWS)
{
    headers = createFromValidHeaders("Host: localhost");

    assertHeaderSize(1);
    assertHeader("Host", "localhost");
}

TEST(recognize_a_header_with_a_single_HTAB_as_first_OWS)
{
    headers = createFromValidHeaders("Host:\tlocalhost");

    assertHeaderSize(1);
    assertHeader("Host", "localhost");
}

TEST(recognize_a_header_with_a_combination_of_first_OWS)
{
    headers = createFromValidHeaders("Host: \t   \t\t  \t\tlocalhost");

    assertHeaderSize(1);
    assertHeader("Host", "localhost");
}

TEST(recognize_a_header_whose_value_has_valid_printable_characters_and_HTAB)
{
    const std::string validValue = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ \t[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

    headers = createFromValidHeaders("Host: " + validValue);

    assertHeaderSize(1);
    assertHeader("Host", validValue);
}

TEST(recognize_a_header_whose_value_has_multiple_words_using_SP_as_separator)
{
    headers = createFromValidHeaders("Host: localhost and something else");

    assertHeaderSize(1);
    assertHeader("Host", "localhost and something else");
}

TEST(recognize_a_header_whose_value_has_multiple_words_using_HTAB_as_separator)
{
    headers = createFromValidHeaders("Host: localhost\tand\tsomething\telse");

    assertHeaderSize(1);
    assertHeader("Host", "localhost\tand\tsomething\telse");
}

TEST(recognize_a_header_whose_value_has_multiple_words_using_multiple_OWS_as_separator)
{
    headers = createFromValidHeaders("Host: localhost\t \tand   \tsomething\t   \t\telse");

    assertHeaderSize(1);
    assertHeader("Host", "localhost\t \tand   \tsomething\t   \t\telse");
}

TEST(take_as_failure_a_header_whose_value_contains_invalid_characters)
{
    assertRequestHeaderIsInvalid("Host: \r\b\n\n\r\x7f\x10", "400 Bad Request");
}

TEST(recognize_a_header_whose_value_is_a_single_SP)
{
    headers = createFromValidHeaders("Host: ");

    assertHeaderSize(1);
    assertHeader("Host", "");
}

TEST(recognize_a_header_whose_value_is_a_single_HTAB)
{
    headers = createFromValidHeaders("Host:\t");

    assertHeaderSize(1);
    assertHeader("Host", "");
}
