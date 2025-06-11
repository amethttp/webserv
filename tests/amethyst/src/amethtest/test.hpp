#pragma once

#include <vector>
#include <string>
#include <utility>

typedef void (*TestFunc)();

extern std::vector<std::pair<std::string, TestFunc> > &getTests();

void registerTest(const std::string& name, TestFunc f);

#define TEST(name) \
    void name(); \
    struct Register_##name \
    { \
        Register_##name() \
        { \
            registerTest(#name, name); \
        } \
    }; \
    static Register_##name instance_##name; \
    void name()
