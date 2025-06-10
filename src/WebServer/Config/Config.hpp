#pragma once
#include <string>
#include <map>
#include <set>
#include <vector>
#include <string>
#include "utils/http.hpp"
#include "utils/config.hpp"

class Config
{
protected:
	std::string path_;
	std::string root_;
	std::map<std::string, std::string> cgis_;
	bool autoIndex_;
	std::vector<std::string> index_;
	size_t clientMaxBodySize_; // In bytes
	std::set<method_t> methods_;
	return_t return_;
	std::vector<error_page_t> errorPages_;
public:
	Config();
	virtual void setConfig(const std::vector<std::string> &words);
	~Config();
};