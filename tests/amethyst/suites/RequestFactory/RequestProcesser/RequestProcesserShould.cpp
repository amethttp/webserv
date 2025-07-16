#include "test/test.hpp"
#include "test/assert/assert.hpp"
#include "WebServer/Client/Request/Request.hpp"
#include "WebServer/Client/Request/RequestFactory/RequestFactory.hpp"
#include "WebServer/Client/Request/RequestFactory/RequestProcesser/RequestProcesser.hpp"

namespace
{
    Target_t target;
    HeaderCollection headers;
}

static void processFromValidTargetUri(const std::string &requestTargetUri)
{
    target.path = "";
    target.query = "";
    target.uri = requestTargetUri;

    RequestProcesser::processRequestTarget(target);
}

static void processValidHeader(const std::string &headerKey, const std::string &headerValue)
{
    headers.removeHeaders();
    headers.addHeader("Host: localhost");
    headers.addHeader(headerKey + ": " + headerValue);

    RequestProcesser::processHeaders(headers);
}

static void assertTargetComponents(const std::string &path, const std::string &query)
{
    ASSERT_EQUALS(path, target.path);
    ASSERT_EQUALS(query, target.query);
}

static void assertHeader(const std::string &key, const std::string &value)
{
    ASSERT_EQUALS(value, headers.getHeaderValue(key));
}

static void assertRequestIsInvalidFromTargetUri(const std::string &invalidRequestTargetUri)
{
    target.uri = invalidRequestTargetUri;

    const SimpleResult result = RequestProcesser::processRequestTarget(target);

    ASSERT_TRUE(result.isFailure());
    ASSERT_EQUALS("400 Bad Request", result.getError());
}


/* REQUEST TARGET PATH-QUERY SEPARATION TESTS */
TEST(separate_a_simple_path_from_an_empty_query)
{
    processFromValidTargetUri("/?");

    assertTargetComponents("/", "");
}

TEST(match_path_to_uri_if_there_is_no_query)
{
    processFromValidTargetUri("/index.html");

    assertTargetComponents("/index.html", "");
}

TEST(separate_a_complex_path_from_an_empty_query)
{
    processFromValidTargetUri("/index.html?");

    assertTargetComponents("/index.html", "");
}

TEST(separate_a_path_from_a_non_empty_query)
{
    processFromValidTargetUri("/index.html?anyQuery");

    assertTargetComponents("/index.html", "anyQuery");
}

TEST(separate_a_path_from_a_query_with_multiple_question_mark_characters)
{
    processFromValidTargetUri("/index.html?any?Query?");

    assertTargetComponents("/index.html", "any?Query?");
}


/* REQUEST PCT-DECODING TESTS */
TEST(leave_the_same_file_target_if_it_does_not_contain_pct_encoded_pchars)
{
    processFromValidTargetUri("/index.html");

    assertTargetComponents("/index.html", "");
}

TEST(leave_the_same_directory_target_if_it_does_not_contain_pct_encoded_pchars)
{
    processFromValidTargetUri("/courses/biology/");

    assertTargetComponents("/courses/biology/", "");
}

TEST(decode_valid_pct_encoded_pchars)
{
    processFromValidTargetUri("/index/%20_%22_%25_%3c_%3e_%5b_%5c_%5d_%7b_%7d");

    assertTargetComponents("/index/ _\"_%_<_>_[_\\_]_{_}", "");
}

TEST(decode_valid_case_insensitive_pct_encoded_pchars)
{
    processFromValidTargetUri("/index/%3c_%3C_%3e_%3E_%5c_%5C_%7b_%7B");

    assertTargetComponents("/index/<_<_>_>_\\_\\_{_{", "");
}

TEST(take_as_failure_a_target_path_with_pct_encoded_control_chars)
{
    assertRequestIsInvalidFromTargetUri("/index/%0d");
    assertRequestIsInvalidFromTargetUri("/index/%0A");
    assertRequestIsInvalidFromTargetUri("/index/%00");
    assertRequestIsInvalidFromTargetUri("/index/%7f");
}

TEST(leave_the_same_target_query_if_it_does_contain_valid_pct_encoded_pchars)
{
    processFromValidTargetUri("/index/query?%20_%22_%25_%3c_%3e_%5b_%5c_%5d_%7b_%7d");

    assertTargetComponents("/index/query", "%20_%22_%25_%3c_%3e_%5b_%5c_%5d_%7b_%7d");
}

TEST(leave_the_same_target_query_if_it_does_contain_valid_case_insensitive_pct_encoded_pchars)
{
    processFromValidTargetUri("/index/query?%3c_%3C_%3e_%3E_%5c_%5C_%7b_%7B");

    assertTargetComponents("/index/query", "%3c_%3C_%3e_%3E_%5c_%5C_%7b_%7B");
}

TEST(take_as_failure_a_target_query_with_pct_encoded_control_chars)
{
    assertRequestIsInvalidFromTargetUri("/index/query?%0d");
    assertRequestIsInvalidFromTargetUri("/index/query?%0A");
    assertRequestIsInvalidFromTargetUri("/index/query?%00");
    assertRequestIsInvalidFromTargetUri("/index/query?%7f");
}

TEST(decode_valid_pct_encoded_pchars_inside_absolute_path_and_leave_the_same_target_query)
{
    processFromValidTargetUri("/index/%25?%20");

    assertTargetComponents("/index/%", "%20");
}

TEST(leave_the_same_path_and_query_if_the_query_contains_pct_encoded_question_marks)
{
    processFromValidTargetUri("/index.html?query%3fvar");

    assertTargetComponents("/index.html", "query%3fvar");
}

TEST(decode_the_target_path_pct_encoded_question_marks_without_separating_the_path_from_the_query)
{
    processFromValidTargetUri("/index.html%3fquery%3fvar");

    assertTargetComponents("/index.html?query?var", "");
}

TEST(decode_the_target_path_pct_encoded_question_marks_separating_the_path_from_the_query_on_the_first_not_pct_encoded_question_mark)
{
    processFromValidTargetUri("/index.html%3fquery?var");

    assertTargetComponents("/index.html?query", "var");
}


/* REQUEST PATH NORMALIZATION TESTS */
TEST(leave_the_same_basic_path_if_it_does_not_need_to_be_normalized)
{
    processFromValidTargetUri("/");

    assertTargetComponents("/", "");
}

TEST(leave_the_same_file_path_if_it_does_not_need_to_be_normalized)
{
    processFromValidTargetUri("/index.html");

    assertTargetComponents("/index.html", "");
}

TEST(leave_the_same_directory_path_if_it_does_not_need_to_be_normalized)
{
    processFromValidTargetUri("/courses/biology/");

    assertTargetComponents("/courses/biology/", "");
}

TEST(normalize_a_single_current_directory)
{
    processFromValidTargetUri("/.");

    assertTargetComponents("/", "");
}

TEST(normalize_a_single_current_directory_ended_with_slash)
{
    processFromValidTargetUri("/./");

    assertTargetComponents("/", "");
}

TEST(normalize_a_multiple_current_directory)
{
    processFromValidTargetUri("/././.");

    assertTargetComponents("/", "");
}

TEST(normalize_a_multiple_current_directory_ended_with_slash)
{
    processFromValidTargetUri("/./././");

    assertTargetComponents("/", "");
}

TEST(normalize_a_single_current_directory_after_text)
{
    processFromValidTargetUri("/courses/.");

    assertTargetComponents("/courses/", "");
}

TEST(normalize_a_single_current_directory_after_text_ended_with_slash)
{
    processFromValidTargetUri("/courses/./");

    assertTargetComponents("/courses/", "");
}

TEST(normalize_a_multiple_current_directory_after_text)
{
    processFromValidTargetUri("/courses/././.");

    assertTargetComponents("/courses/", "");
}

TEST(normalize_a_multiple_current_directory_after_text_ended_with_slash)
{
    processFromValidTargetUri("/courses/./././");

    assertTargetComponents("/courses/", "");
}

TEST(normalize_a_single_current_directory_before_text)
{
    processFromValidTargetUri("/./courses");

    assertTargetComponents("/courses", "");
}

TEST(normalize_a_multiple_current_directory_before_text)
{
    processFromValidTargetUri("/./././courses");

    assertTargetComponents("/courses", "");
}

TEST(normalize_a_multiple_current_directory_without_final_slash_before_text)
{
    processFromValidTargetUri("/./././.courses");

    assertTargetComponents("/.courses", "");
}

TEST(normalize_a_single_current_directory_between_text)
{
    processFromValidTargetUri("/courses/./index.html");

    assertTargetComponents("/courses/index.html", "");
}

TEST(normalize_a_multiple_current_directory_between_text)
{
    processFromValidTargetUri("/courses/./././index.html");

    assertTargetComponents("/courses/index.html", "");
}

TEST(normalize_a_multiple_current_directory_without_final_slash_between_text)
{
    processFromValidTargetUri("/courses/././.index.html");

    assertTargetComponents("/courses/.index.html", "");
}

TEST(normalize_a_single_current_directory_before_and_after_text)
{
    processFromValidTargetUri("/./courses/./");

    assertTargetComponents("/courses/", "");
}

TEST(normalize_a_multiple_current_directory_before_and_after_text)
{
    processFromValidTargetUri("/././courses/././");

    assertTargetComponents("/courses/", "");
}

TEST(normalize_a_single_parent_directory)
{
    processFromValidTargetUri("/..");

    assertTargetComponents("/", "");
}

TEST(normalize_a_single_parent_directory_ended_with_slash)
{
    processFromValidTargetUri("/../");

    assertTargetComponents("/", "");
}

TEST(normalize_a_multiple_parent_directory)
{
    processFromValidTargetUri("/../../..");

    assertTargetComponents("/", "");
}

TEST(normalize_a_single_parent_directory_after_text)
{
    processFromValidTargetUri("/courses/..");

    assertTargetComponents("/", "");
}

TEST(normalize_a_single_parent_directory_after_text_ended_with_slash)
{
    processFromValidTargetUri("/courses/../");

    assertTargetComponents("/", "");
}

TEST(normalize_a_multiple_parent_directory_after_text)
{
    processFromValidTargetUri("/courses/../../..");

    assertTargetComponents("/", "");
}

TEST(normalize_a_multiple_parent_directory_after_text_ended_with_slash)
{
    processFromValidTargetUri("/courses/../../../");

    assertTargetComponents("/", "");
}

TEST(normalize_a_single_parent_directory_before_text)
{
    processFromValidTargetUri("/../courses");

    assertTargetComponents("/courses", "");
}

TEST(normalize_a_multiple_parent_directory_before_text)
{
    processFromValidTargetUri("/../../../courses");

    assertTargetComponents("/courses", "");
}

TEST(normalize_a_multiple_parent_directory_without_final_slash_before_text)
{
    processFromValidTargetUri("/../../..courses");

    assertTargetComponents("/..courses", "");
}

TEST(normalize_a_single_parent_directory_between_text)
{
    processFromValidTargetUri("/courses/../index.html");

    assertTargetComponents("/index.html", "");
}

TEST(normalize_a_multiple_parent_directory_between_text)
{
    processFromValidTargetUri("/courses/../../../index.html");

    assertTargetComponents("/index.html", "");
}

TEST(normalize_a_multiple_parent_directory_without_final_slash_between_text)
{
    processFromValidTargetUri("/courses/../../..index.html");

    assertTargetComponents("/..index.html", "");
}

TEST(normalize_a_single_parent_directory_before_and_after_text)
{
    processFromValidTargetUri("/../courses/../");

    assertTargetComponents("/", "");
}

TEST(normalize_a_multiple_parent_directory_before_and_after_text)
{
    processFromValidTargetUri("/../../courses/../../");

    assertTargetComponents("/", "");
}

TEST(normalize_a_multiple_slashes_path)
{
    processFromValidTargetUri("////");

    assertTargetComponents("/", "");
}

TEST(normalize_a_multiple_slashes_path_after_text)
{
    processFromValidTargetUri("/courses//////");

    assertTargetComponents("/courses/", "");
}

TEST(normalize_a_multiple_slashes_path_before_text)
{
    processFromValidTargetUri("/////courses");

    assertTargetComponents("/courses", "");
}

TEST(normalize_a_multiple_slashes_path_between_text)
{
    processFromValidTargetUri("/courses/////index.html");

    assertTargetComponents("/courses/index.html", "");
}

TEST(normalize_a_multiple_slashes_path_before_and_after_text)
{
    processFromValidTargetUri("///courses///");

    assertTargetComponents("/courses/", "");
}

TEST(normalize_a_combination_of_multiple_current_directories_and_parent_directories_and_slashes)
{
    processFromValidTargetUri("/./../courses/../////././about/index.php");

    assertTargetComponents("/about/index.php", "");
}

TEST(normalize_a_pct_encoded_current_directory)
{
    processFromValidTargetUri("/%2e/about/index.html");

    assertTargetComponents("/about/index.html", "");
}

TEST(normalize_a_pct_encoded_parent_directory)
{
    processFromValidTargetUri("/courses/%2e%2e/about/index.html");

    assertTargetComponents("/about/index.html", "");
}

TEST(normalize_a_pct_encoded_slash)
{
    processFromValidTargetUri("/courses/%2f/%2Findex.html");

    assertTargetComponents("/courses/index.html", "");
}

TEST(normalize_a_combination_of_pct_encoded_and_not_pct_encoded_current_directories_and_parent_directories_and_slashes)
{
    processFromValidTargetUri("//../%2e%2e/about/./%2e/%2F//index.html");

    assertTargetComponents("/about/index.html", "");
}

TEST(normalize_a_resource_outside_the_document_root)
{
    processFromValidTargetUri("/../../../../..%2f..%2F%2e%2E/%2e%2E/%2e%2E/%2e%2E/%2e%2E/etc/passwd");

    assertTargetComponents("/etc/passwd", "");
}


/* REQUEST HEADERS TESTS */
TEST(recognize_a_request_with_valid_case_insensitive_transfer_encoding_header)
{
    processValidHeader("Transfer-Encoding", "cHUnKeD");

    assertHeader("Host", "localhost");
}

TEST(recognize_a_request_with_valid_connection_header_with_case_insensitive_keep_alive_value)
{
    processValidHeader("Connection", "KeEP-ALivE");

    assertHeader("Connection", "keep-alive");
}

TEST(recognize_a_request_with_valid_connection_header_with_case_insensitive_close_value)
{
    processValidHeader("Connection", "cLOSe");

    assertHeader("Connection", "close");
}
