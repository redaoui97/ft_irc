#include"../include/server.hpp"
#include"../include/client.hpp"
#include <fcntl.h>
#include <iterator>
#include <ostream>
#include <sys/poll.h>
#include <sys/socket.h>
#include <vector>

Server::Server(int maxClients, const std::string& serverPassword) {

	this->serverFd = -1;
	this->port = -1;
	this->maxClients = maxClients;
	this->serverPassword = serverPassword;
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

	connectedClients.reserve(maxClients);
	connectedClients.push_back(serverSocket);
	while(1) {

		filesnum = poll(connectedClients.data(), connectedClients.size(), -1);
		if(filesnum == -1) {

			std::cout << "Error polling the data from clients" << std::endl;
			break;
		}
		if (connectedClients[0].revents & POLLIN) {
			newClientConnections(connectedClients);
		} else {
			for (int i = 0; i < connectedClients.size(); ++i) {

				if (connectedClients[i].revents & POLLIN) {
					if (connectedClients[i].fd > 0) {
						clientData(connectedClients[i].fd);
					} else {
						std::cout << "invalid client socket";
						connectedClients.erase(connectedClients.begin() + i);
						--i;
					}
				}
			}
		}
	}
}

void	Server::clientData(int clientFd) const {

	char buffer[1024];
	ssize_t	bytes;
	std::string message;
	std::string completeMessage;
	size_t pos;

	if (clientFd < 0) {
		std::cout << "Error bad client file discriptor" << std::endl;
		return ;
	}
	bytes = recv(clientFd, buffer, sizeof(buffer), 0);
	if (bytes <= 0) {
		if (bytes == 0) {
			std::cout << "Client closed connection" << std::endl;
		} else if (errno == EBADF) {
            std::cout << "Error: Bad file descriptor" << std::endl;
        } else { 
			std::cout << "Error occurred during Client connection" << std::endl;
		}
        for (auto it = connectedClients.begin(); it != connectedClients.end();) {
			if (it->fd == clientFd) {
				connectedClients.erase(it);
				break;
			}
			++it;
		}
		close(clientFd);
		return ;
	} else {
		completeMessage += std::string(buffer, bytes);
		pos = completeMessage.find("\r\n");
		std::cout << pos << std::endl;
		while ((pos = completeMessage.find("\r\n")) != std::string::npos) {
			message = completeMessage.substr(0, pos);
			completeMessage.erase(0, pos + 2);
			std::cout << message << std::endl;
		}
	}
}

void	Server::newClientConnections(std::vector<struct pollfd>& connectedClients) {

	struct sockaddr_in	clientAddr;
	socklen_t	clientAddrlen;
	int clientFd;

	clientAddrlen = sizeof(clientAddr);
	clientFd = accept(serverFd, (sockaddr*)&clientAddr, &clientAddrlen);
	if (clientFd == -1) {

		std::cout << "Error accepting client connection" << std::endl;
	} else if (connectedClients.size() < maxClients) {

		if (checkClientAuthorization(clientFd)) {
			struct pollfd connectedClient;
			connectedClient.fd = clientFd;
			connectedClient.events = POLLIN;
			connectedClients.push_back(connectedClient);
		}
	} else {
		std::cout << "Reject Connection : Error max Clients." << std::endl;
		close(clientFd);
	}
}

bool Server::checkClientAuthorization(int clientFd) {

	char buffer[1024];
	ssize_t bytes;
	std::string clientPassword;

	send(clientFd, "Enter the server password: ", 26, 0);
	bytes = recv(clientFd, buffer, sizeof(buffer), 0);
	if (bytes <= 0) {

		std::cout << "Error reading password from client" << std::endl;
		close(clientFd);
		return false;
	}

	clientPassword = std::string(buffer, bytes);
	clientPassword = clientPassword.substr(0, clientPassword.find_last_not_of(" \t\n") + 1);
	if (clientPassword == serverPassword) {
		std::cout << "Client authenticated" << std::endl;
		return true;
	} else {
		std::cout << "incorrect password, reject connection" << std::endl;
		close(clientFd);
		return false;
	}
}

Server::~Server() {

	if(serverFd != -1) {
		close(serverFd);
	}
	std::cout << "Server destructor" << std::endl;
}
