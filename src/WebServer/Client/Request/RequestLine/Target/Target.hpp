#pragma once

#include <string>

class Target
{
private:
    std::string uri_;
    std::string path_;
    std::string query_;

    void separateUriComponents();

public:
    Target();
    ~Target();

    std::string getUri() const;
    std::string getUriPath() const;
    std::string getUriQuery() const;

    void setUri(const std::string &uri);

        void setPath(const std::string &path)
        {
            this->path_ = path;
        }

        void setQuery(const std::string &query)
        {
            this->query_ = query;
        }
};
