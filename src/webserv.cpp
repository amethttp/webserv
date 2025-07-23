#include <iostream>
#include <limits.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "WebServer/WebServer.hpp"

int main(int argc, char *argv[])
{
	WebServer webserver;
	std::string configFile;
	try
	{
		if (argc > 1)
			configFile = argv[1];
		else
			configFile = "assets/default.conf";

		webserver.setConfigFromFile(configFile);
		webserver.serve();
	}
	catch (std::exception &e)
	{
		std::cerr << "webserv: "
				  << e.what()
				  << std::endl;
		return 1;
	}
	return 0;
}
