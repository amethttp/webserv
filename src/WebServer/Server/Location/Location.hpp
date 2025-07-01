#pragma once
#include <map>
#include <set>
#include <vector>
#include <string>
#include <string.h>
#include "utils/http.hpp"

typedef struct s_return
{
	t_httpCode code;
	std::string path;
} t_return;

typedef struct s_error_page
{
	t_httpCode code;
	std::string page;
} t_error_page;

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
	t_return return_;
	std::set<t_error_page> errorPages_;

public:
	void setPath(const std::string &path);
	void setRoot(const std::string &root);
	void setAutoIndex(bool mode);
	void setIndexList(const std::vector<std::string> indexes);
	void setMethods(const std::set<t_method> &methods);

	std::string getPath();
	std::string getRoot();
	std::set<t_method> getMethods();
	std::set<t_error_page> getErrorPages();
	bool getAutoIndex();
	std::vector<std::string> getIndexList();
};
