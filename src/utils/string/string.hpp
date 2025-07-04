#pragma once

#include <vector>
#include <string>

bool isEmpty(const std::string &str);

bool endsWith(const std::string &str, const std::string &suffix);

std::vector<std::string> split(const std::string &input, const std::string &del);

std::string trim(const std::string &input, const std::string &charsToTrim);

std::string toHttpHeaderCase(const std::string &string);
