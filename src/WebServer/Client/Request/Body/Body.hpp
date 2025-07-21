#pragma once

#include <string>

class Body
{
private:
    std::string message_;

    static bool isNotHex(char c);

public:
    Body();
    ~Body();

    std::string getMessage();
    static size_t getChunkSize(const std::string &chunkSize);

    void addFragment(const std::string &fragment);
};
