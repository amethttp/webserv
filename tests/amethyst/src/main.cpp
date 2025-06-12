#include "utils/colors.hpp"
#include "amethtest/test.hpp"
#include "amethtest/assert/assert.hpp"
#include <iostream>

#define TEST_IDENTATION "    "
#define OK GREEN_BOLD << TEST_IDENTATION << "[ ✔ ] " << GREEN
#define FAIL RED_BOLD << TEST_IDENTATION << "[ ✘ ] " << RED
#define EXCP RED_BOLD << TEST_IDENTATION << "[ ! ] "

static size_t executeTests(std::vector<TestCase_t> &tests)
{
    size_t passed = 0;

    for (size_t j = 0; j < tests.size(); ++j)
    {
        try
        {
            tests[j].testFunc();
            ++passed;
            std::cout << OK << tests[j].name << RESET << std::endl;
        }
        catch (TestFailure &e)
        {
            std::cerr << FAIL << tests[j].name << " ---> " << e.what() << RESET << std::endl;
        }
        catch (std::exception &e)
        {
            std::cerr << EXCP << "Uncaught exception in test: " << e.what() << RESET << std::endl;
        }
    }
    return passed;
}

static void showTestsResults(size_t totalTests, size_t passed)
{
    size_t failures = totalTests - passed;

    std::cout << "Total: " << YELLOW << totalTests << RESET
              << " | Passed: " << GREEN << passed << RESET
              << " | Failures: " << RED << failures << std::endl;
}

int main()
{
    size_t passed = 0;
    size_t totalTests = 0;
    std::vector<TestSuite_t> &suites = getTestSuites();

    for (size_t i = 0; i < suites.size(); ++i)
    {
        std::cout << YELLOW_BOLD << "[ SUITE ] " << YELLOW << suites[i].name << RESET << std::endl;
        
        passed += executeTests(suites[i].tests);
        totalTests += suites[i].tests.size();

        std::cout << std::endl;
    }

    showTestsResults(totalTests, passed);
    return 0;
}
