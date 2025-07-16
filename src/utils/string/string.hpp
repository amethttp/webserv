#pragma once

#include <vector>
#include <string>

bool isEmpty(const std::string &str);

std::string toLowerString(const std::string &str);

std::vector<std::string> split(const std::string &input, const std::string &del);
std::string trim(const std::string &input, const std::string &charsToTrim);
