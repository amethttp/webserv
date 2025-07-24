#pragma once
#include "../Server.hpp"
#include "WebServer/ConfigParser/models/ConfigNode.hpp"

class ServerMapper
{
private:

public:
	static Server* fromConfigNode(ConfigNode &);
};