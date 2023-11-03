#include"../include/server.hpp"
#include"../include/client.hpp"
#include <fcntl.h>
#include <iterator>
#include <ostream>
#include <sys/poll.h>
#include <sys/socket.h>
#include <vector>

Server::Server(int maxClients) {

	this->serverFd = -1;
	this->port = -1;
	this->maxClients = maxClients;
	std::cout << "Server constructor" << std::endl;
}
	
bool	Server::initializeServer(int port) {

	struct sockaddr_in ServerAddr;

	this->port = port;
	serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if (serverFd == -1) {
		std::cout << "Error at Creation of server socket" << std::endl;
		return false;
	}

	if (fcntl(serverFd, F_SETFL, O_NONBLOCK) == -1) {
		std::cout << "Error Mode NonBlocking Server" << std::endl;
		return false;
	}

	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(port);
	ServerAddr.sin_addr.s_addr = inet_addr("0.0.0.0");

	if(bind(serverFd, (sockaddr*)&ServerAddr, sizeof(ServerAddr)) == -1 ) {
		std::cout << "Error Binding" << std::endl;
		close(serverFd);
		return false;
	}

	if(listen(serverFd, 10) == -1) {
		std::cout << "Error listening" << std::endl;
		close(serverFd);
		return 1;
	}

	serverSocket.fd = serverFd;
	serverSocket.events = POLLIN;

	std::cout << "the Server is initialized and listening on port :" << this->port << std::endl;
	return true;
}

void	Server::startListening() {

	int filesnum;

	clientSockets.reserve(maxClients);
	clientSockets.push_back(serverSocket);
	while(1) {

		filesnum = poll(clientSockets.data(), clientSockets.size(), -1);
		if(filesnum == -1) {

			std::cout << "Error polling the data from clients" << std::endl;
			break;
		}
		for (int i = 0; i < clientSockets.size(); ++i) {

			if (clientSockets[i].revents & POLLIN) {
				if (i == serverSocket.fd) {
					newClientConnections(clientSockets);
				} else {
					clientData(i);
                }
			}
	}
}

void	Server::clientData(int clientFd) {

	char buffer[1024];
	ssize_t	bytes;

	bytes = recv(clientFd, buffer, sizeof(buffer), 0);
	if (bytes <= 0) {
		if (bytes == 0)
			std::cout << "Client closed connection" << std::endl;
		else 
			std::cout << "Error occurred during Client connection" << std::endl;
		close(clientFd);
		return ;
	}
	else {
		// here i will handle the messeges and extraxt commands

	}

}

void	Server::newClientConnections(std::vector<struct pollfd>& clientSockets) {
	
	struct sockaddr_in	clientAddr;
	socklen_t	clientAddrlen;
	int clientFd;

	clientAddrlen = sizeof(clientAddr);
	clientFd = accept(serverFd, (sockaddr*)&clientAddr, &clientAddrlen);
	if (clientFd == -1) {

		std::cout << "Error accepting client connection" << std::endl;
	} else if (clientSockets.size() < maxClients) {

		struct pollfd clientSocket;
		clientSocket.fd = clientFd;
		clientSocket.events = POLLIN;
		clientSockets.push_back(clientSocket);
	} else {
		std::cout << "Reject Connection : Error max Clients." << std::endl;
		close(clientFd);
	}
}

Server::~Server() {

	if(serverFd != -1) {
		close(serverFd);
	}
	std::cout << "Server destructor" << std::endl;
}
