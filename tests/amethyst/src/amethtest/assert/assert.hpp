#pragma once

#include "utils/colors.hpp"
#include <iostream>

#define FAIL RED_BOLD << "[ FAIL ] " << RED

#define ASSERT_TRUE(condition) \
    if ((condition) == false) \
    { \
        std::cerr << FAIL << __FILE__ << ":" << __LINE__ \
                  << ": ASSERT_TRUE(" << #condition << ") failed" << RESET << std::endl; \
        return; \
    }

#define ASSERT_FALSE(condition) \
    if ((condition) == true) \
    { \
        std::cerr << FAIL << __FILE__ << ":" << __LINE__ \
                  << ": ASSERT_FALSE(" << #condition << ") failed" << RESET << std::endl; \
        return; \
    }

#define ASSERT_EQUALS(expected, actual) \
    if (!((expected) == (actual))) \
    { \
        std::cerr << FAIL << __FILE__ << ":" << __LINE__ \
                  << ": ASSERT_EQUALS(" << #expected << ", " << #actual << ") failed\n" \
                  << "         Expected: " << (expected) << "\n" \
                  << "         Actual: " << (actual) << RESET << std::endl; \
        return; \
    }

#define ASSERT_NOT_EQUALS(expected, actual) \
    if (!((expected) != (actual))) \
    { \
        std::cerr << FAIL << __FILE__ << ":" << __LINE__ \
                  << ": ASSERT_NOT_EQUALS(" << #expected << ", " << #actual << ") failed\n" \
                  << "         Expected: " << (expected) << "\n" \
                  << "         Actual: " << (actual) << RESET << std::endl; \
        return; \
    }
