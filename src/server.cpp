#include"../include/server.hpp"
#include"../include/client.hpp"
#include <iterator>
#include <ostream>
#include <sys/poll.h>
#include <sys/socket.h>
#include <vector>

Server::Server() {

	serverFd = -1;
	port = -1;
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

//	struct sockaddr_in ClientAddr;
	std::vector<struct pollfd> Fds;
//	socklen_t ClientAddrlen;
//	pollfd	ServerSocket;
//	int	ClientFd; 
	int maxClient;
	int filesnum;


//	ClientAddrlen = sizeof(ClientAddr);
//	ServerSocket.fd = serverFd;
//	ServerSocket.events = POLLIN;
//	ServerSocket.revents = 0;
//	clientSockets.push_back(ServerSocket);
	maxClient = 100;
	Fds.push_back(serverSocket);
	while(1) {

		filesnum = poll(Fds.data(), Fds.size(), -1);
		if(filesnum == -1) {

			std::cout << "Error polling the data from clients" << std::endl;
			break;
		}
		
		for (int i = 0; i < Fds.size(); ++i) {
			if (Fds[i].revents & POLLIN) {
				if (Fds[i].fd == serverFd) {

				}
			}
		}


		Client	client(ClientFd);
		client.clientConnections();
	}
	ClientFd = accept(serverFd, (sockaddr*)&ClientAddr, &ClientAddrlen);
		if (ClientFd == -1) {
			std::cout << "Error accepting client connection" << std::endl;
			continue;
		}

}

Server::~Server() {

	if(serverFd != -1) {
		close(serverFd);
	}
	std::cout << "Server destructor" << std::endl;
}
