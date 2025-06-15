#pragma once

#include "test/TestFailure/TestFailure.hpp"
#include <iostream>
#include <sstream>

#define LOG_IDENTATION "          "

#define ASSERT_TRUE(condition)                                                           \
    {                                                                                    \
        std::ostringstream oss;                                                          \
                                                                                         \
        if ((condition) == false)                                                        \
        {                                                                                \
            oss << __FILE__ << ":" << __LINE__ << "\n"                                   \
                << LOG_IDENTATION << "Reason: ASSERT_TRUE(" << #condition << ") failed"; \
                                                                                         \
            throw TestFailure(oss.str());                                                \
        }                                                                                \
    }

#define ASSERT_FALSE(condition)                                                           \
    {                                                                                     \
        std::ostringstream oss;                                                           \
                                                                                          \
        if ((condition) == true)                                                          \
        {                                                                                 \
            oss << __FILE__ << ":" << __LINE__ << "\n"                                    \
                << LOG_IDENTATION << "Reason: ASSERT_FALSE(" << #condition << ") failed"; \
                                                                                          \
            throw TestFailure(oss.str());                                                 \
        }                                                                                 \
    }

#define ASSERT_EQUALS(expected, actual)                                                                       \
    {                                                                                                         \
        std::ostringstream oss;                                                                               \
                                                                                                              \
        if ((expected) != (actual))                                                                           \
        {                                                                                                     \
            oss << __FILE__ << ":" << __LINE__ << "\n"                                                        \
                << LOG_IDENTATION << "Reason: ASSERT_EQUALS(" << #expected << ", " << #actual << ") failed\n" \
                << LOG_IDENTATION << "Expected: " << (expected) << "\n"                                       \
                << LOG_IDENTATION << "Actual: " << (actual);                                                  \
                                                                                                              \
            throw TestFailure(oss.str());                                                                     \
        }                                                                                                     \
    }

#define ASSERT_NOT_EQUALS(expected, actual)                                                                       \
    {                                                                                                             \
        std::ostringstream oss;                                                                                   \
                                                                                                                  \
        if ((expected) == (actual))                                                                               \
        {                                                                                                         \
            oss << __FILE__ << ":" << __LINE__ << "\n"                                                            \
                << LOG_IDENTATION << "Reason: ASSERT_NOT_EQUALS(" << #expected << ", " << #actual << ") failed\n" \
                << LOG_IDENTATION << "Expected: " << (expected) << "\n"                                           \
                << LOG_IDENTATION << "Actual: " << (actual);                                                      \
                                                                                                                  \
            throw TestFailure(oss.str());                                                                         \
        }                                                                                                         \
    }

#define ASSERT_THROWS(SUT, excp)                                                                                                   \
    {                                                                                                                              \
        std::ostringstream oss;                                                                                                    \
        bool shouldThrow = false;                                                                                       \
                                                                                                                                   \
        try                                                                                                                        \
        {                                                                                                                          \
            SUT;                                                                                                                   \
            oss << __FILE__ << ":" << __LINE__ << "\n"                                                                             \
                << LOG_IDENTATION << "Reason: ASSERT_THROWS(" << #SUT << ", " << #excp << ") failed: Didn't throw an exception\n"; \
            shouldThrow = true;                                                                                         \
        }                                                                                                                          \
        catch (const excp &e)                                                                                                      \
        {                                                                                                                          \
        }                                                                                                                          \
        catch (const std::exception &e)                                                                                            \
        {                                                                                                                          \
            oss << __FILE__ << ":" << __LINE__ << "\n"                                                                             \
                << LOG_IDENTATION << "Reason: ASSERT_THROWS(" << #SUT << ", " << #excp << ") failed: Threw a wrong exception\n";   \
            shouldThrow = true;                                                                                         \
        }                                                                                                                          \
                                                                                                                                   \
        if (shouldThrow)                                                                                                \
            throw TestFailure(oss.str());                                                                                          \
    }
