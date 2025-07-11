#pragma once

#include <string>
#include <vector>

class Header
{
private:
    std::string key_;
    std::vector<std::string> values_;

    static std::string toHeaderCase(const std::string &string);

public:
    Header(const std::string &key);
    ~Header();

    const std::string &getKey() const;
    const std::string &getValue() const;
    size_t getAmountOfValues() const;

    void addValue(const std::string &newValue);
    void removeValues();

    static std::string getHeaderKey(const std::string &headerString);
    static std::string getHeaderValue(const std::string &headerString);
};
