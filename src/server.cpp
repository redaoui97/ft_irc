#include"../include/server.hpp"
#include"../include/client.hpp"
#include <cstddef>
#include <fcntl.h>
#include <fstream>
#include <iterator>
#include <ostream>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
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
		close(serverFd);
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

	if(listen(serverFd, MAX_CONNECTED_CLIENTS) == -1) {
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
			for (size_t i = 0; i < connectedClients.size(); ++i) {

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

void	Server::CompleteMessage(int clientFd, const std::string& completeMessage) {
	std::cout << clientFd << " : " << completeMessage << std::endl;
	std::fstream	prefix;
	
	std::fstream	command;


}


void	Server::processData(int clientFd, const std::string& data) {
	size_t pos;
	std::string completeMessage;

	clientIncompleteBuffers[clientFd] += data;
	pos = clientIncompleteBuffers[clientFd].find("\r\n", 0);
	while (pos != std::string::npos) {
		completeMessage = clientIncompleteBuffers[clientFd].substr(0, pos);
		clientIncompleteBuffers[clientFd].erase(0, pos + 2);
		CompleteMessage(clientFd, completeMessage);
		pos = clientIncompleteBuffers[clientFd].find("\r\n", 0);
	}
}

void	Server::clientData(int clientFd) {

	char buffer[1024];
	ssize_t	bytes;

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
		removeDisconnectedClient(clientFd);
	} else {
		processData(clientFd, std::string(buffer, bytes));
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

void	Server::removeDisconnectedClient(int clientFd) {
	
	std::map<int, std::string>::iterator it;

	it = clientIncompleteBuffers.find(clientFd);
	if (it != clientIncompleteBuffers.end()) {
		clientIncompleteBuffers.erase(it);
	}
	for (size_t i = 0; i < connectedClients.size();i++) {
		if (connectedClients[i].fd == clientFd) {
			connectedClients.erase(connectedClients.begin() + i);
			break;
		}
	}
	close(clientFd);
}

Server::~Server() {

	if(serverFd != -1) {
		close(serverFd);
	}
	std::cout << "Server destructor" << std::endl;
}
