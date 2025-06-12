#include "utils/colors.hpp"
#include "amethtest/test.hpp"
#include "amethtest/assert/assert.hpp"
#include <iostream>

#define OK GREEN_BOLD << "    [ ✔ ] " << GREEN
#define FAIL RED_BOLD << "    [ ✘ ] " << RED
#define EXCP RED_BOLD << "    [ ! ] "

int main()
{
    int failures = 0;
    std::vector<TestSuite_t> &suites = getTestSuites();

    for (size_t i = 0; i < suites.size(); ++i)
    {
        std::cout << YELLOW_BOLD << "[ SUITE ] " << YELLOW << suites[i].name << RESET << std::endl;

        for (size_t j = 0; j < suites[i].tests.size(); ++j)
        {
            try
            {
                suites[i].tests[j].testFunc();
                std::cout << OK << suites[i].tests[j].name << RESET << std::endl;
            }
            catch (TestFailure &e)
            {
                std::cerr << FAIL << suites[i].tests[j].name << " ---> " << e.what() << RESET << std::endl;
            }
            catch (std::exception &e)
            {
                std::cerr << EXCP << "Uncaught exception in test: " << e.what() << RESET << std::endl;
                ++failures;
            }
        }
        std::cout << std::endl;
    }

    std::cout << "Total: " << YELLOW << suites.size() << RESET
              << " | Failures: " << RED << failures << std::endl;

    return 0;
}