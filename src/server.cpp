#include"../include/server.hpp"
#include <iterator>
#include <ostream>
#include <sys/socket.h>

Server::Server() {

	ServerFd = -1;
	port = -1;
	std::cout << "Server constructor" << std::endl;
}
	
bool	Server::InitializeServer(int port) {

	struct sockaddr_in ServerAddr;

	this->port = port;
	ServerFd = socket(AF_INET, SOCK_STREAM, 0);
	if (ServerFd == -1) {
		std::cout << "Error at Creation of server socket" << std::endl;
		return false;
	}

	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(port);
	ServerAddr.sin_addr.s_addr = inet_addr("0.0.0.0");

	if(bind(ServerFd, (sockaddr*)&ServerAddr, sizeof(ServerAddr)) == -1 ) {
		std::cout << "Error Binding" << std::endl;
		close(ServerFd);
		return false;
	}

	if(listen(ServerFd, 10) == -1) {
		std::cout << "Error listening" << std::endl;
		close(ServerFd);
		return 1;
	}

	std::cout << "the Server is initialized and listening on port :" << port << std::endl;
	return true;
}

void	Server::StartListening() {

	struct sockaddr_in ClientAddr;
	socklen_t ClientAddrlen;
	int	ClientFd; 

	ClientAddrlen = sizeof(ClientAddr);
	while(1) {

		ClientFd = accept(ServerFd, (sockaddr*)&ClientAddr, &ClientAddrlen);
		if (ClientFd == -1) {
			std::cout << "Error accepting client connection" << std::endl;
			continue;
		}
        ClientConnections(ClientFd);
	}

}

void	Server::ClientConnections(int ClientFd) {

	char buffer[1024];
	ssize_t	bytes;

	bytes = recv(ClientFd, buffer, sizeof(buffer), 0);
	if (bytes <= 0) {
		if (bytes == 0)
			std::cout << "Client closed connection" << std::endl;
		else 
			std::cout << "Error occurred during Client connection" << std::endl;
		close(ClientFd);
	}
	else {
		// here i will handle the messeges and extraxt commands

	}

}

Server::~Server() {

	if(ServerFd != -1) {
		close(ServerFd);
	}
	std::cout << "Server destructor" << std::endl;
}
