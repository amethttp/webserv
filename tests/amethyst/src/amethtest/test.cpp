#include "test.hpp"
#include <algorithm>

std::vector<TestSuite_t> &getTestSuites()
{
    static std::vector<TestSuite_t> testSuites;

    return testSuites;
}

void registerTest(const std::string &suiteName, const std::string &testName, TestFunc f)
{
    TestCase_t test;
    std::vector<TestSuite_t>& suites = getTestSuites();

    test.name = testName;
    test.testFunc = f;

    for (size_t i = 0; i < suites.size(); ++i) {
        if (suites[i].name == suiteName) {
            suites[i].tests.push_back(test);
            return;
        }
    }

    TestSuite_t newSuite;
    newSuite.name = suiteName;
    newSuite.tests.push_back(test);
    suites.push_back(newSuite);
}
