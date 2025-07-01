#pragma once
#include <map>
#include <set>
#include <vector>
#include <string>
#include <string.h>
#include "utils/http.hpp"

typedef struct return_s
{
	t_httpCode code;
	std::string path;
} return_t;

typedef struct error_page_s
{
	t_httpCode code;
	std::string page;
} error_page_t;

class Location
{
private:
	std::string path_;
	std::string root_;
	std::map<std::string, std::string> cgis_;
	bool autoIndex_;
	std::vector<std::string> indexList_;
	size_t clientMaxBodySize_; // In bytes
	std::set<t_method> methods_;
	return_t return_;
	std::set<error_page_t> errorPages_;

public:
	void setPath(const std::string &path);
	void setRoot(const std::string &root);
	void setAutoIndex(bool mode);
	void setIndexList(const std::vector<std::string> indexes);
	void setMethods(const std::set<t_method> &methods);

	std::string getPath();
	std::string getRoot();
	std::set<t_method> getMethods();
	std::set<error_page_t> getErrorPages();
	bool getAutoIndex();
	std::vector<std::string> getIndexList();
};
