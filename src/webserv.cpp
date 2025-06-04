#include <errno.h>
#include <string.h>
#include <iostream>
#include "WebServer/WebServer.hpp"

int main(void)
{
    try
    {
        WebServer webServer = WebServer();

        // std::vector<Server> servers;
        // Server server;

        // server.ports_.push_back(8080);
        // server.ports_.push_back(9000);
        // webServer.servers_.push_back(server);

        // server.ports_.clear();
        // server.ports_.push_back(3004);
        // webServer.servers_.push_back(server);

        // server.ports_.clear();
        // server.ports_.push_back(4444);
        // server.ports_.push_back(5555);
        // server.ports_.push_back(6000);
        // webServer.servers_.push_back(server);

        // server.ports_.clear();
        // server.ports_.push_back(3000);
        // server.ports_.push_back(3241);
        // webServer.servers_.push_back(server);

        webServer.serve();
    }
    catch(std::exception &e)
    {
        std::cerr << "Error: " << e.what()
            << ": " << strerror(errno) << std::endl;
    }

    return 0;
}
