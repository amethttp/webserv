#include "ResponseFactory.hpp"

Response ResponseFactory::create(HandlingResult &res)
{
	// body and stuff
    return generateResponse(res.code, res.mode);
}

Response ResponseFactory::create(t_httpCode code, t_connection mode)
{
    return generateResponse(code, mode);
}