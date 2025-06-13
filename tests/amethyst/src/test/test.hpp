#pragma once

#include <vector>
#include <string>
#include <utility>

typedef void (*TestFunc)();

typedef struct TestCase_s
{
    std::string name;
    TestFunc testFunc;
} TestCase_t;

typedef struct TestSuite_s
{
    std::string name;
    std::vector<TestCase_t> tests;
} TestSuite_t;

extern std::vector<TestSuite_t> &getTestSuites();

std::string getSuiteName(const std::string &file);

void registerTest(const std::string &suiteName, const std::string &testName, TestFunc f);

#define TEST(name)                                         \
    void name();                                           \
    struct Register_##name                                 \
    {                                                      \
        Register_##name()                                  \
        {                                                  \
            std::string fullPath = getSuiteName(__FILE__); \
            registerTest(fullPath, #name, name);           \
        }                                                  \
    };                                                     \
    static Register_##name instance_##name;                \
    void name()
