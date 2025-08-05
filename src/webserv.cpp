#include <errno.h>
#include <string.h>
#include <iostream>
#include "WebServer/WebServer.hpp"
#include "WebServer/ConfigParser/ConfigParser.hpp"

int main(int argc, char *argv[])
{

    try
    {
        WebServer webserver;
        std::string configFile;
        if (argc > 1)
            configFile = argv[1];
        else
            configFile = "assets/configs/default.conf";
        ConfigParser::useConfig(configFile, webserver);
        webserver.serve();
    }
    catch (std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
