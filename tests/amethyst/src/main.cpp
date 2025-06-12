#include "amethtest/test.hpp"
#include "amethtest/assert/assert.hpp"
#include <iostream>

int main() {
    int failures = 0;
    std::vector<TestSuite_t> &suites = getTestSuites();

    for (size_t i = 0; i < suites.size(); ++i) {
        std::cout << YELLOW_BOLD << "[ SUITE ] " << YELLOW << suites[i].name << RESET << std::endl;

        try {
            for (size_t j = 0; j < suites[i].tests.size(); ++j) {
                suites[i].tests[j].testFunc();
                std::cout << GREEN_BOLD << "    [ ✔ ] " << GREEN << suites[i].tests[j].name << RESET << std::endl;
            }
        } catch (std::exception &e) {
            std::cerr << RED_BOLD << "    [ ! ] Uncaught exception in test: " << e.what() << RESET << std::endl;
            ++failures;
        }
        std::cout << std::endl;
    }

    std::cout << "Total: " << YELLOW << suites.size() << RESET
              << " | Failures: " << RED << failures << std::endl;

    return failures ? 1 : 0;
}