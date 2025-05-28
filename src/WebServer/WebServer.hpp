#pragma once

#include <vector>
#include "utils/fd.hpp"
#include "Server/Server.hpp"

class WebServer
{
private:
	std::vector<Server> servers_;
	std::vector<fd_t> listeners_;

public:
	std::vector<Server> getServers();
	std::vector<fd_t> getListeners();

	void serve();
};
