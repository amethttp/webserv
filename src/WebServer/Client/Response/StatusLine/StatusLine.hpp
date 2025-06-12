#include <string>
#include <iostream>

class StatusLine
{
private:
	int code_;
	std::string httpVersion_;
	std::string statusMessage_;
public:
	StatusLine();
	~StatusLine();

	void setStatusLine(int code, const std::string &message);

	friend std::string &operator+=(std::string &string, StatusLine &statusLine);
	friend std::ostream &operator<<(std::ostream &stream, StatusLine &statusLine);
};
