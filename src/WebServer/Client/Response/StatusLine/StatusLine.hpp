#include <string>
#include <iostream>
#include "utils/http.hpp"

class StatusLine
{
private:
	httpCode_t code_;
	std::string httpVersion_;
	std::string statusMessage_;
public:
	StatusLine();
	~StatusLine();

	void setFields(httpCode_t code, const std::string &message);
	httpCode_t getCode();

	friend std::string &operator+=(std::string &string, StatusLine &statusLine);
	friend std::ostream &operator<<(std::ostream &stream, StatusLine &statusLine);
};
