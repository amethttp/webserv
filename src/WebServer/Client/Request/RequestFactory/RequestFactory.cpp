#include "RequestFactory.hpp"

Result<RequestInfo_t> RequestFactory::create(const std::string &requestBuffer)
{
    RequestInfo_t requestInfo;
    
    if (requestBuffer.find("GET") == 0)
        requestInfo.request.method = GET;
    else if (requestBuffer.find("POST") == 0)
        requestInfo.request.method = POST;
    else
        requestInfo.request.method = DELETE;

    requestInfo.request.target = "/";
    requestInfo.request.httpVersion = "HTTP/1.1";
    requestInfo.request.headers["Host"] = "localhost";

    if (requestInfo.request.method == POST)
        requestInfo.request.headers["Content-Length"] = "0";

    return Result<RequestInfo_t>::ok(requestInfo);
}