#include "test/test.hpp"
#include "test/assert/assert.hpp"
#include "WebServer/Client/Request/Request.hpp"
#include "WebServer/Client/Request/RequestFactory/RequestFactory.hpp"

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
    ASSERT_EQUALS(size, request.headers.size());
}

static void assertHeader(const std::string &key, const std::string &value)
{
    ASSERT_EQUALS(value, request.headers.at(key).back());
}

static void assertRequestIsInvalid(const std::string &invalidRequestString)
{
    Result<Request_t> result = RequestFactory::create(invalidRequestString);

    ASSERT_TRUE(result.isFailure());
    ASSERT_EQUALS("400 Bad Request", result.getError());
}

static void assertRequestIsInvalidFromRequestLine(const std::string &invalidRequestLineString)
{
    const std::string invalidRequestString = invalidRequestLineString + "\r\nHost: localhost\r\n\r\n";

    assertRequestIsInvalid(invalidRequestString);
}


/* REQUEST TARGET PATH-QUERY SEPARATION TESTS */
TEST(separate_a_simple_path_from_an_empty_query)
{
    request = createRequestFromValidRequestLine("GET /? HTTP/1.1");

    assertTargetComponents("/", "");
    assertRequestLine(GET, "/?", "HTTP/1.1");
}

TEST(match_path_to_uri_if_there_is_no_query)
{
    request = createRequestFromValidRequestLine("GET /index.html HTTP/1.1");

    assertTargetComponents("/index.html", "");
    assertRequestLine(GET, "/index.html", "HTTP/1.1");
}

TEST(separate_a_complex_path_from_an_empty_query)
{
    request = createRequestFromValidRequestLine("GET /index.html? HTTP/1.1");

    assertTargetComponents("/index.html", "");
    assertRequestLine(GET, "/index.html?", "HTTP/1.1");
}

TEST(separate_a_path_from_a_non_empty_query)
{
    request = createRequestFromValidRequestLine("GET /index.html?anyQuery HTTP/1.1");

    assertTargetComponents("/index.html", "anyQuery");
    assertRequestLine(GET, "/index.html?anyQuery", "HTTP/1.1");
}

TEST(separate_a_path_from_a_query_with_multiple_question_mark_characters)
{
    request = createRequestFromValidRequestLine("GET /index.html?any?Query? HTTP/1.1");

    assertTargetComponents("/index.html", "any?Query?");
    assertRequestLine(GET, "/index.html?any?Query?", "HTTP/1.1");
}


/* REQUEST PCT-DECODING TESTS */
TEST(leave_the_same_file_target_if_it_does_not_contain_pct_encoded_pchars)
{
    request = createRequestFromValidRequestLine("GET /index.html HTTP/1.1");

    assertTargetComponents("/index.html", "");
    assertRequestLine(GET, "/index.html", "HTTP/1.1");
}

TEST(leave_the_same_directory_target_if_it_does_not_contain_pct_encoded_pchars)
{
    request = createRequestFromValidRequestLine("GET /courses/biology/ HTTP/1.1");

    assertTargetComponents("/courses/biology/", "");
    assertRequestLine(GET, "/courses/biology/", "HTTP/1.1");
}

TEST(decode_valid_pct_encoded_pchars)
{
    request = createRequestFromValidRequestLine("GET /index/%20_%22_%25_%3c_%3e_%5b_%5c_%5d_%7b_%7d HTTP/1.1");

    assertTargetComponents("/index/ _\"_%_<_>_[_\\_]_{_}", "");
    assertRequestLine(GET, "/index/%20_%22_%25_%3c_%3e_%5b_%5c_%5d_%7b_%7d", "HTTP/1.1");
}

TEST(decode_valid_case_insensitive_pct_encoded_pchars)
{
    request = createRequestFromValidRequestLine("GET /index/%3c_%3C_%3e_%3E_%5c_%5C_%7b_%7B HTTP/1.1");

    assertTargetComponents("/index/<_<_>_>_\\_\\_{_{", "");
    assertRequestLine(GET, "/index/%3c_%3C_%3e_%3E_%5c_%5C_%7b_%7B", "HTTP/1.1");
}

TEST(take_as_failure_a_target_path_with_pct_encoded_control_chars)
{
    assertRequestIsInvalidFromRequestLine("GET /index/%0d HTTP/1.1");
    assertRequestIsInvalidFromRequestLine("GET /index/%0A HTTP/1.1");
    assertRequestIsInvalidFromRequestLine("GET /index/%00 HTTP/1.1");
    assertRequestIsInvalidFromRequestLine("GET /index/%7f HTTP/1.1");
}

TEST(leave_the_same_target_query_if_it_does_contain_valid_pct_encoded_pchars)
{
    request = createRequestFromValidRequestLine("GET /index/query?%20_%22_%25_%3c_%3e_%5b_%5c_%5d_%7b_%7d HTTP/1.1");

    assertTargetComponents("/index/query", "%20_%22_%25_%3c_%3e_%5b_%5c_%5d_%7b_%7d");
    assertRequestLine(GET, "/index/query?%20_%22_%25_%3c_%3e_%5b_%5c_%5d_%7b_%7d", "HTTP/1.1");
}

TEST(leave_the_same_target_query_if_it_does_contain_valid_case_insensitive_pct_encoded_pchars)
{
    request = createRequestFromValidRequestLine("GET /index/query?%3c_%3C_%3e_%3E_%5c_%5C_%7b_%7B HTTP/1.1");

    assertTargetComponents("/index/query", "%3c_%3C_%3e_%3E_%5c_%5C_%7b_%7B");
    assertRequestLine(GET, "/index/query?%3c_%3C_%3e_%3E_%5c_%5C_%7b_%7B", "HTTP/1.1");
}

TEST(take_as_failure_a_target_query_with_pct_encoded_control_chars)
{
    assertRequestIsInvalidFromRequestLine("GET /index/query?%0d HTTP/1.1");
    assertRequestIsInvalidFromRequestLine("GET /index/query?%0A HTTP/1.1");
    assertRequestIsInvalidFromRequestLine("GET /index/query?%00 HTTP/1.1");
    assertRequestIsInvalidFromRequestLine("GET /index/query?%7f HTTP/1.1");
}

TEST(decode_valid_pct_encoded_pchars_inside_absolute_path_and_leave_the_same_target_query)
{
    request = createRequestFromValidRequestLine("GET /index/%25?%20 HTTP/1.1");

    assertTargetComponents("/index/%", "%20");
    assertRequestLine(GET, "/index/%25?%20", "HTTP/1.1");
}

TEST(leave_the_same_path_and_query_if_the_query_contains_pct_encoded_question_marks)
{
    request = createRequestFromValidRequestLine("GET /index.html?query%3fvar HTTP/1.1");

    assertTargetComponents("/index.html", "query%3fvar");
    assertRequestLine(GET, "/index.html?query%3fvar", "HTTP/1.1");
}

TEST(decode_the_target_path_pct_encoded_question_marks_without_separating_the_path_from_the_query)
{
    request = createRequestFromValidRequestLine("GET /index.html%3fquery%3fvar HTTP/1.1");

    assertTargetComponents("/index.html?query?var", "");
    assertRequestLine(GET, "/index.html%3fquery%3fvar", "HTTP/1.1");
}

TEST(decode_the_target_path_pct_encoded_question_marks_separating_the_path_from_the_query_on_the_first_not_pct_encoded_question_mark)
{
    request = createRequestFromValidRequestLine("GET /index.html%3fquery?var HTTP/1.1");

    assertTargetComponents("/index.html?query", "var");
    assertRequestLine(GET, "/index.html%3fquery?var", "HTTP/1.1");
}


/* REQUEST TARGET PATH NORMALIZATION TESTS */
TEST(leave_the_same_basic_path_if_it_does_not_need_to_be_normalized)
{
    request = createRequestFromValidRequestLine("GET / HTTP/1.1");

    assertTargetComponents("/", "");
    assertRequestLine(GET, "/", "HTTP/1.1");
}

TEST(leave_the_same_file_path_if_it_does_not_need_to_be_normalized)
{
    request = createRequestFromValidRequestLine("GET /index.html HTTP/1.1");

    assertTargetComponents("/index.html", "");
    assertRequestLine(GET, "/index.html", "HTTP/1.1");
}

TEST(leave_the_same_directory_path_if_it_does_not_need_to_be_normalized)
{
    request = createRequestFromValidRequestLine("GET /courses/biology/ HTTP/1.1");

    assertTargetComponents("/courses/biology/", "");
    assertRequestLine(GET, "/courses/biology/", "HTTP/1.1");
}

TEST(normalize_a_single_current_directory)
{
    request = createRequestFromValidRequestLine("GET /. HTTP/1.1");

    assertTargetComponents("/", "");
    assertRequestLine(GET, "/.", "HTTP/1.1");
}

TEST(normalize_a_single_current_directory_ended_with_slash)
{
    request = createRequestFromValidRequestLine("GET /./ HTTP/1.1");

    assertTargetComponents("/", "");
    assertRequestLine(GET, "/./", "HTTP/1.1");
}

TEST(normalize_a_multiple_current_directory)
{
    request = createRequestFromValidRequestLine("GET /././. HTTP/1.1");

    assertTargetComponents("/", "");
    assertRequestLine(GET, "/././.", "HTTP/1.1");
}

TEST(normalize_a_multiple_current_directory_ended_with_slash)
{
    request = createRequestFromValidRequestLine("GET /./././ HTTP/1.1");

    assertTargetComponents("/", "");
    assertRequestLine(GET, "/./././", "HTTP/1.1");
}

TEST(normalize_a_single_current_directory_after_text)
{
    request = createRequestFromValidRequestLine("GET /courses/. HTTP/1.1");

    assertTargetComponents("/courses/", "");
    assertRequestLine(GET, "/courses/.", "HTTP/1.1");
}

TEST(normalize_a_single_current_directory_after_text_ended_with_slash)
{
    request = createRequestFromValidRequestLine("GET /courses/./ HTTP/1.1");

    assertTargetComponents("/courses/", "");
    assertRequestLine(GET, "/courses/./", "HTTP/1.1");
}

TEST(normalize_a_multiple_current_directory_after_text)
{
    request = createRequestFromValidRequestLine("GET /courses/././. HTTP/1.1");

    assertTargetComponents("/courses/", "");
    assertRequestLine(GET, "/courses/././.", "HTTP/1.1");
}

TEST(normalize_a_multiple_current_directory_after_text_ended_with_slash)
{
    request = createRequestFromValidRequestLine("GET /courses/./././ HTTP/1.1");

    assertTargetComponents("/courses/", "");
    assertRequestLine(GET, "/courses/./././", "HTTP/1.1");
}

TEST(normalize_a_single_current_directory_before_text)
{
    request = createRequestFromValidRequestLine("GET /./courses HTTP/1.1");

    assertTargetComponents("/courses", "");
    assertRequestLine(GET, "/./courses", "HTTP/1.1");
}

TEST(normalize_a_multiple_current_directory_before_text)
{
    request = createRequestFromValidRequestLine("GET /./././courses HTTP/1.1");

    assertTargetComponents("/courses", "");
    assertRequestLine(GET, "/./././courses", "HTTP/1.1");
}

TEST(normalize_a_multiple_current_directory_without_final_slash_before_text)
{
    request = createRequestFromValidRequestLine("GET /./././.courses HTTP/1.1");

    assertTargetComponents("/.courses", "");
    assertRequestLine(GET, "/./././.courses", "HTTP/1.1");
}

TEST(normalize_a_single_current_directory_between_text)
{
    request = createRequestFromValidRequestLine("GET /courses/./index.html HTTP/1.1");

    assertTargetComponents("/courses/index.html", "");
    assertRequestLine(GET, "/courses/./index.html", "HTTP/1.1");
}

TEST(normalize_a_multiple_current_directory_between_text)
{
    request = createRequestFromValidRequestLine("GET /courses/./././index.html HTTP/1.1");

    assertTargetComponents("/courses/index.html", "");
    assertRequestLine(GET, "/courses/./././index.html", "HTTP/1.1");
}

TEST(normalize_a_multiple_current_directory_without_final_slash_between_text)
{
    request = createRequestFromValidRequestLine("GET /courses/././.index.html HTTP/1.1");

    assertTargetComponents("/courses/.index.html", "");
    assertRequestLine(GET, "/courses/././.index.html", "HTTP/1.1");
}

TEST(normalize_a_single_current_directory_before_and_after_text)
{
    request = createRequestFromValidRequestLine("GET /./courses/./ HTTP/1.1");

    assertTargetComponents("/courses/", "");
    assertRequestLine(GET, "/./courses/./", "HTTP/1.1");
}

TEST(normalize_a_multiple_current_directory_before_and_after_text)
{
    request = createRequestFromValidRequestLine("GET /././courses/././ HTTP/1.1");

    assertTargetComponents("/courses/", "");
    assertRequestLine(GET, "/././courses/././", "HTTP/1.1");
}

TEST(normalize_a_single_parent_directory)
{
    request = createRequestFromValidRequestLine("GET /.. HTTP/1.1");

    assertTargetComponents("/", "");
    assertRequestLine(GET, "/..", "HTTP/1.1");
}

TEST(normalize_a_single_parent_directory_ended_with_slash)
{
    request = createRequestFromValidRequestLine("GET /../ HTTP/1.1");

    assertTargetComponents("/", "");
    assertRequestLine(GET, "/../", "HTTP/1.1");
}

TEST(normalize_a_multiple_parent_directory)
{
    request = createRequestFromValidRequestLine("GET /../../.. HTTP/1.1");

    assertTargetComponents("/", "");
    assertRequestLine(GET, "/../../..", "HTTP/1.1");
}

TEST(normalize_a_single_parent_directory_after_text)
{
    request = createRequestFromValidRequestLine("GET /courses/.. HTTP/1.1");

    assertTargetComponents("/", "");
    assertRequestLine(GET, "/courses/..", "HTTP/1.1");
}

TEST(normalize_a_single_parent_directory_after_text_ended_with_slash)
{
    request = createRequestFromValidRequestLine("GET /courses/../ HTTP/1.1");

    assertTargetComponents("/", "");
    assertRequestLine(GET, "/courses/../", "HTTP/1.1");
}

TEST(normalize_a_multiple_parent_directory_after_text)
{
    request = createRequestFromValidRequestLine("GET /courses/../../.. HTTP/1.1");

    assertTargetComponents("/", "");
    assertRequestLine(GET, "/courses/../../..", "HTTP/1.1");
}

TEST(normalize_a_multiple_parent_directory_after_text_ended_with_slash)
{
    request = createRequestFromValidRequestLine("GET /courses/../../../ HTTP/1.1");

    assertTargetComponents("/", "");
    assertRequestLine(GET, "/courses/../../../", "HTTP/1.1");
}

TEST(normalize_a_single_parent_directory_before_text)
{
    request = createRequestFromValidRequestLine("GET /../courses HTTP/1.1");

    assertTargetComponents("/courses", "");
    assertRequestLine(GET, "/../courses", "HTTP/1.1");
}

TEST(normalize_a_multiple_parent_directory_before_text)
{
    request = createRequestFromValidRequestLine("GET /../../../courses HTTP/1.1");

    assertTargetComponents("/courses", "");
    assertRequestLine(GET, "/../../../courses", "HTTP/1.1");
}

TEST(normalize_a_multiple_parent_directory_without_final_slash_before_text)
{
    request = createRequestFromValidRequestLine("GET /../../..courses HTTP/1.1");

    assertTargetComponents("/..courses", "");
    assertRequestLine(GET, "/../../..courses", "HTTP/1.1");
}

TEST(normalize_a_single_parent_directory_between_text)
{
    request = createRequestFromValidRequestLine("GET /courses/../index.html HTTP/1.1");

    assertTargetComponents("/index.html", "");
    assertRequestLine(GET, "/courses/../index.html", "HTTP/1.1");
}

TEST(normalize_a_multiple_parent_directory_between_text)
{
    request = createRequestFromValidRequestLine("GET /courses/../../../index.html HTTP/1.1");

    assertTargetComponents("/index.html", "");
    assertRequestLine(GET, "/courses/../../../index.html", "HTTP/1.1");
}

TEST(normalize_a_multiple_parent_directory_without_final_slash_between_text)
{
    request = createRequestFromValidRequestLine("GET /courses/../../..index.html HTTP/1.1");

    assertTargetComponents("/..index.html", "");
    assertRequestLine(GET, "/courses/../../..index.html", "HTTP/1.1");
}

TEST(normalize_a_single_parent_directory_before_and_after_text)
{
    request = createRequestFromValidRequestLine("GET /../courses/../ HTTP/1.1");

    assertTargetComponents("/", "");
    assertRequestLine(GET, "/../courses/../", "HTTP/1.1");
}

TEST(normalize_a_multiple_parent_directory_before_and_after_text)
{
    request = createRequestFromValidRequestLine("GET /../../courses/../../ HTTP/1.1");

    assertTargetComponents("/", "");
    assertRequestLine(GET, "/../../courses/../../", "HTTP/1.1");
}

TEST(normalize_a_multiple_slashes_path)
{
    request = createRequestFromValidRequestLine("GET //// HTTP/1.1");

    assertTargetComponents("/", "");
    assertRequestLine(GET, "////", "HTTP/1.1");
}

TEST(normalize_a_multiple_slashes_path_after_text)
{
    request = createRequestFromValidRequestLine("GET /courses////// HTTP/1.1");

    assertTargetComponents("/courses/", "");
    assertRequestLine(GET, "/courses//////", "HTTP/1.1");
}

TEST(normalize_a_multiple_slashes_path_before_text)
{
    request = createRequestFromValidRequestLine("GET /////courses HTTP/1.1");

    assertTargetComponents("/courses", "");
    assertRequestLine(GET, "/////courses", "HTTP/1.1");
}

TEST(normalize_a_multiple_slashes_path_between_text)
{
    request = createRequestFromValidRequestLine("GET /courses/////index.html HTTP/1.1");

    assertTargetComponents("/courses/index.html", "");
    assertRequestLine(GET, "/courses/////index.html", "HTTP/1.1");
}

TEST(normalize_a_multiple_slashes_path_before_and_after_text)
{
    request = createRequestFromValidRequestLine("GET ///courses/// HTTP/1.1");

    assertTargetComponents("/courses/", "");
    assertRequestLine(GET, "///courses///", "HTTP/1.1");
}

TEST(normalize_a_combination_of_multiple_current_directories_and_parent_directories_and_slashes)
{
    request = createRequestFromValidRequestLine("GET /./../courses/../////././about/index.php HTTP/1.1");

    assertTargetComponents("/about/index.php", "");
    assertRequestLine(GET, "/./../courses/../////././about/index.php", "HTTP/1.1");
}

TEST(normalize_a_pct_encoded_current_directory)
{
    request = createRequestFromValidRequestLine("GET /%2e/about/index.html HTTP/1.1");

    assertTargetComponents("/about/index.html", "");
    assertRequestLine(GET, "/%2e/about/index.html", "HTTP/1.1");
}

TEST(normalize_a_pct_encoded_parent_directory)
{
    request = createRequestFromValidRequestLine("GET /courses/%2e%2e/about/index.html HTTP/1.1");

    assertTargetComponents("/about/index.html", "");
    assertRequestLine(GET, "/courses/%2e%2e/about/index.html", "HTTP/1.1");
}

TEST(normalize_a_pct_encoded_slash)
{
    request = createRequestFromValidRequestLine("GET /courses/%2f/%2Findex.html HTTP/1.1");

    assertTargetComponents("/courses/index.html", "");
    assertRequestLine(GET, "/courses/%2f/%2Findex.html", "HTTP/1.1");
}

TEST(normalize_a_combination_of_pct_encoded_and_not_pct_encoded_current_directories_and_parent_directories_and_slashes)
{
    request = createRequestFromValidRequestLine("GET //../%2e%2e/about/./%2e/%2F//index.html HTTP/1.1");

    assertTargetComponents("/about/index.html", "");
    assertRequestLine(GET, "//../%2e%2e/about/./%2e/%2F//index.html", "HTTP/1.1");
}

TEST(normalize_a_resource_outside_the_document_root)
{
    request = createRequestFromValidRequestLine("GET /../../../../..%2f..%2F%2e%2E/%2e%2E/%2e%2E/%2e%2E/%2e%2E/etc/passwd HTTP/1.1");

    assertTargetComponents("/etc/passwd", "");
    assertRequestLine(GET, "/../../../../..%2f..%2F%2e%2E/%2e%2E/%2e%2E/%2e%2E/%2e%2E/etc/passwd", "HTTP/1.1");
}


/* REQUEST HEADERS */
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
