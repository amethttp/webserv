#include "test.hpp"
#include <algorithm>

std::vector<TestSuite_t> &getTestSuites()
{
    static std::vector<TestSuite_t> testSuites;

    return testSuites;
}

std::string getSuiteName(const std::string &file)
{
    std::string fullPath = file;
    std::string suiteName = fullPath;
    size_t pos = fullPath.find_last_of("/\\");

    if (pos != std::string::npos)
        suiteName = fullPath.substr(pos + 1);

    size_t dot = suiteName.find_last_of('.');
    suiteName = suiteName.substr(0, dot);
    suiteName[0] = toupper(suiteName[0]);

    return suiteName;
}

void registerTest(const std::string &suiteName, const std::string &testName, TestFunc f)
{
    TestCase_t test;
    std::vector<TestSuite_t> &suites = getTestSuites();

    test.name = testName;
    test.testFunc = f;

    for (size_t i = 0; i < suites.size(); ++i)
    {
        if (suites[i].name == suiteName)
        {
            suites[i].tests.push_back(test);
            return;
        }
    }

    TestSuite_t newSuite;
    newSuite.name = suiteName;
    newSuite.tests.push_back(test);
    suites.push_back(newSuite);
}
