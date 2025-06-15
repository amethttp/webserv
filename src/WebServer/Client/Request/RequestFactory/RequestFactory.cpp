#include "RequestFactory.hpp"

Result<RequestInfo_t> RequestFactory::create(const std::string &requestBuffer)
{
    RequestInfo_t requestInfo;
    
    if (requestBuffer.find("GET") == 0)
        requestInfo.request.method = GET;
    else
        requestInfo.request.method = DELETE;

    requestInfo.request.target = "/";
    requestInfo.request.httpVersion = "HTTP/1.1";
    requestInfo.request.headers["Host"] = "localhost";
    return Result<RequestInfo_t>::ok(requestInfo);
}