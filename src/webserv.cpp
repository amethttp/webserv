#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <cerrno>

#include "WebServer/Client/Client.hpp"

#define EVENT_SIZE 64
#define BUFFER_SIZE 4096
#define PORT 8080

int main(void)
{
    sockaddr_in serverAddress, client;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    int c = sizeof(struct sockaddr_in);

    int serverSocket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Bind failed: Rebinding:" << errno << std::endl;
        serverAddress.sin_port = htons(PORT + 1);
        if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
            std::cerr << "Rebinding failed: " << errno << std::endl;
            return 1;
        }
    }
    listen(serverSocket, SOMAXCONN);

    int dots = 1;
    int bytesRead;
    char buffer[BUFFER_SIZE + 1];
    epoll_event evSetter;
    epoll_event events[EVENT_SIZE];
    int nfds;
    int epfd = epoll_create1(0);

    evSetter.events = EPOLLIN;
    evSetter.data.fd = serverSocket;
    epoll_ctl(epfd, EPOLL_CTL_ADD, serverSocket, &evSetter);

    while (true) {
        std::cout << "Epoll waiting";
        for (int i = 0; i < dots; ++i) {
            std::cout << ".";
        }
        std::cout << std::endl;
        dots = (dots + 1) % 4;

        nfds = epoll_wait(epfd, events, EVENT_SIZE, -1); // -1 For indefinite waiting(blocking)
        std::cerr << "NFDS: " << nfds << std::endl;
        for (int i = 0; i < nfds; ++i) {
            if (events[i].data.fd == serverSocket) {
                memset(&client, 0, sizeof(client));
                Client *newClient = new Client;
                evSetter.events = EPOLLIN;
                newClient->fd_ = accept4(serverSocket, (struct sockaddr *)&client, (socklen_t*)&c, O_NONBLOCK);
                if (newClient->fd_ < 0) {
                    std::cerr << "Accept failed: " << errno << std::endl;
                    return 1;
                }
                evSetter.data.ptr = static_cast<void *>(newClient);
                epoll_ctl(epfd, EPOLL_CTL_ADD, newClient->fd_, &evSetter);
                std::cout << "Connection accepted" << std::endl;
            } else {
                std::cout << "Existing Client" << std::endl;
                Client *client = static_cast<Client *>(events[i].data.ptr);
                if (events[i].events & EPOLLIN) {
                    std::cout << "EPOLLIN" << std::endl;
                    if ((bytesRead = recv(client->fd_ , buffer, BUFFER_SIZE, 0)) > 0) {
                        client->request_.requestBuf += buffer;
                        bzero(buffer, sizeof(buffer));
                        if (client->request_.requestBuf.size() >= 6) {
                            if (client->request_.requestBuf.substr(client->request_.requestBuf.size() - 6, 4) == "done") {
                                client->response_.responseBuf = client->request_.requestBuf + "\n ------------------------- \n";
                                evSetter.events = EPOLLOUT;
                                evSetter.data.ptr = static_cast<void *>(client);
                                epoll_ctl(epfd, EPOLL_CTL_MOD, client->fd_, &evSetter);
                                continue ;
                            }
                        }
                    } else if (bytesRead == 0) {
                        epoll_ctl(epfd, EPOLL_CTL_DEL, client->fd_, NULL);
                        close(client->fd_);
                        delete client;
                        std::cout << "Client disconnected" << std::endl;
                        continue ;
                    } else if (bytesRead < 0) {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) {
                            std::cout << "EOM:" << errno << std::endl;
                            continue ;
                        } else {
                            std::cerr << bytesRead << "| Recv failed: " << errno << std::endl;
                            return 1;
                        }
                    }
                } else if (events[i].events & EPOLLOUT) {
                    std::cout << "Writing... | " << client->response_.responseBuf << " |" << std::endl;
                    send(client->fd_ , client->response_.responseBuf.c_str() , client->response_.responseBuf.size(), 0);
                    std::cout << "Response Successfull!" << std::endl;
                    client->request_.requestBuf.clear();
                    client->response_.responseBuf.clear();
                    evSetter.events = EPOLLIN;
                    evSetter.data.ptr = static_cast<void *>(client);
                    epoll_ctl(epfd, EPOLL_CTL_MOD, client->fd_, &evSetter);
                }
            }
        }
    }

    return 0;
}
