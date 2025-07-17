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

WebServer::WebServer()
{
}

WebServer::~WebServer()
{
	for (std::vector<Client*>::iterator it = clients_.begin(); it != clients_.end(); ++it) 
		delete *it;
	this->clients_.clear();

	for (std::vector<Server*>::iterator it = servers_.begin(); it != servers_.end(); ++it) 
		delete *it;
	this->servers_.clear();
}

std::vector<fd_t> WebServer::createServerFds()
{
	sockaddr_in serverAddress;
	std::vector<fd_t> serversFds;
	// TODO: ERASE THIS VAR
	int enable = 1;

	bzero(&serverAddress, sizeof(serverAddress));
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_family = AF_INET;
	for (std::vector<Server *>::iterator serversIt = servers_.begin(); serversIt != servers_.end(); ++serversIt)
	{
		std::vector<int> serverPorts = (*serversIt)->getPorts();
		for (std::vector<int>::iterator portsIt = serverPorts.begin(); portsIt != serverPorts.end(); ++portsIt)
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
			serversFds.push_back(socketFd);
		}
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

bool WebServer::tryBuildRequest(Client *client, char *buffer)
{
	client->appendRequest(buffer);
	if (!client->hasFullRequestHeaders())
		return false;
	client->buildRequest();

	return true;
}

void WebServer::readySendResponse(Client *client, t_epoll &epoll)
{
	client->clearRequest();
	client->updateResponse();
	setEpollWrite(epoll, client);
}

void WebServer::receiveRequest(Client *client, t_epoll &epoll)
{
	int bytesReceived;
	char buffer[READ_BUFFER_SIZE + 1];

	bzero(buffer, sizeof(buffer));
	bytesReceived = recv(client->getFd(), buffer, READ_BUFFER_SIZE, 0);
	if (bytesReceived > 0)
	{
		client->updateLastReceivedPacket();
		if (!this->tryBuildRequest(client, buffer))
			return ;
		client->buildResponse(this->servers_);
		this->readySendResponse(client, epoll);
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

	// struct sockaddr_in addr;
	// socklen_t len = sizeof(addr);
	// getsockname(client->getFd(), (struct sockaddr*)&addr, &len);
	// int local_port = ntohs(addr.sin_port);

	if (epoll.eventBuffer[eventIndex].events & EPOLLIN)
		receiveRequest(client, epoll);
	else if (epoll.eventBuffer[eventIndex].events & EPOLLOUT)
		sendResponse(client, epoll);
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
				this->readySendResponse(*it, epoll);
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
