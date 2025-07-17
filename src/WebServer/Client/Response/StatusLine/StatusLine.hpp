#include <string>
#include <iostream>
#include "utils/http.hpp"

class StatusLine
{
private:
	t_httpCode code_;
	std::string httpVersion_;
	std::string statusMessage_;
public:
	StatusLine();
	~StatusLine();

	void clear();
	void setFields(t_httpCode code, const std::string &message);

	t_httpCode getCode() const;
	const std::string &getHttpVersion() const;
	const std::string &getStatusMessage() const;

	friend std::string &operator+=(std::string &string, const StatusLine &statusLine);
	friend std::ostream &operator<<(std::ostream &stream, const StatusLine &statusLine);
};
