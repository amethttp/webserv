#pragma once

#include <string>

class Target
{
private:
    std::string uri_;
    std::string path_;
    std::string query_;

public:
    Target();
    ~Target();

    std::string getUri() const;

    void setUri(const std::string &uri);
};
