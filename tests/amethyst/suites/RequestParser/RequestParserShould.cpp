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

static void assertRequestLineIsInvalid(const std::string &invalidRequestString, const std::string &errorMessage)
{
    RequestTokenizer requestTokenizer(invalidRequestString);
    RequestParser sut(requestTokenizer);

    Result<RequestLineParams_t> result = sut.parseRequestLine();

    ASSERT_TRUE(result.isFailure());
    ASSERT_EQUALS(errorMessage, result.getError());
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
TEST(take_as_failure_a_case_insensitive_method)
{
    assertRequestLineIsInvalid("get / HTTP/1.1", "501 Not Implemented");
}

TEST(take_as_failure_a_not_implemented_method_consisted_only_of_alphabetic_characters)
{
    assertRequestLineIsInvalid("INVALID / HTTP/1.1", "501 Not Implemented");
}

TEST(take_as_failure_a_not_implemented_method_consisted_of_tchars)
{
    const std::string tchars = "!#$%&'*+-.^_`|~0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    const std::string invalidRequestLineString = tchars + " / HTTP/1.1";

    assertRequestLineIsInvalid(invalidRequestLineString, "501 Not Implemented");
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

TEST(take_as_failure_a_target_longer_than_maximum_length)
{
    const char anyValidCharacter = 'A';
    const std::string target = "/" + std::string(MAX_URI_LENGTH, anyValidCharacter);
    const std::string invalidRequestLine = "GET " + target + " HTTP/1.1";

    assertRequestLineIsInvalid(invalidRequestLine, "414 URI Too Long");
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

TEST(take_as_failure_a_non_supported_major_version)
{
    assertRequestLineIsInvalid("GET / HTTP/0.1", "505 HTTP Version Not Supported");
    assertRequestLineIsInvalid("GET / HTTP/2.1", "505 HTTP Version Not Supported");
    assertRequestLineIsInvalid("GET / HTTP/7.1", "505 HTTP Version Not Supported");
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

TEST(take_as_failure_a_non_supported_minor_version)
{
    assertRequestLineIsInvalid("GET / HTTP/1.0", "505 HTTP Version Not Supported");
    assertRequestLineIsInvalid("GET / HTTP/1.2", "505 HTTP Version Not Supported");
    assertRequestLineIsInvalid("GET / HTTP/1.7", "505 HTTP Version Not Supported");
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
