#pragma once
#include <map>
#include <set>
#include <vector>
#include <string>
#include "utils/http/http.hpp"
#include <string.h>

typedef struct s_return
{
	t_httpCode code;
	std::string path;

	s_return()
	{
		code =  0;
		path = "";
	}
} t_return;

typedef struct s_error_page
{
	t_httpCode code;
	std::string page;
	bool operator<(const s_error_page &other) const
	{
		return code < other.code || (code == other.code && page < other.page);
	}
} t_error_page;

class Location
{
private:
	std::string path_;
	std::string root_;
	std::map<std::string, std::string> cgis_;
	bool autoIndex_;
	std::vector<std::string> indexList_;
	size_t connectionMaxBodySize_; // In bytes
	std::set<t_method> methods_;
	t_return return_;
	std::set<t_error_page> errorPages_;

public:
	Location();
	~Location();

	void setDefaults();
	void setPath(const std::string &path);
	void setRoot(const std::string &root);
	void setAutoIndex(bool mode);
	void setIndexList(const std::vector<std::string> indexes);
	void setMethods(const std::set<t_method> &methods);
	void setReturn(const t_return &ret);
	void setMaxBodySize(const size_t size);
	void setCGIs(std::map<std::string, std::string> cgis);
	void setErrorPages(const std::set<t_error_page> errorPages_);

	std::string getPath() const;
	std::string getRoot() const;
	bool getAutoIndex() const;
	std::vector<std::string> getIndexList() const;
	std::set<t_method> getMethods() const;
	t_return getReturn() const;
	size_t getMaxBodySize() const;
	std::map<std::string, std::string> getCGIs() const;
	std::set<t_error_page> getErrorPages() const;

	friend std::ostream &operator<<(std::ostream &, const Location &);
};
