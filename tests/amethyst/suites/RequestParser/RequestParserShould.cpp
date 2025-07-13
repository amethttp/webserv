#include "test/test.hpp"
#include "test/assert/assert.hpp"
#include "WebServer/Client/Request/RequestParser/RequestParser.hpp"

namespace
{
    RequestLineParams_t requestLine;
    HeaderCollection headers;
    std::string body;
}

static RequestLineParams_t parseFromValidRequestLine(const std::string &requestLineString)
{
    const RequestTokenizer requestTokenizer(requestLineString);
    RequestParser sut(requestTokenizer);

    const Result<RequestLineParams_t> result = sut.parseRequestLine();

    return result.getValue();
}

static HeaderCollection parseFromValidHeaders(const std::string &requestHeadersString)
{
    const RequestTokenizer requestTokenizer(requestHeadersString);
    RequestParser sut(requestTokenizer);

    const Result<HeaderCollection> result = sut.parseHeaders();

    return result.getValue();
}

static std::string parseFromValidBody(const std::string &requestBodyString)
{
    const RequestTokenizer requestTokenizer(requestBodyString);
    RequestParser sut(requestTokenizer);

    const Result<std::string> result = sut.parseChunkedBody();

    return result.getValue();
}

static void assertRequestLine(method_t method, const std::string &targetUri, const std::string &version)
{
    ASSERT_EQUALS(method, requestLine.method);
    ASSERT_EQUALS(targetUri, requestLine.target.uri);
    ASSERT_EQUALS(version, requestLine.httpVersion);
}

static void assertRequestLineIsInvalid(const std::string &invalidRequestString)
{
    const RequestTokenizer requestTokenizer(invalidRequestString);
    RequestParser sut(requestTokenizer);

    const Result<RequestLineParams_t> result = sut.parseRequestLine();

    ASSERT_TRUE(result.isFailure());
    ASSERT_EQUALS("400 Bad Request", result.getError());
}

static void assertRequestHeaderIsInvalid(const std::string &invalidHeader)
{
    const RequestTokenizer requestTokenizer(invalidHeader);
    RequestParser sut(requestTokenizer);

    const Result<HeaderCollection> result = sut.parseHeaders();

    ASSERT_TRUE(result.isFailure());
    ASSERT_EQUALS("400 Bad Request", result.getError());
}

static void assertRequestChunkedBodyIsInvalid(const std::string &invalidBody)
{
    const RequestTokenizer requestTokenizer(invalidBody);
    RequestParser sut(requestTokenizer);

    Result<std::string> result = sut.parseChunkedBody();

    ASSERT_TRUE(result.isFailure());
    ASSERT_EQUALS("400 Bad Request", result.getError());
}

static void assertHeaderSize(const size_t size)
{
    ASSERT_EQUALS(size, headers.getAmountOfHeaders());
}

static void assertHeader(const std::string &key, const std::string &value)
{
    ASSERT_EQUALS(value, headers.getHeader(key).getValue());
}

static void assertBodyIsEmpty()
{
    ASSERT_EQUALS("", body);
}

static void assertBody(const std::string &expectedBody)
{
    ASSERT_EQUALS(expectedBody, body);
}


/* BASIC REQUEST LINE */
TEST(recognize_a_basic_GET_request_line)
{
    requestLine = parseFromValidRequestLine("GET / HTTP/1.1");

    assertRequestLine(GET, "/", "HTTP/1.1");
}

TEST(recognize_a_basic_POST_request_line)
{
    requestLine = parseFromValidRequestLine("POST / HTTP/1.1");

    assertRequestLine(POST, "/", "HTTP/1.1");
}

TEST(recognize_a_basic_DELETE_request_line)
{
    requestLine = parseFromValidRequestLine("DELETE / HTTP/1.1");

    assertRequestLine(DELETE, "/", "HTTP/1.1");
}


/* REQUEST LINE METHOD CRITERIA */
TEST(recognize_as_not_implemented_a_case_insensitive_method)
{
    requestLine = parseFromValidRequestLine("get / HTTP/1.1");

    assertRequestLine(NOT_IMPLEMENTED, "/", "HTTP/1.1");
}

TEST(recognize_as_not_implemented_a_non_implemented_method_consisted_only_of_alphabetic_characters)
{
    requestLine = parseFromValidRequestLine("NotImplemented / HTTP/1.1");

    assertRequestLine(NOT_IMPLEMENTED, "/", "HTTP/1.1");
}

TEST(recognize_as_not_implemented_a_non_implemented_method_consisted_of_tchars)
{
    const std::string tchars = "!#$%&'*+-.^_`|~0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    const std::string nonImplementedRequestLineString = tchars + " / HTTP/1.1";

    requestLine = parseFromValidRequestLine(nonImplementedRequestLineString);

    assertRequestLine(NOT_IMPLEMENTED, "/", "HTTP/1.1");
}

TEST(take_as_failure_a_method_consisted_of_invalid_characters)
{
    assertRequestLineIsInvalid("(),/:;\r<=>\b?@\f[\\]\t{}\n / HTTP/1.1");
}

TEST(take_as_failure_a_method_that_contains_invalid_characters)
{
    assertRequestLineIsInvalid("DEL(),/:;\r<=>\b?@\f[\\]\t{}\nTE / HTTP/1.1");
}

TEST(take_as_failure_an_empty_method)
{
    assertRequestLineIsInvalid(" / HTTP/1.1");
}

TEST(take_as_failure_a_non_existant_method)
{
    assertRequestLineIsInvalid("/ HTTP/1.1");
}


/* REQUEST LINE FIRST SP CRITERIA */
TEST(take_as_failure_a_request_line_without_the_first_SP)
{
    assertRequestLineIsInvalid("GET/ HTTP/1.1");
}

TEST(take_as_failure_a_request_line_with_another_character_instead_of_the_first_SP)
{
    assertRequestLineIsInvalid("GET?/ HTTP/1.1");
}

TEST(take_as_failure_a_request_line_with_a_HTAB_instead_of_the_first_SP)
{
    assertRequestLineIsInvalid("GET\t/ HTTP/1.1");
}

TEST(take_as_failure_a_request_line_with_a_HTAB_and_SP_instead_of_a_single_first_SP)
{
    assertRequestLineIsInvalid("GET\t / HTTP/1.1");
}

TEST(take_as_failure_a_request_line_with_a_SP_and_HTAB_instead_of_a_single_first_SP)
{
    assertRequestLineIsInvalid("GET \t/ HTTP/1.1");
}

TEST(take_as_failure_a_request_line_with_multiple_first_SP)
{
    assertRequestLineIsInvalid("GET   / HTTP/1.1");
}


/* REQUEST LINE TARGET CRITERIA */
TEST(recognize_a_simple_alphabetic_target)
{
    requestLine = parseFromValidRequestLine("GET /index HTTP/1.1");

    assertRequestLine(GET, "/index", "HTTP/1.1");
}

TEST(recognize_a_target_with_extension)
{
    requestLine = parseFromValidRequestLine("GET /index.html HTTP/1.1");

    assertRequestLine(GET, "/index.html", "HTTP/1.1");
}

TEST(recognize_a_target_with_one_directory_level)
{
    requestLine = parseFromValidRequestLine("GET /profile/contact.php HTTP/1.1");

    assertRequestLine(GET, "/profile/contact.php", "HTTP/1.1");
}

TEST(recognize_a_target_with_multiple_directory_levels)
{
    requestLine = parseFromValidRequestLine("GET /courses/science/physics.py HTTP/1.1");

    assertRequestLine(GET, "/courses/science/physics.py", "HTTP/1.1");
}

TEST(recognize_a_directory_target)
{
    requestLine = parseFromValidRequestLine("GET /about/ HTTP/1.1");

    assertRequestLine(GET, "/about/", "HTTP/1.1");
}

TEST(recognize_a_multiple_directory_target)
{
    requestLine = parseFromValidRequestLine("GET /about/services/ HTTP/1.1");

    assertRequestLine(GET, "/about/services/", "HTTP/1.1");
}

TEST(take_as_failure_a_target_not_starting_with_slash)
{
    assertRequestLineIsInvalid("GET ./ HTTP/1.1");
    assertRequestLineIsInvalid("GET INVALID HTTP/1.1");
    assertRequestLineIsInvalid("GET INVALID/ HTTP/1.1");
}

TEST(recognize_a_target_consisted_of_pchars)
{
    const std::string pchars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz%AA-._~!$&'()*+,;=:@";
    const std::string target = "/" + pchars;
    const std::string validRequestLine = "GET " + target + " HTTP/1.1";

    requestLine = parseFromValidRequestLine(validRequestLine);

    assertRequestLine(GET, target, "HTTP/1.1");
}

TEST(take_as_failure_a_target_consisted_of_invalid_characters)
{
    assertRequestLineIsInvalid("GET /^{}[] HTTP/1.1");
    assertRequestLineIsInvalid("GET /\b\"\n`\t HTTP/1.1");
}

TEST(take_as_failure_a_target_that_contains_invalid_characters)
{
    assertRequestLineIsInvalid("GET /INVALID^{}[] HTTP/1.1");
    assertRequestLineIsInvalid("GET /^{}[]INVALID HTTP/1.1");
    assertRequestLineIsInvalid("GET /INVA^\r{}\"[]\t\bLID HTTP/1.1");
}

TEST(recognize_a_target_with_valid_pct_encoded_pchars)
{
    const std::string target = "/%aa_%ff_%AA_%FF_%09_%A0_%9F_%a0_%9f_%Df_%dF";
    const std::string validRequestLine = "GET " + target + " HTTP/1.1";

    requestLine = parseFromValidRequestLine(validRequestLine);

    assertRequestLine(GET, target, "HTTP/1.1");
}

TEST(take_as_failure_a_target_with_invalid_pct_encoded_pchars)
{
    assertRequestLineIsInvalid("GET /%gg HTTP/1.1");
    assertRequestLineIsInvalid("GET /%GG HTTP/1.1");
    assertRequestLineIsInvalid("GET /%-r HTTP/1.1");
}

TEST(take_as_failure_a_target_with_incomplete_pct_encoded_pchars)
{
    assertRequestLineIsInvalid("GET /%a HTTP/1.1");
    assertRequestLineIsInvalid("GET /% HTTP/1.1");
}

TEST(recognize_a_target_with_an_empty_query)
{
    requestLine = parseFromValidRequestLine("GET /VALID/PATH/? HTTP/1.1");

    assertRequestLine(GET, "/VALID/PATH/?", "HTTP/1.1");
}

TEST(recognize_a_query_with_an_empty_parameter)
{
    requestLine = parseFromValidRequestLine("GET /VALID/PATH/?param HTTP/1.1");

    assertRequestLine(GET, "/VALID/PATH/?param", "HTTP/1.1");
}

TEST(recognize_a_query_with_a_full_parameter)
{
    requestLine = parseFromValidRequestLine("GET /VALID/PATH/?param=anyValue HTTP/1.1");

    assertRequestLine(GET, "/VALID/PATH/?param=anyValue", "HTTP/1.1");
}

TEST(recognize_a_query_with_multiple_parameters)
{
    requestLine = parseFromValidRequestLine("GET /VALID/PATH/?param=anyValue&mode= HTTP/1.1");

    assertRequestLine(GET, "/VALID/PATH/?param=anyValue&mode=", "HTTP/1.1");
}

TEST(recognize_a_query_consisted_of_valid_characters)
{
    const std::string validChars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz%AA-._~!$&'()*+,;=:@/?";
    const std::string target = "/VALID/PATH/?" + validChars;
    const std::string validRequestLine = "GET " + target + " HTTP/1.1";

    requestLine = parseFromValidRequestLine(validRequestLine);

    assertRequestLine(GET, target, "HTTP/1.1");
}

TEST(take_as_failure_a_query_consisted_of_invalid_characters)
{
    assertRequestLineIsInvalid("GET /VALID/PATH?^{}[] HTTP/1.1");
    assertRequestLineIsInvalid("GET /VALID/PATH?\b\"\n`\t HTTP/1.1");
}

TEST(take_as_failure_a_query_that_contains_invalid_characters)
{
    assertRequestLineIsInvalid("GET /VALID/PATH?INVALID^{}[] HTTP/1.1");
    assertRequestLineIsInvalid("GET /VALID/PATH?^{}[]INVALID HTTP/1.1");
    assertRequestLineIsInvalid("GET /VALID/PATH?INVA^\r{}\"[]\t\bLID HTTP/1.1");
}

TEST(recognize_a_query_with_valid_pct_encoded_pchars)
{
    const std::string target = "/VALID/PATH/?%aa_%ff_%AA_%FF_%09_%A0_%9F_%a0_%9f_%Df_%dF";
    const std::string validRequestLine = "GET " + target + " HTTP/1.1";

    requestLine = parseFromValidRequestLine(validRequestLine);

    assertRequestLine(GET, target, "HTTP/1.1");
}

TEST(take_as_failure_a_query_with_invalid_pct_encoded_pchars)
{
    assertRequestLineIsInvalid("GET /VALID/PATH/?%gg HTTP/1.1");
    assertRequestLineIsInvalid("GET /VALID/PATH/?%GG HTTP/1.1");
    assertRequestLineIsInvalid("GET /VALID/PATH/?%-r HTTP/1.1");
}

TEST(take_as_failure_a_query_with_incomplete_pct_encoded_pchars)
{
    assertRequestLineIsInvalid("GET /VALID/PATH/?%a HTTP/1.1");
    assertRequestLineIsInvalid("GET /VALID/PATH/?% HTTP/1.1");
    assertRequestLineIsInvalid("GET /VALID/PATH/%a? HTTP/1.1");
    assertRequestLineIsInvalid("GET /VALID/PATH/%? HTTP/1.1");
}

TEST(take_as_failure_a_target_with_only_query)
{
    assertRequestLineIsInvalid("GET ?invalidQuery=anyValue HTTP/1.1");
}

TEST(recognize_a_target_longer_than_maximum_length)
{
    const char anyValidCharacter = 'A';
    const std::string target = "/" + std::string(MAX_URI_LENGTH, anyValidCharacter);
    const std::string targetTooLongRequestLine = "GET " + target + " HTTP/1.1";

    requestLine = parseFromValidRequestLine(targetTooLongRequestLine);

    assertRequestLine(GET, target, "HTTP/1.1");
}

TEST(take_as_failure_a_target_mixed_with_HTTP_version)
{
    assertRequestLineIsInvalid("GET /index.html/HTTP/1.1");
    assertRequestLineIsInvalid("GET /index.html?HTTP/1.1");
}

TEST(recognize_a_not_normalized_pct_encoded_target)
{
    requestLine = parseFromValidRequestLine("GET /%2e%2e/%2e%2e/%2e%2e/etc/passwd HTTP/1.1");

    assertRequestLine(GET, "/%2e%2e/%2e%2e/%2e%2e/etc/passwd", "HTTP/1.1");
}

TEST(recognize_a_query_with_a_single_question_mark)
{
    requestLine = parseFromValidRequestLine("GET /?? HTTP/1.1");

    assertRequestLine(GET, "/??", "HTTP/1.1");
}

TEST(recognize_a_query_with_a_single_equal)
{
    requestLine = parseFromValidRequestLine("GET /?= HTTP/1.1");

    assertRequestLine(GET, "/?=", "HTTP/1.1");
}

TEST(recognize_a_query_with_a_single_ampersand)
{
    requestLine = parseFromValidRequestLine("GET /?& HTTP/1.1");

    assertRequestLine(GET, "/?&", "HTTP/1.1");
}

TEST(recognize_a_query_with_multiple_ampersands)
{
    requestLine = parseFromValidRequestLine("GET /?&&&&&& HTTP/1.1");

    assertRequestLine(GET, "/?&&&&&&", "HTTP/1.1");
}

TEST(take_as_failure_a_target_with_a_fragment_section)
{
    assertRequestLineIsInvalid("GET /index.html# HTTP/1.1");
    assertRequestLineIsInvalid("GET /index.html#section HTTP/1.1");
    assertRequestLineIsInvalid("GET /index.html?param=anyValue#section HTTP/1.1");
}

TEST(take_as_failure_an_empty_target)
{
    assertRequestLineIsInvalid("GET  HTTP/1.1");
}

TEST(take_as_failure_a_non_existant_target)
{
    assertRequestLineIsInvalid("GET HTTP/1.1");
}


/* REQUEST LINE LAST SP CRITERIA */
TEST(take_as_failure_a_request_line_without_the_last_SP)
{
    assertRequestLineIsInvalid("GET /HTTP/1.1");
}

TEST(take_as_failure_a_request_line_with_another_character_instead_of_the_last_SP)
{
    assertRequestLineIsInvalid("GET /?HTTP/1.1");
}

TEST(take_as_failure_a_request_line_with_a_HTAB_instead_of_the_last_SP)
{
    assertRequestLineIsInvalid("GET /\tHTTP/1.1");
}

TEST(take_as_failure_a_request_line_with_a_HTAB_and_SP_instead_of_a_single_last_SP)
{
    assertRequestLineIsInvalid("GET /\t HTTP/1.1");
}

TEST(take_as_failure_a_request_line_with_a_SP_and_HTAB_instead_of_a_single_last_SP)
{
    assertRequestLineIsInvalid("GET / \tHTTP/1.1");
}

TEST(take_as_failure_a_request_line_with_multiple_last_SP)
{
    assertRequestLineIsInvalid("GET /   HTTP/1.1");
}


/* REQUEST LINE HTTP-VERSION CRITERIA */
TEST(take_as_failure_a_case_insensitive_HTTP_name)
{
    assertRequestLineIsInvalid("GET / Http/1.1");
    assertRequestLineIsInvalid("GET / http/1.1");
}

TEST(take_as_failure_an_invalid_HTTP_name)
{
    assertRequestLineIsInvalid("GET / INVALID/1.1");
    assertRequestLineIsInvalid("GET / HFDAJFK_INVALID/1.1");
    assertRequestLineIsInvalid("GET / HTTPINVALID/1.1");
}

TEST(take_as_failure_a_repeated_HTTP_name)
{
    assertRequestLineIsInvalid("GET / HTTPHTTP/1.1");
}

TEST(take_as_failure_a_SP_instead_of_HTTP_name)
{
    assertRequestLineIsInvalid("GET /  /1.1");
}

TEST(take_as_failure_a_HTAB_instead_of_HTTP_name)
{
    assertRequestLineIsInvalid("GET / \t/1.1");
}

TEST(take_as_failure_an_empty_HTTP_name)
{
    assertRequestLineIsInvalid("GET / /1.1");
}

TEST(take_as_failure_an_invalid_HTTP_version_slash)
{
    assertRequestLineIsInvalid("GET / HTTP?1.1");
    assertRequestLineIsInvalid("GET / HTTP$1.1");
    assertRequestLineIsInvalid("GET / HTTPINVALID1.1");
}

TEST(take_as_failure_a_repeated_HTTP_version_slash)
{
    assertRequestLineIsInvalid("GET / HTTP//1.1");
}

TEST(take_as_failure_a_SP_instead_of_HTTP_version_slash)
{
    assertRequestLineIsInvalid("GET / HTTP 1.1");
}

TEST(take_as_failure_a_HTAB_instead_of_HTTP_version_slash)
{
    assertRequestLineIsInvalid("GET / HTTP\t1.1");
}

TEST(take_as_failure_an_empty_HTTP_version_slash)
{
    assertRequestLineIsInvalid("GET / HTTP1.1");
}

TEST(recognize_a_non_supported_major_version)
{
    requestLine = parseFromValidRequestLine("GET / HTTP/2.1");

    assertRequestLine(GET, "/", "HTTP/2.1");
}

TEST(take_as_failure_an_invalid_major_version)
{
    assertRequestLineIsInvalid("GET / HTTP/?.1");
    assertRequestLineIsInvalid("GET / HTTP/1fA($!.1");
    assertRequestLineIsInvalid("GET / HTTP/fe\r\b1.1");
}

TEST(take_as_failure_a_major_version_with_more_than_one_digit)
{
    assertRequestLineIsInvalid("GET / HTTP/11.1");
}

TEST(take_as_failure_a_negative_major_version)
{
    assertRequestLineIsInvalid("GET / HTTP/-1.1");
}

TEST(take_as_failure_a_SP_instead_of_major_version)
{
    assertRequestLineIsInvalid("GET / HTTP/ .1");
}

TEST(take_as_failure_a_HTAB_instead_of_major_version)
{
    assertRequestLineIsInvalid("GET / HTTP/\t.1");
}

TEST(take_as_failure_an_empty_major_version)
{
    assertRequestLineIsInvalid("GET / HTTP/.1");
}

TEST(take_as_failure_an_invalid_HTTP_version_dot)
{
    assertRequestLineIsInvalid("GET / HTTP/1(1");
    assertRequestLineIsInvalid("GET / HTTP/1_1");
    assertRequestLineIsInvalid("GET / HTTP/1INVALID1");
}

TEST(take_as_failure_a_repeated_HTTP_version_dot)
{
    assertRequestLineIsInvalid("GET / HTTP/1..1");
}

TEST(take_as_failure_a_SP_instead_of_HTTP_version_dot)
{
    assertRequestLineIsInvalid("GET / HTTP/1 1");
}

TEST(take_as_failure_a_HTAB_instead_of_HTTP_version_dot)
{
    assertRequestLineIsInvalid("GET / HTTP/1\t1");
}

TEST(take_as_failure_an_empty_HTTP_version_dot)
{
    assertRequestLineIsInvalid("GET / HTTP/11");
}

TEST(recognize_a_non_supported_minor_version)
{
    requestLine = parseFromValidRequestLine("GET / HTTP/1.0");

    assertRequestLine(GET, "/", "HTTP/1.0");
}

TEST(take_as_failure_an_invalid_minor_version)
{
    assertRequestLineIsInvalid("GET / HTTP/1.?");
    assertRequestLineIsInvalid("GET / HTTP/1.1fA($!");
    assertRequestLineIsInvalid("GET / HTTP/1.fe\r\b1");
}

TEST(take_as_failure_a_minor_version_with_more_than_one_digit)
{
    assertRequestLineIsInvalid("GET / HTTP/1.11");
}

TEST(take_as_failure_a_negative_minor_version)
{
    assertRequestLineIsInvalid("GET / HTTP/1.-1");
}

TEST(take_as_failure_a_SP_instead_of_minor_version)
{
    assertRequestLineIsInvalid("GET / HTTP/1. ");
}

TEST(take_as_failure_a_HTAB_instead_of_minor_version)
{
    assertRequestLineIsInvalid("GET / HTTP/1.\t");
}

TEST(take_as_failure_an_empty_minor_version)
{
    assertRequestLineIsInvalid("GET / HTTP/1.");
}

TEST(take_as_failure_an_invalid_HTTP_version)
{
    assertRequestLineIsInvalid("GET / INVALID");
}

TEST(take_as_failure_an_incomplete_HTTP_version)
{
    assertRequestLineIsInvalid("GET / 1.1");
    assertRequestLineIsInvalid("GET / HTTP");
    assertRequestLineIsInvalid("GET / HTTP/");
    assertRequestLineIsInvalid("GET / HTTP/1");
}

TEST(take_as_failure_an_HTTP_version_with_multiple_minor_versions)
{
    assertRequestLineIsInvalid("GET / HTTP/1.1.1");
}

TEST(take_as_failure_an_empty_HTTP_version)
{
    assertRequestLineIsInvalid("GET / ");
}

TEST(take_as_failure_a_non_existant_HTTP_version)
{
    assertRequestLineIsInvalid("GET /");
}


/* MISC. REQUEST LINE CRITERIA */
TEST(take_as_failure_a_request_line_preceded_by_SP)
{
    assertRequestLineIsInvalid(" GET / HTTP/1.1");
}

TEST(take_as_failure_a_request_line_followed_by_SP)
{
    assertRequestLineIsInvalid("GET / HTTP/1.1 ");
}

TEST(take_as_failure_a_request_line_with_an_invalid_first_token_and_more_than_three_tokens)
{
    assertRequestLineIsInvalid("INVALID GET / HTTP/1.1");
}

TEST(take_as_failure_a_request_line_with_an_invalid_second_token_and_more_than_three_tokens)
{
    assertRequestLineIsInvalid("GET INVALID / HTTP/1.1");
}

TEST(take_as_failure_a_request_line_with_an_invalid_third_token_and_more_than_three_tokens)
{
    assertRequestLineIsInvalid("GET / INVALID HTTP/1.1");
}

TEST(take_as_failure_a_request_line_with_an_invalid_fourth_token_and_more_than_three_tokens)
{
    assertRequestLineIsInvalid("GET / HTTP/1.1 INVALID");
}

TEST(take_as_failure_an_invalid_request_line)
{
    assertRequestLineIsInvalid("INVALID");
}

TEST(take_as_failure_a_request_line_with_only_a_SP)
{
    assertRequestLineIsInvalid(" ");
}

TEST(take_as_failure_an_empty_request_line)
{
    assertRequestLineIsInvalid("");
}


/* REQUEST HEADERS CRITERIA */
TEST(recognize_a_simple_header)
{
    headers = parseFromValidHeaders("Host: localhost");

    assertHeaderSize(1);
    assertHeader("Host", "localhost");
}

TEST(recognize_a_complex_header)
{
    headers = parseFromValidHeaders("Content-Length: 1312");

    assertHeaderSize(1);
    assertHeader("Content-Length", "1312");
}

TEST(recognize_a_case_insensitive_header)
{
    headers = parseFromValidHeaders("Host: localhost");

    assertHeaderSize(1);
    assertHeader("Host", "localhost");
}

TEST(recognize_a_header_whose_key_is_consisted_of_tchars)
{
    const std::string validKey = "!#$%&'*+-.^_`|~0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    const std::string expectedKey = "!#$%&'*+-.^_`|~0123456789abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz";

    headers = parseFromValidHeaders(validKey + ": localhost");

    assertHeaderSize(1);
    assertHeader(expectedKey, "localhost");
}

TEST(take_as_failure_a_header_whose_key_contains_non_tchars)
{
    assertRequestHeaderIsInvalid("(),/;\r<=>\b?@\f[\\]\t{}\n: localhost");
    assertRequestHeaderIsInvalid("Ho(),/;\r<=>\b?@\f[\\]\t{}\nst: localhost");
    assertRequestHeaderIsInvalid("Ho\x01\x14st: localhost");
}

TEST(take_as_failure_a_header_whose_key_contains_SP)
{
    assertRequestHeaderIsInvalid("Host : localhost");
    assertRequestHeaderIsInvalid(" Host: localhost");
    assertRequestHeaderIsInvalid("Ho st: localhost");
}

TEST(take_as_failure_a_header_whose_key_contains_HTAB)
{
    assertRequestHeaderIsInvalid("Host\t: localhost");
    assertRequestHeaderIsInvalid("\tHost: localhost");
    assertRequestHeaderIsInvalid("Ho\tst: localhost");
}

TEST(take_as_failure_a_header_whose_key_is_SP)
{
    assertRequestHeaderIsInvalid(" : localhost");
}

TEST(take_as_failure_a_header_whose_key_is_HTAB)
{
    assertRequestHeaderIsInvalid("\t: localhost");
}

TEST(take_as_failure_a_header_whose_key_is_empty)
{
    assertRequestHeaderIsInvalid(": localhost");
}

TEST(take_as_failure_a_header_missing_the_colon_separator)
{
    assertRequestHeaderIsInvalid("Host localhost");
}

TEST(take_as_failure_a_header_that_has_any_other_character_instead_of_the_colon_separator)
{
    assertRequestHeaderIsInvalid("Host? localhost");
    assertRequestHeaderIsInvalid("Host! localhost");
    assertRequestHeaderIsInvalid("Hosta localhost");
}

TEST(take_as_failure_a_header_that_has_SP_instead_of_the_colon_separator)
{
    assertRequestHeaderIsInvalid("Host  localhost");
}

TEST(take_as_failure_a_header_that_has_HTAB_instead_of_the_colon_separator)
{
    assertRequestHeaderIsInvalid("Host\t localhost");
}

TEST(recognize_a_header_with_repeated_colon_separator)
{
    headers = parseFromValidHeaders("Host:: localhost");

    assertHeaderSize(1);
    assertHeader("Host", ": localhost");
}

TEST(recognize_a_header_without_the_first_OWS)
{
    headers = parseFromValidHeaders("Host:localhost");

    assertHeaderSize(1);
    assertHeader("Host", "localhost");
}

TEST(recognize_a_header_with_a_single_SP_as_first_OWS)
{
    headers = parseFromValidHeaders("Host: localhost");

    assertHeaderSize(1);
    assertHeader("Host", "localhost");
}

TEST(recognize_a_header_with_a_single_HTAB_as_first_OWS)
{
    headers = parseFromValidHeaders("Host:\tlocalhost");

    assertHeaderSize(1);
    assertHeader("Host", "localhost");
}

TEST(recognize_a_header_with_a_combination_of_first_OWS)
{
    headers = parseFromValidHeaders("Host: \t   \t\t  \t\tlocalhost");

    assertHeaderSize(1);
    assertHeader("Host", "localhost");
}

TEST(recognize_a_header_whose_value_has_valid_printable_characters_and_HTAB)
{
    const std::string validValue = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ \t[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

    headers = parseFromValidHeaders("Host: " + validValue);

    assertHeaderSize(1);
    assertHeader("Host", validValue);
}

TEST(recognize_a_header_whose_value_has_multiple_words_using_SP_as_separator)
{
    headers = parseFromValidHeaders("Host: localhost and something else");

    assertHeaderSize(1);
    assertHeader("Host", "localhost and something else");
}

TEST(recognize_a_header_whose_value_has_multiple_words_using_HTAB_as_separator)
{
    headers = parseFromValidHeaders("Host: localhost\tand\tsomething\telse");

    assertHeaderSize(1);
    assertHeader("Host", "localhost\tand\tsomething\telse");
}

TEST(recognize_a_header_whose_value_has_multiple_words_using_multiple_OWS_as_separator)
{
    headers = parseFromValidHeaders("Host: localhost\t \tand   \tsomething\t   \t\telse");

    assertHeaderSize(1);
    assertHeader("Host", "localhost\t \tand   \tsomething\t   \t\telse");
}

TEST(take_as_failure_a_header_whose_value_contains_invalid_characters)
{
    assertRequestHeaderIsInvalid("Host: \r\b\n\n\r\x7f\x10");
}

TEST(recognize_a_header_whose_value_is_a_single_SP)
{
    headers = parseFromValidHeaders("Host: ");

    assertHeaderSize(1);
    assertHeader("Host", "");
}

TEST(recognize_a_header_whose_value_is_a_single_HTAB)
{
    headers = parseFromValidHeaders("Host:\t");

    assertHeaderSize(1);
    assertHeader("Host", "");
}

TEST(recognize_a_header_whose_value_is_multiple_OWS)
{
    headers = parseFromValidHeaders("Host:\t \t\t   \t \t");

    assertHeaderSize(1);
    assertHeader("Host", "");
}

TEST(recognize_a_header_whose_value_is_empty)
{
    headers = parseFromValidHeaders("Host:");

    assertHeaderSize(1);
    assertHeader("Host", "");
}

TEST(recognize_a_header_with_a_single_SP_as_last_OWS)
{
    headers = parseFromValidHeaders("Host: localhost ");

    assertHeaderSize(1);
    assertHeader("Host", "localhost");
}

TEST(recognize_a_header_with_a_single_HTAB_as_last_OWS)
{
    headers = parseFromValidHeaders("Host: localhost\t");

    assertHeaderSize(1);
    assertHeader("Host", "localhost");
}

TEST(recognize_a_header_with_a_combination_of_last_OWS)
{
    headers = parseFromValidHeaders("Host: localhost \t   \t\t  \t\t");

    assertHeaderSize(1);
    assertHeader("Host", "localhost");
}

TEST(recognize_multiple_headers_separated_with_CRLF)
{
    headers = parseFromValidHeaders("Host: localhost\r\nContent-Length:0\r\nConnection:\tclose");

    assertHeaderSize(3);
    assertHeader("Host", "localhost");
    assertHeader("Content-Length", "0");
    assertHeader("Connection", "close");
}

TEST(take_as_failure_multiple_headers_with_OWS_as_separator)
{
    assertRequestHeaderIsInvalid("Host: localhost\r\n Content-Length: 0");
    assertRequestHeaderIsInvalid("Host: localhost\r\n\tContent-Length: 0");
}

TEST(take_as_failure_multiple_headers_with_invalid_separator)
{
    assertRequestHeaderIsInvalid("Host: localhost\rContent-Length: 0");
    assertRequestHeaderIsInvalid("Host: localhost\nContent-Length: 0");
    assertRequestHeaderIsInvalid("Host: localhost\b\fContent-Length: 0");
    assertRequestHeaderIsInvalid("Host: localhost\r \nContent-Length: 0");
}

TEST(take_as_failure_multiple_headers_with_an_invalid_middle_header)
{
    assertRequestHeaderIsInvalid("Host: localhost\r\nContent-Length: \b0\r\nConnection: close");
}


/* MISC. REQUEST HEADERS CRITERIA */
TEST(take_as_failure_an_invalid_header_line)
{
    assertRequestHeaderIsInvalid("INVALID");
}

TEST(take_as_failure_a_header_line_consisted_of_OWS)
{
    assertRequestHeaderIsInvalid(" ");
    assertRequestHeaderIsInvalid("\t");
    assertRequestHeaderIsInvalid(" \t  \t    \t \t\t");
}

TEST(take_as_failure_an_empty_header_line)
{
    assertRequestHeaderIsInvalid("");
}


/* REQUEST CHUNKED BODY CRITERIA */
TEST(recognize_a_basic_chunked_body)
{
    body = parseFromValidBody("0\r\n\r\n");

    assertBodyIsEmpty();
}

TEST(recognize_a_basic_chunked_body_with_a_last_chunk_whose_chunk_size_has_multiple_zeros)
{
    body = parseFromValidBody("0000000\r\n\r\n");

    assertBodyIsEmpty();
}

TEST(take_as_failure_a_chunked_body_with_a_last_chunk_whose_chunk_size_has_a_value_different_from_zero)
{
    assertRequestChunkedBodyIsInvalid("5\r\n\r\n");
    assertRequestChunkedBodyIsInvalid("0a\r\n\r\n");
    assertRequestChunkedBodyIsInvalid("x\r\n\r\n");
}

TEST(take_as_failure_a_chunked_body_with_a_last_chunk_whose_chunk_size_has_WS)
{
    assertRequestChunkedBodyIsInvalid("0 \r\n\r\n");
    assertRequestChunkedBodyIsInvalid(" 0\r\n\r\n");
}

TEST(take_as_failure_a_chunked_body_with_a_last_chunk_whose_chunk_size_is_SP)
{
    assertRequestChunkedBodyIsInvalid(" \r\n\r\n");
}

TEST(take_as_failure_a_chunked_body_with_a_last_chunk_whose_chunk_size_is_empty)
{
    assertRequestChunkedBodyIsInvalid("\r\n\r\n");
}

TEST(take_as_failure_a_chunked_body_with_a_last_chunk_whose_last_crlf_is_malformed)
{
    assertRequestChunkedBodyIsInvalid("0\r\r\n");
    assertRequestChunkedBodyIsInvalid("0\n\r\n");
    assertRequestChunkedBodyIsInvalid("0\f\b\r\n");
    assertRequestChunkedBodyIsInvalid("0\r \n\r\n");
}

TEST(take_as_failure_a_chunked_body_with_a_last_chunk_whose_last_crlf_is_SP)
{
    assertRequestChunkedBodyIsInvalid("0 \r\n");
}

TEST(take_as_failure_a_chunked_body_with_a_last_chunk_without_last_crlf)
{
    assertRequestChunkedBodyIsInvalid("0\r\n");
}

TEST(take_as_failure_a_chunked_body_with_a_last_chunk_that_contains_chunk_data)
{
    assertRequestChunkedBodyIsInvalid("0\r\nInvalid\r\n");
}

TEST(recognize_a_chunked_body_with_a_last_chunk_that_has_a_chunk_extension)
{
    body = parseFromValidBody("0;extension\r\n\r\n");

    assertBodyIsEmpty();
}

TEST(recognize_a_chunked_body_with_a_last_chunk_that_has_a_chunk_extension_consisted_of_valid_chars)
{
    const std::string validChars = "!#$%&'*+-.^_`|~0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    body = parseFromValidBody("0;" + validChars + "\r\n\r\n");

    assertBodyIsEmpty();
}

TEST(take_as_failure_a_chunked_body_with_a_last_chunk_that_has_a_chunk_extension_that_contains_invalid_chars)
{
    assertRequestChunkedBodyIsInvalid("0;(),/:;<=>?@[\\]{}\r\n\r\n");
    assertRequestChunkedBodyIsInvalid("0;exten(),/:;<=>?@[\\]{}sion\r\n\r\n");
}

TEST(take_as_failure_a_chunked_body_with_a_last_chunk_that_has_a_chunk_extension_that_contains_SP)
{
    assertRequestChunkedBodyIsInvalid("0;ext en sion\r\n\r\n");
    assertRequestChunkedBodyIsInvalid("0; ext\r\n\r\n");
    assertRequestChunkedBodyIsInvalid("0;ext \r\n\r\n");
}

TEST(take_as_failure_a_chunked_body_with_a_last_chunk_that_has_a_chunk_extension_that_is_SP)
{
    assertRequestChunkedBodyIsInvalid("0; \r\n\r\n");
}

TEST(take_as_failure_a_chunked_body_with_a_last_chunk_that_has_an_empty_chunk_extension)
{
    assertRequestChunkedBodyIsInvalid("0;\r\n\r\n");
}

TEST(recognize_a_chunked_body_with_a_last_chunk_that_has_a_chunk_extension_with_value)
{
    body = parseFromValidBody("0;ext=value\r\n\r\n");

    assertBodyIsEmpty();
}

TEST(recognize_a_chunked_body_with_a_last_chunk_that_has_a_chunk_extension_whose_value_contains_valid_chars)
{
    const std::string validChars = "!#$%&'*+-.^_`|~0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    body = parseFromValidBody("0;ext=" + validChars + "\r\n\r\n");

    assertBodyIsEmpty();
}

TEST(take_as_failure_a_chunked_body_with_a_last_chunk_that_has_a_chunk_extension_whose_value_contains_invalid_chars)
{
    assertRequestChunkedBodyIsInvalid("0;ext=(),/:;<=>?@[\\]{}\r\n\r\n");
    assertRequestChunkedBodyIsInvalid("0;ext=val(),/:;<=>?@[\\]{}ue\r\n\r\n");
}

TEST(take_as_failure_a_chunked_body_with_a_last_chunk_that_has_a_chunk_extension_whose_value_contains_SP)
{
    assertRequestChunkedBodyIsInvalid("0;ext=va l ue\r\n\r\n");
    assertRequestChunkedBodyIsInvalid("0;ext= value\r\n\r\n");
    assertRequestChunkedBodyIsInvalid("0;ext=value \r\n\r\n");
}

TEST(take_as_failure_a_chunked_body_with_a_last_chunk_that_has_a_chunk_extension_whose_value_is_SP)
{
    assertRequestChunkedBodyIsInvalid("0;ext= \r\n\r\n");
}

TEST(take_as_failure_a_chunked_body_with_a_last_chunk_that_has_a_chunk_extension_whose_value_is_empty)
{
    assertRequestChunkedBodyIsInvalid("0;ext=\r\n\r\n");
}

TEST(recognize_a_chunked_body_with_a_last_chunk_that_has_a_chunk_extension_whose_value_is_an_empty_quoted_string)
{
    body = parseFromValidBody("0;ext=\"\"\r\n\r\n");

    assertBodyIsEmpty();
}

TEST(recognize_a_chunked_body_with_a_last_chunk_that_has_a_chunk_extension_whose_value_is_a_non_empty_quoted_string)
{
    body = parseFromValidBody("0;ext=\"validBody\"\r\n\r\n");

    assertBodyIsEmpty();
}

TEST(recognize_a_chunked_body_with_a_last_chunk_that_has_a_chunk_extension_whose_value_is_a_quoted_string_with_valid_qdtext)
{
    const std::string validQdtext = "!#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_`abcdefghijklmnopqrstuvwxyz{|}~ \t";

    body = parseFromValidBody("0;ext=\"" + validQdtext + "\"\r\n\r\n");

    assertBodyIsEmpty();
}

TEST(recognize_a_chunked_body_with_a_last_chunk_that_has_a_chunk_extension_whose_value_is_a_quoted_string_with_valid_quoted_pairs)
{
    body = parseFromValidBody("0;ext=\"\\\\ \\\" \\ \\\t\"\r\n\r\n");

    assertBodyIsEmpty();
}

TEST(take_as_failure_a_chunked_body_with_a_last_chunk_that_has_a_chunk_extension_whose_value_is_a_quoted_string_with_invalid_chars)
{
    assertRequestChunkedBodyIsInvalid("0;ext=\"\r\f\b\x04\x01\"\r\n\r\n");
    assertRequestChunkedBodyIsInvalid("0;ext=\"\\\r\\\b\"\r\n\r\n");
    assertRequestChunkedBodyIsInvalid("0;ext=\"\\\"\r\n\r\n");
    assertRequestChunkedBodyIsInvalid("0;ext=\"\"\"\r\n\r\n");
}

TEST(take_as_failure_a_chunked_body_with_a_last_chunk_that_has_a_chunk_extension_whose_value_is_a_quoted_string_with_surrounding_SP)
{
    assertRequestChunkedBodyIsInvalid("0;ext= \"\"\r\n\r\n");
    assertRequestChunkedBodyIsInvalid("0;ext=\"\" \r\n\r\n");
}

TEST(take_as_failure_a_chunked_body_with_a_last_chunk_that_has_a_chunk_extension_whose_value_is_a_mal_formed_quoted_string)
{
    assertRequestChunkedBodyIsInvalid("0;ext=\"invalid\r\n\r\n");
    assertRequestChunkedBodyIsInvalid("0;ext=invalid\"\r\n\r\n");
    assertRequestChunkedBodyIsInvalid("0;ext=\"\r\n\r\n");
}

TEST(take_as_failure_a_chunked_body_with_a_last_chunk_that_has_a_mal_formed_chunk_extension)
{
    assertRequestChunkedBodyIsInvalid("0;=val\r\n\r\n");
    assertRequestChunkedBodyIsInvalid("0;=\r\n\r\n");
    assertRequestChunkedBodyIsInvalid("0;\"ext\"=val\r\n\r\n");
    assertRequestChunkedBodyIsInvalid("0;ext===\r\n\r\n");
    assertRequestChunkedBodyIsInvalid("0;ext=val=lue\r\n\r\n");
    assertRequestChunkedBodyIsInvalid("0;===\r\n\r\n");
}

TEST(recognize_a_chunked_body_with_a_last_chunk_that_has_multiple_chunk_extensions)
{
    body = parseFromValidBody("0;ext;ext\r\n\r\n");

    assertBodyIsEmpty();
}

TEST(recognize_a_chunked_body_with_a_last_chunk_that_has_multiple_chunk_extensions_with_values)
{
    body = parseFromValidBody("0;ext=val;ext=val\r\n\r\n");

    assertBodyIsEmpty();
}

TEST(recognize_a_chunked_body_with_a_last_chunk_that_has_multiple_chunk_extensions_whose_values_have_valid_chars)
{
    body = parseFromValidBody("0;ext=val;!#$%&'*+-.^_`|~=!#$%&'*+-.^_`|~\r\n\r\n");

    assertBodyIsEmpty();
}

TEST(recognize_a_chunked_body_with_a_last_chunk_that_has_multiple_chunk_extensions_whose_values_have_valid_chars_and_valid_quoted_pairs)
{
    body = parseFromValidBody("0;ext=val;!#$%&'*+-.^_`|~=\"{|}~\t \\\\ \\\"\"\r\n\r\n");

    assertBodyIsEmpty();
}

TEST(take_as_failure_a_chunked_body_with_a_last_chunk_that_has_multiple_chunk_extensions_with_invalid_chars)
{
    assertRequestChunkedBodyIsInvalid("0;ext=val;\r\b=val\r\n\r\n");
    assertRequestChunkedBodyIsInvalid("0;ext=val;ext=\b\f\r\n\r\n");
    assertRequestChunkedBodyIsInvalid("0;ext=val;ext=\"\\\b\"\r\n\r\n");
}

TEST(take_as_failure_a_chunked_body_with_a_last_chunk_that_has_multiple_mal_formed_chunk_extensions)
{
    assertRequestChunkedBodyIsInvalid("0;ext=val;ext=\r\n\r\n");
    assertRequestChunkedBodyIsInvalid("0;ext=val;=val\r\n\r\n");
    assertRequestChunkedBodyIsInvalid("0;ext=val;=\r\n\r\n");
}

TEST(take_as_failure_a_chunked_body_with_a_last_chunk_that_has_multiple_chunk_extension_separators)
{
    assertRequestChunkedBodyIsInvalid("0;ext=val;;;ext=val\r\n\r\n");
}

TEST(take_as_failure_a_chunked_body_with_a_last_chunk_that_has_multiple_chunk_extensions_with_mal_formed_separator)
{
    assertRequestChunkedBodyIsInvalid("0;ext=val ;ext=val\r\n\r\n");
    assertRequestChunkedBodyIsInvalid("0;ext=val; ext=val\r\n\r\n");
}

TEST(take_as_failure_a_chunked_body_with_a_last_chunk_that_has_multiple_chunk_extensions_whose_separator_is_another_character_different_than_colon)
{
    assertRequestChunkedBodyIsInvalid("0;ext=valXext=val\r\n\r\n");
    assertRequestChunkedBodyIsInvalid("0;ext=val\"ext=val\r\n\r\n");
}

TEST(recognize_a_chunked_body_with_a_basic_chunk)
{
    body = parseFromValidBody("5\r\nValid\r\n0\r\n\r\n");

    assertBody("Valid");
}

TEST(recognize_a_chunked_body_with_a_chunk_whose_chunk_size_has_leading_zeroes)
{
    body = parseFromValidBody("000000000000000005\r\nValid\r\n0\r\n\r\n");

    assertBody("Valid");
}

TEST(recognize_a_chunked_body_with_a_chunk_whose_chunk_size_has_multiple_hexadecimal_digits)
{
    body = parseFromValidBody("0a\r\nValid body\r\n0\r\n\r\n");

    assertBody("Valid body");
}

TEST(recognize_a_chunked_body_with_a_chunk_whose_chunk_size_has_multiple_case_insensitive_hexadecimal_digits)
{
    body = parseFromValidBody("0A\r\nValid body\r\n0\r\n\r\n");

    assertBody("Valid body");
}

TEST(take_as_failure_a_chunked_body_with_a_chunk_whose_chunk_size_has_non_hexadecimal_digits)
{
    assertRequestChunkedBodyIsInvalid("0g\r\nInvalid\r\n0\r\n\r\n");
    assertRequestChunkedBodyIsInvalid("0G\r\nInvalid\r\n0\r\n\r\n");
    assertRequestChunkedBodyIsInvalid("-4\r\nInvalid\r\n0\r\n\r\n");
}

TEST(take_as_failure_a_chunked_body_with_a_chunk_whose_chunk_size_has_SP)
{
    assertRequestChunkedBodyIsInvalid("7 \r\nInvalid\r\n0\r\n\r\n");
    assertRequestChunkedBodyIsInvalid(" 7\r\nInvalid\r\n0\r\n\r\n");
}

TEST(take_as_failure_a_chunked_body_with_a_chunk_whose_chunk_size_is_SP)
{
    assertRequestChunkedBodyIsInvalid(" \r\nInvalid\r\n0\r\n\r\n");
}

TEST(take_as_failure_a_chunked_body_with_a_chunk_without_chunk_size)
{
    assertRequestChunkedBodyIsInvalid("\r\nInvalid\r\n0\r\n\r\n");
}
