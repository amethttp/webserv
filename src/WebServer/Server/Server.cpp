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
    for (std::map<int, Session *>::const_iterator it = this->sessions_.begin(); it != this->sessions_.end(); ++it)
        delete (*it).second;
    this->sessions_.clear();
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

Location *Server::matchLocation(std::string targetRoute) const
{
    int matchIndex = 0;
    int matchLength = 0;
    int longestMatch = 0;
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

std::set<int> Server::getPorts() const
{
    return this->ports_;
}

void Server::setPorts(const std::set<int> &ports)
{
    this->ports_ = ports;
}

std::vector<Location *> Server::getLocations() const
{
    return this->locations_;
}

void Server::setLocations(const std::vector<Location *> &locations)
{
    this->locations_ = locations;
}

std::vector<std::string> Server::getNames() const
{
    return this->names_;
}

void Server::setNames(const std::vector<std::string> &names)
{
    this->names_ = names;
}

std::string Server::getUploadPath() const
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
        ports_.insert(8000);
        ports_.insert(80);
    }
    if (names_.empty())
        names_.push_back("");
    if (uploadPath_.empty())
        uploadPath_ = "uploads";
}

std::ostream &operator<<(std::ostream &stream, const Server &server)
{
    stream << "-- SERVER -- \nPorts: {";
    for (std::set<int>::const_iterator portsIt = server.ports_.begin(); portsIt != server.ports_.end(); ++portsIt)
        stream << *portsIt << ", ";
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

Session *Server::getSession(int id) const
{
    return this->sessions_.at(id);
}

std::map<int, Session *> Server::getSessions() const
{
    return this->sessions_;
}

void Server::pushSession(Session *session)
{
    int index = 0;
    if (!this->sessions_.empty())
        index = (--this->sessions_.end())->first + 1;
    this->sessions_[index] = session;
    session->setId(index);
}