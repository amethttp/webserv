#pragma once

#include "test/TestFailure/TestFailure.hpp"
#include <iostream>
#include <sstream>

#define LOG_IDENTATION "          "

#define ASSERT_TRUE(condition)                                                   \
    if ((condition) == true)                                                     \
        return;                                                                  \
                                                                                 \
    std::ostringstream oss;                                                      \
    oss << __FILE__ << ":" << __LINE__ << "\n"                                   \
        << LOG_IDENTATION << "Reason: ASSERT_TRUE(" << #condition << ") failed"; \
                                                                                 \
    throw TestFailure(oss.str());

#define ASSERT_FALSE(condition)                                                   \
    if ((condition) == false)                                                     \
        return;                                                                   \
                                                                                  \
    std::ostringstream oss;                                                       \
    oss << __FILE__ << ":" << __LINE__ << "\n"                                    \
        << LOG_IDENTATION << "Reason: ASSERT_FALSE(" << #condition << ") failed"; \
                                                                                  \
    throw TestFailure(oss.str());

#define ASSERT_EQUALS(expected, actual)                                                               \
    if ((expected) == (actual))                                                                       \
        return;                                                                                       \
                                                                                                      \
    std::ostringstream oss;                                                                           \
    oss << __FILE__ << ":" << __LINE__ << "\n"                                                        \
        << LOG_IDENTATION << "Reason: ASSERT_EQUALS(" << #expected << ", " << #actual << ") failed\n" \
        << LOG_IDENTATION << "Expected: " << (expected) << "\n"                                       \
        << LOG_IDENTATION << "Actual: " << (actual);                                                  \
                                                                                                      \
    throw TestFailure(oss.str());

#define ASSERT_NOT_EQUALS(expected, actual)                                                               \
    if ((expected) != (actual))                                                                           \
        return;                                                                                           \
                                                                                                          \
    std::ostringstream oss;                                                                               \
    oss << __FILE__ << ":" << __LINE__ << "\n"                                                            \
        << LOG_IDENTATION << "Reason: ASSERT_NOT_EQUALS(" << #expected << ", " << #actual << ") failed\n" \
        << LOG_IDENTATION << "Expected: " << (expected) << "\n"                                           \
        << LOG_IDENTATION << "Actual: " << (actual);                                                      \
                                                                                                          \
    throw TestFailure(oss.str());

#define ASSERT_THROWS(SUT, excp)                                                                                               \
    std::ostringstream oss;                                                                                                    \
                                                                                                                               \
    try                                                                                                                        \
    {                                                                                                                          \
        SUT();                                                                                                                 \
        oss << __FILE__ << ":" << __LINE__ << "\n"                                                                             \
            << LOG_IDENTATION << "Reason: ASSERT_THROWS(" << #SUT << ", " << #excp << ") failed: Didn't throw an exception\n"; \
    }                                                                                                                          \
    catch (excp & e)                                                                                                           \
    {                                                                                                                          \
        return;                                                                                                                \
    }                                                                                                                          \
    catch (const std::exception &e)                                                                                            \
    {                                                                                                                          \
        oss << __FILE__ << ":" << __LINE__ << "\n"                                                                             \
            << LOG_IDENTATION << "Reason: ASSERT_THROWS(" << #SUT << ", " << #excp << ") failed: Threw a wrong exception\n";   \
    }                                                                                                                          \
                                                                                                                               \
    throw TestFailure(oss.str());
