#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <strings.h>
#include <exception>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "WebServer.hpp"
#include "Client/Client.hpp"
#include "Client/Request/RequestFactory/RequestFactory.hpp"
#include "utils/exceptions/Exceptions.hpp"

WebServer::WebServer()
{
}

WebServer::~WebServer()
{
	for (std::vector<Client *>::iterator it = clients_.begin(); it != clients_.end(); ++it)
		delete *it;
	this->clients_.clear();

	for (std::vector<Server *>::iterator it = servers_.begin(); it != servers_.end(); ++it)
		delete *it;
	this->servers_.clear();
}

static fd_t setServerFd(sockaddr_in &serverAddress, std::set<int> serverPorts)
{
	int enable = 1;

	for (std::set<int>::iterator portsIt = serverPorts.begin(); portsIt != serverPorts.end(); ++portsIt)
	{
		serverAddress.sin_port = htons(*portsIt);
		int socketFd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
		// TODO: ERASE SETSOCKOPT
		if (setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) == -1)
			throw std::runtime_error("Couldn't set SO_REUSEADDR");
		if (socketFd < 0)
			throw std::runtime_error("Couldn't create socket");
		if (bind(socketFd, (struct sockaddr *)(&serverAddress), sizeof(serverAddress)) == -1)
			throw std::runtime_error("Couldn't bind socket");
		if (listen(socketFd, SOMAXCONN) == -1)
			throw std::runtime_error("Couldn't listen to port");
		return socketFd;
	}
}

std::vector<fd_t> WebServer::createServerFds()
{
	sockaddr_in serverAddress;
	fd_t serverFd;
	std::vector<fd_t> serversFds;
	// TODO: ERASE THIS VAR

	bzero(&serverAddress, sizeof(serverAddress));
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_family = AF_INET;
	for (std::vector<Server *>::iterator serversIt = servers_.begin(); serversIt != servers_.end();)
	{
		try
		{
			std::set<int> serverPorts = (*serversIt)->getPorts();
			serverFd = setServerFd(serverAddress, serverPorts);
			serversFds.push_back(serverFd);
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << " || Server: " << *((*serversIt)->getNames().begin()) << std::endl;
			delete *serversIt;
			serversIt = servers_.erase(serversIt);
			continue ;
		}
		serversIt++;
	}
	return serversFds;
}

void WebServer::setEpollInstance(t_epoll &epoll, std::vector<fd_t> &serversFds)
{
	epoll.fd = epoll_create1(0);
	if (epoll.fd == -1)
		throw std::runtime_error("Couldn't create epoll");

	epoll.eventConfig.events = EPOLLIN;
	for (std::vector<fd_t>::iterator it = serversFds.begin(); it != serversFds.end(); ++it)
	{
		epoll.eventConfig.data.fd = *it;
		if (epoll_ctl(epoll.fd, EPOLL_CTL_ADD, *it, &epoll.eventConfig) == -1)
			throw std::runtime_error("Couldn't add server fd to epoll");
	}
}

void WebServer::setEpollRead(t_epoll &epoll, Client *client)
{
	epoll.eventConfig.events = EPOLLIN;
	epoll.eventConfig.data.ptr = static_cast<Client *>(client);
	if (epoll_ctl(epoll.fd, EPOLL_CTL_MOD, client->getFd(), &epoll.eventConfig) == -1)
		throw std::runtime_error("Couldn't add POLLIN flag to client fd");
}

void WebServer::setEpollWrite(t_epoll &epoll, Client *client)
{
	epoll.eventConfig.events = EPOLLOUT;
	epoll.eventConfig.data.ptr = static_cast<void *>(client);
	if (epoll_ctl(epoll.fd, EPOLL_CTL_MOD, client->getFd(), &epoll.eventConfig) == -1)
		throw std::runtime_error("Couldn't add POLLOUT flag to client fd");
}

fd_t WebServer::getServerFd(std::vector<fd_t> &serversFds, fd_t eventFd)
{
	for (std::vector<fd_t>::iterator it = serversFds.begin(); it != serversFds.end(); ++it)
	{
		if (eventFd == *it)
			return *it;
	}
	return -1;
}

void WebServer::acceptNewClient(fd_t &serverFd, t_epoll &epoll)
{
	sockaddr_in newClientAddress;
	socklen_t socketSize = sizeof(newClientAddress);

	while (true)
	{
		bzero(&newClientAddress, sizeof(newClientAddress));
		fd_t newClientFd = accept(serverFd, (struct sockaddr *)&newClientAddress, &socketSize);
		if (newClientFd < 0)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				break;
			throw std::runtime_error("Couldn't accept new client");
		}
		if (fcntl(newClientFd, F_SETFL, O_NONBLOCK) < 0)
			throw std::runtime_error("Couldn't set NONBLOCKING flag to client fd");
		Client *newClient = new Client();
		this->clients_.push_back(newClient);
		newClient->setFd(newClientFd);
		epoll.eventConfig.data.ptr = newClient;
		epoll.eventConfig.events = EPOLLIN;
		if (epoll_ctl(epoll.fd, EPOLL_CTL_ADD, newClientFd, &epoll.eventConfig) == -1)
			throw std::runtime_error("Couldn't add client fd to epoll");
		std::cout << "New Client (ID: " << newClient->getId() << ") connected" << std::endl;
	}
}

std::vector<Client *>::iterator WebServer::disconnectClient(Client *client, t_epoll &epoll, const std::string &reason)
{
	if (epoll_ctl(epoll.fd, EPOLL_CTL_DEL, client->getFd(), NULL) == -1)
		throw std::runtime_error("Couldn't delete client fd from epoll");
	if (close(client->getFd()) == -1)
		throw std::runtime_error("Couldn't close client fd");
	std::cout << "Client (ID: " << client->getId() << ") " << reason << std::endl;

	return removeClient(client);
}

Server *WebServer::matchServer(std::string hostName)
{
	for (std::vector<Server *>::iterator serverIt = servers_.begin(); serverIt != servers_.end(); ++serverIt)
	{
		if ((*serverIt)->matchesName(hostName))
			return *serverIt;
	}

	return *servers_.begin();
}

void WebServer::processRequest(Client *client, Result<t_Request> &result)
{
	Server *server;
	Location *location;
	HandlingResult handlingResult;

	if (result.isSuccess())
	{
		client->setRequest(result.getValue());
		server = this->matchServer(client->getRequest().headers.getHeaderValue(HOST));
		location = server->matchLocation(client->getRequest().requestLine.getTargetPath());
		handlingResult = RequestHandler::handleRequest(client->getRequest(), *location, *server);

		client->buildResponse(handlingResult);
	}
	else
	{
		t_httpCode code = result.getError();
		t_connection mode = (code == 404) ? C_CLOSE : C_KEEP_ALIVE;

		client->buildResponse(code, mode);
	}

	client->clearRequestBuffer();
}
// TODO: Client not disconnecting after closing tab?????
void WebServer::receiveRequest(Client *client, t_epoll &epoll)
{
	int bytesReceived;
	char buffer[READ_BUFFER_SIZE + 1];

	bzero(buffer, sizeof(buffer));
	bytesReceived = recv(client->getFd(), buffer, READ_BUFFER_SIZE, 0);
	if (0 < bytesReceived)
	{
		client->updateLastReceivedPacket();
		if (!client->canBuildRequest(buffer))
			return;
		else
		{
			Result<t_Request> result = RequestFactory::create(client->getRequestBuffer());
			this->processRequest(client, result);
		}
		this->setEpollWrite(epoll,client);
	}
	else if (bytesReceived == 0)
		disconnectClient(client, epoll, DISCONNECTED);
	else
		throw std::runtime_error("Couldn't receive data from client fd");
}

void WebServer::sendResponse(Client *client, t_epoll &epoll)
{
	ssize_t bytesSent = send(client->getFd(), client->getResponseBuffer().c_str(), client->getResponseBuffer().length(), 0);

	if (bytesSent < 0)
		throw std::runtime_error("Couldn't send response");

	client->eraseResponse(bytesSent);
	if ((ssize_t)client->getResponseBuffer().length())
		return;

	if (client->shouldClose())
	{
		std::string reason = DISCONNECTED;
		if (client->getResponseStatus() == REQUEST_TIME_OUT)
			reason = TIMED_OUT;
		disconnectClient(client, epoll, reason);
	}
	else
		setEpollRead(epoll, client);
}

void WebServer::checkClientEvent(t_epoll &epoll, const int &eventIndex)
{
	Client *client = static_cast<Client *>(epoll.eventBuffer[eventIndex].data.ptr);

	if (epoll.eventBuffer[eventIndex].events & EPOLLIN)
		receiveRequest(client, epoll);
	else if (epoll.eventBuffer[eventIndex].events & EPOLLOUT)
		sendResponse(client, epoll);
	else
		disconnectClient(client, epoll, DISCONNECTED);
}

void WebServer::handleConnectionEvents(std::vector<fd_t> &serversFds, t_epoll &epoll)
{
	int readyFds;

	std::cout << "Waiting for connections..." << std::endl;
	while (true)
	{
		readyFds = epoll_wait(epoll.fd, epoll.eventBuffer, EVENT_BUFFER_SIZE, TIMEOUT);
		if (readyFds == -1)
			throw std::runtime_error("Couldn't wait for epoll fds");

		for (int i = 0; i < readyFds; i++)
		{
			fd_t serverFd = getServerFd(serversFds, epoll.eventBuffer[i].data.fd);

			if (serverFd != -1)
				acceptNewClient(serverFd, epoll);
			else
				checkClientEvent(epoll, i);
		}
		disconnectTimedoutClients(epoll);
	}
}

std::vector<Client *>::iterator WebServer::removeClient(Client *client)
{
	int clientId = client->getId();

	for (std::vector<Client *>::iterator it = this->clients_.begin(); it != this->clients_.end(); ++it)
	{
		if ((*it)->getId() == clientId)
		{
			delete *it;
			return this->clients_.erase(it);
		}
	}

	return this->clients_.end();
}

void WebServer::disconnectTimedoutClients(t_epoll &epoll)
{
	time_t now = std::time(NULL);

	for (std::vector<Client *>::iterator it = clients_.begin(); it != clients_.end();)
	{
		if ((now - (*it)->getLastReceivedPacket()) * 1000 > TIMEOUT)
		{
			if ((*it)->hasPendingRequest())
			{
				(*it)->buildResponse(REQUEST_TIME_OUT, C_CLOSE);
				this->setEpollWrite(epoll, *it);
			}
			else
			{
				it = disconnectClient(*it, epoll, TIMED_OUT);
				continue;
			}
		}
		++it;
	}
}

void WebServer::setServers(std::vector<Server *> &servers)
{
	this->servers_ = servers;
}

void WebServer::serve()
{
	if (this->servers_.empty())
		return;

	t_epoll epoll;
	std::vector<fd_t> serversFds;

	serversFds = createServerFds();
	setEpollInstance(epoll, serversFds);
	handleConnectionEvents(serversFds, epoll);
}

std::ostream &operator<<(std::ostream &stream, const WebServer &ws)
{
	stream << "--- WEBSERVER ---" << std::endl;
	for (std::vector<Server *>::const_iterator it = ws.servers_.begin(); it != ws.servers_.end(); ++it)
		stream << **it << "\r\n";
	stream << "\r\n";
	return stream;
}