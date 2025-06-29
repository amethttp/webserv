#pragma once

#include <string>

bool isLong(const std::string &str);

bool isHexdig(char ch);

bool isHex(const std::string &str);

size_t hexToDec(const std::string &hexNumber);

char hexToChar(char firstDigit, char secondDigit);
