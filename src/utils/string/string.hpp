#pragma once

#include <vector>
#include <string>

#define SPACE_VALUES " \f\n\r\t\v"

bool endsWith(const std::string &str, const std::string &suffix);

std::vector<std::string> split(const std::string &input, const std::string &del);

std::string toLower(const std::string &string);

std::string trim(const std::string &input, const std::string &charsToTrim);
std::vector<std::string> splitSet(const std::string &input, const std::string &delimiters);
std::string getSpaceValues();
void removeDoubleSlashes(std::string &str);
void removeTrailingSlashes(std::string &str);
int countMatchingDepth(std::string base, std::string target);

char *newCstr(std::string str);
void freeArr(char **arr);
