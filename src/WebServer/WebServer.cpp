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
}

std::vector<fd_t> WebServer::createServerFds()
{
	sockaddr_in serverAddress;
	std::vector<fd_t> serversFds;

	bzero(&serverAddress, sizeof(serverAddress));
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_family = AF_INET;
	for (std::vector<Server>::iterator serversIt = this->servers_.begin(); serversIt < this->servers_.end(); ++serversIt)
	{
		std::vector<int> serverPorts = serversIt->getPorts();
		for (std::vector<int>::iterator portsIt = serverPorts.begin(); portsIt < serverPorts.end(); ++portsIt)
		{
			serverAddress.sin_port = htons(*portsIt);
			int socketFd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
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

void WebServer::setEpoll(t_epoll &epoll, std::vector<fd_t> &serversFds)
{
	epoll.fd = epoll_create1(0);
	if (epoll.fd == -1)
		throw std::runtime_error("Couldn't create epoll");

	epoll.eventConfig.events = EPOLLIN;
	for (std::vector<fd_t>::iterator it = serversFds.begin(); it < serversFds.end(); ++it)
	{
		epoll.eventConfig.data.fd = *it;
		if (epoll_ctl(epoll.fd, EPOLL_CTL_ADD, *it, &epoll.eventConfig) == -1)
			throw std::runtime_error("Couldn't add server fd to epoll");
	}
}

fd_t WebServer::getServerFd(std::vector<fd_t> &serversFds, fd_t eventFd)
{
	for (std::vector<fd_t>::iterator it = serversFds.begin(); it < serversFds.end(); ++it)
	{
		if (eventFd == *it)
			return *it;
	}
	return -1;
}

void WebServer::acceptNewClient(fd_t &serverFd, t_epoll &epoll)
{
	Client *newClient = new Client();
	sockaddr_in newClientAddress;
	socklen_t socketSize = sizeof(newClientAddress);

	bzero(&newClientAddress, sizeof(newClientAddress));
	fd_t newClientFd = accept(serverFd, (struct sockaddr *)&newClientAddress, &socketSize);
	if (newClientFd < 0)
		throw std::runtime_error("Couldn't accept new client");
	if (fcntl(newClientFd, F_SETFL, O_NONBLOCK) < 0)
		throw std::runtime_error("Couldn't set NONBLOCKING flag to client fd");
	newClient->setFd(newClientFd);
	epoll.eventConfig.data.ptr = newClient;
	epoll.eventConfig.events = EPOLLIN;
	if (epoll_ctl(epoll.fd, EPOLL_CTL_ADD, newClientFd, &epoll.eventConfig) == -1)
		throw std::runtime_error("Couldn't add client fd to epoll");
	std::cout << "New Client (ID: " << newClient->getId() << ") connected" << std::endl;
}

void WebServer::disconnectClient(Client *client, t_epoll &epoll)
{
	if (epoll_ctl(epoll.fd, EPOLL_CTL_DEL, client->getFd(), NULL) == -1)
		throw std::runtime_error("Couldn't delete client fd from epoll");
	if (close(client->getFd()) == -1)
		throw std::runtime_error("Couldn't close client fd");
	std::cout << "Client (ID: " << client->getId() << ") disconnected" << std::endl;
	delete client;
}

void WebServer::handleRequestResponse(Client *client, t_epoll &epoll)
{
	epoll.eventConfig.events = EPOLLOUT;
	epoll.eventConfig.data.ptr = static_cast<void *>(client);
	if (epoll_ctl(epoll.fd, EPOLL_CTL_MOD, client->getFd(), &epoll.eventConfig) == -1)
		throw std::runtime_error("Couldn't add POLLOUT flag to client fd");
}

void WebServer::handleRequestEvent(Client *client, char *buffer, t_epoll &epoll)
{
	bzero(buffer, sizeof(buffer));
	int bytesRead = recv(client->getFd(), buffer, READ_BUFFER_SIZE, 0);

	if (bytesRead > 0)
		handleRequestResponse(client, epoll);
	else if (bytesRead == 0)
		disconnectClient(client, epoll);
	else
		throw std::runtime_error("Couldn't receive data from client fd");
}

void WebServer::handleResponseEvent(Client *client, t_epoll &epoll)
{
	std::string stringifiedResponse = client->getStringifiedResponse();
	size_t bytesSent = send(client->getFd(), stringifiedResponse.c_str(), stringifiedResponse.length(), 0);

	if (bytesSent >= stringifiedResponse.size())
	{
		epoll.eventConfig.events = EPOLLIN;
		epoll.eventConfig.data.ptr = static_cast<Client *>(client);
		if (epoll_ctl(epoll.fd, EPOLL_CTL_MOD, client->getFd(), &epoll.eventConfig) == -1)
			throw std::runtime_error("Couldn't add POLLIN flag to client fd");
	}
}

void WebServer::handleClientEvent(char *buffer, t_epoll &epoll, const int &eventIndex)
{
	Client *client = static_cast<Client *>(epoll.eventBuffer[eventIndex].data.ptr);

	if (epoll.eventBuffer[eventIndex].events & EPOLLIN)
		handleRequestEvent(client, buffer, epoll);
	else if (epoll.eventBuffer[eventIndex].events & EPOLLOUT)
		handleResponseEvent(client, epoll);
}

void WebServer::handleConnections(std::vector<fd_t> &serversFds, t_epoll &epoll)
{
	int readyFds;
	char buffer[READ_BUFFER_SIZE + 1];

	while (true)
	{
		readyFds = epoll_wait(epoll.fd, epoll.eventBuffer, EVENT_BUFFER_SIZE, -1);
		if (readyFds == -1)
			throw std::runtime_error("Couldn't wait for epoll fds");

		for (int i = 0; i < readyFds; i++)
		{
			fd_t serverFd = getServerFd(serversFds, epoll.eventBuffer[i].data.fd);

			if (serverFd != -1)
				acceptNewClient(serverFd, epoll);
			else
				handleClientEvent(buffer, epoll, i);
		}
	}
}

void WebServer::serve()
{
	if (this->servers_.empty())
		return;

	try
	{
		t_epoll epoll;
		std::vector<fd_t> serversFds;

		serversFds = createServerFds();
		setEpoll(epoll, serversFds);
		handleConnections(serversFds, epoll);
	}
	catch (std::exception &e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
	}
}
