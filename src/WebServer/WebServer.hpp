#pragma once

#include <vector>
#include "utils/fd.hpp"
#include "Server/Server.hpp"

using namespace std;

class WebServer
{
private:
	vector<Server> servers;
	vector<fd_t> listeners;

public:
	vector<Server> getServers();
	vector<fd_t> getListeners();

	void serve();
};
