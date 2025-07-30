#include "http.hpp"

t_method getHTTPMethod(const std::string &method)
{
	if (method == "GET")
		return M_GET;
	else if (method == "POST")
		return M_POST;
	else if (method == "DELETE")
		return M_DELETE;
	return M_NOT_IMPLEMENTED;
}