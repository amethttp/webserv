#pragma once

#define BAD_REQUEST_ERR "400 Bad Request"
#define NOT_IMPLEMENTED_ERR "501 Not Implemented"
#define URI_TOO_LONG_ERR "414 URI Too Long"
#define HTTP_VERSION_NOT_SUPPORTED_ERR "505 HTTP Version Not Supported"
#define LENGTH_REQUIRED_ERR "411 Length Required"

typedef enum method_e
{
	GET,
	POST,
	DELETE,
	NOT_IMPLEMENTED
} method_t;
