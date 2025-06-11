#include "test.hpp"

std::vector<std::pair<std::string, TestFunc> > &getTests() {
    static std::vector<std::pair<std::string, TestFunc> > tests;

    return tests;
}

void registerTest(const std::string &name, TestFunc f) {
    getTests().push_back(std::make_pair(name, f));
}
