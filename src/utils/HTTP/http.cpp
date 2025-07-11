#include "http.hpp"

method_t getHttpMethodFromString(const std::string &requestMethodString)
{
    if (requestMethodString == "GET")
        return GET;
    if (requestMethodString == "POST")
        return POST;
    if (requestMethodString == "DELETE")
        return DELETE;
    return NOT_IMPLEMENTED;
}
