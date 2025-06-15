#pragma once

#include "test/TestFailure/TestFailure.hpp"
#include <iostream>
#include <sstream>

#define LOG_IDENTATION "          "

static std::ostringstream oss;

#define ASSERT_TRUE(condition)                                                       \
    if ((condition) == false)                                                        \
    {                                                                                \
        oss << __FILE__ << ":" << __LINE__ << "\n"                                   \
            << LOG_IDENTATION << "Reason: ASSERT_TRUE(" << #condition << ") failed"; \
                                                                                     \
        throw TestFailure(oss.str());                                                \
    }

#define ASSERT_FALSE(condition)                                                       \
    if ((condition) == true)                                                          \
    {                                                                                 \
        oss << __FILE__ << ":" << __LINE__ << "\n"                                    \
            << LOG_IDENTATION << "Reason: ASSERT_FALSE(" << #condition << ") failed"; \
                                                                                      \
        throw TestFailure(oss.str());                                                 \
    }

#define ASSERT_EQUALS(expected, actual)                                                                   \
    if ((expected) != (actual))                                                                           \
    {                                                                                                     \
        oss << __FILE__ << ":" << __LINE__ << "\n"                                                        \
            << LOG_IDENTATION << "Reason: ASSERT_EQUALS(" << #expected << ", " << #actual << ") failed\n" \
            << LOG_IDENTATION << "Expected: " << (expected) << "\n"                                       \
            << LOG_IDENTATION << "Actual: " << (actual);                                                  \
                                                                                                          \
        throw TestFailure(oss.str());                                                                     \
    }

#define ASSERT_NOT_EQUALS(expected, actual)                                                                   \
    if ((expected) == (actual))                                                                               \
    {                                                                                                         \
        oss << __FILE__ << ":" << __LINE__ << "\n"                                                            \
            << LOG_IDENTATION << "Reason: ASSERT_NOT_EQUALS(" << #expected << ", " << #actual << ") failed\n" \
            << LOG_IDENTATION << "Expected: " << (expected) << "\n"                                           \
            << LOG_IDENTATION << "Actual: " << (actual);                                                      \
                                                                                                              \
        throw TestFailure(oss.str());                                                                         \
    }

// TODO: Refactor 
#define ASSERT_THROWS(SUT, excp)                                                                                               \
                                                                                                                               \
    try                                                                                                                        \
    {                                                                                                                          \
        SUT;                                                                                                                   \
        oss << __FILE__ << ":" << __LINE__ << "\n"                                                                             \
            << LOG_IDENTATION << "Reason: ASSERT_THROWS(" << #SUT << ", " << #excp << ") failed: Didn't throw an exception\n"; \
        throw TestFailure(oss.str());                                                                                          \
    }                                                                                                                          \
    catch (excp & e)                                                                                                           \
    {                                                                                                                          \
    }                                                                                                                          \
    catch (TestFailure & e)                                                                                                    \
    {                                                                                                                          \
        throw TestFailure(oss.str());                                                                                          \
    }                                                                                                                          \
    catch (const std::exception &e)                                                                                            \
    {                                                                                                                          \
        oss << __FILE__ << ":" << __LINE__ << "\n"                                                                             \
            << LOG_IDENTATION << "Reason: ASSERT_THROWS(" << #SUT << ", " << #excp << ") failed: Threw a wrong exception\n";   \
        throw TestFailure(oss.str());                                                                                          \
    }
