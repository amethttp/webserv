#include "amethtest/test.hpp"
#include "amethtest/assert/assert.hpp"
#include <iostream>

int main() {
    int failures = 0;
    std::vector<std::pair<std::string, TestFunc> > &tests = getTests();

    for (size_t i = 0; i < tests.size(); ++i) {
        std::cout << GREY << "[ TEST ] " << tests[i].first << RESET << std::endl;
        try {
            tests[i].second();
            std::cout << GREEN_BOLD << "[  OK  ] " << GREEN << tests[i].first << RESET << std::endl;
        } catch (...) {
            std::cerr << RED_BOLD << "[ EXCP ] Uncaught exception in test" << RESET << std::endl;
            ++failures;
        }
    }

    std::cout << std::endl;
    std::cout << "Total: " << GREY << tests.size()
              << " | Failures: " << RED << failures << std::endl;

    return failures ? 1 : 0;
}