#include <string>
#include <map>
#include "utils/http.hpp"

typedef std::map<std::string, std::string> t_headers;

typedef struct s_body
{
	std::string content;
	std::string type;
} t_body;

class ExecutionResult
{
public:
	t_httpCode code_;
	t_connection mode_;
	t_headers tempHeaders_;
	t_body tempBody_;
	bool isCGI_;

public:
	ExecutionResult();
	~ExecutionResult();
};