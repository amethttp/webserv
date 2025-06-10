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
		std::cout << "Some error occurred with config <"
				  << configFile << "> because " << e.what() << "."
				  << std::endl;
	}
	return 0;
}
