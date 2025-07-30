#include <ostream>
#include "Server.hpp"
#include "utils/string/string.hpp"

Server::Server()
{
}

Server::~Server()
{
    for (std::vector<Location *>::iterator it = locations_.begin(); it != locations_.end(); ++it)
        delete *it;
    this->locations_.clear();
}

bool Server::matchesName(std::string &match)
{
    for (std::vector<std::string>::iterator name = this->names_.begin(); name != this->names_.end(); ++name)
    {
        if (*name == match)
            return true;
    }

    return false;
}

// Decide on no locations defined on server
Location *Server::matchLocation(t_Request request)
{
    int matchIndex = 0;
    int matchLength = 0;
    int longestMatch = 0;
    std::string targetRoute = request.requestLine.getTargetPath();
    std::vector<Location *> locations = this->getLocations();

    for (size_t i = 0; i < locations.size(); ++i)
    {
        matchLength = countMatchingDepth(locations[i]->getPath(), targetRoute);
        if (matchLength > longestMatch)
        {
            matchIndex = i;
            longestMatch = matchLength;
        }
    }

    return locations[matchIndex];
}

std::vector<int> Server::getPorts()
{
    return this->ports_;
}

void Server::setPorts(const std::vector<int> &ports)
{
    this->ports_ = ports;
}

std::vector<Location *> Server::getLocations()
{
    return this->locations_;
}

void Server::setLocations(const std::vector<Location *> &locations)
{
    this->locations_ = locations;
}

std::vector<std::string> Server::getNames()
{
    return this->names_;
}

void Server::setNames(const std::vector<std::string> &names)
{
    this->names_ = names;
}

std::string Server::getUploadPath()
{
    return this->uploadPath_;
}

void Server::setUploadPath(const std::string &path)
{
    this->uploadPath_ = path;
}

void Server::setDefaults()
{
    if (ports_.empty())
    {
        ports_.push_back(8000);
        ports_.push_back(80);
    }
    if (names_.empty())
        names_.push_back("");
    if (uploadPath_.empty())
        uploadPath_ = "uploads";
}

std::ostream &operator<<(std::ostream &stream, const Server &server)
{
    stream << "-- SERVER -- \nPorts: {";
    for (size_t i = 0; i < server.ports_.size(); i++)
        stream << server.ports_.at(i) << ", ";
    stream << "}\nNames: {";
    for (size_t i = 0; i < server.names_.size(); i++)
        stream << server.names_.at(i) << ", ";
    stream << "}\nUpload path: " << server.uploadPath_ << std::endl
           << "Locations:" << std::endl;
    for (std::vector<Location *>::const_iterator it = server.locations_.begin(); it != server.locations_.end(); ++it)
        stream << **it;
    stream << std::endl;
    return stream;
}