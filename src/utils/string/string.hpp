#pragma once

#include <vector>
#include <string>

bool endsWith(const std::string &str, const std::string &suffix);
bool isEmpty(const std::string &str);

std::vector<std::string> split(const std::string &input, const std::string &del);

std::string toLower(const std::string &string);

std::string trim(const std::string &input, const std::string &charsToTrim);

void removeTrailingSlashes(std::string &str);
int countMatchingDepth(std::string base, std::string target);
