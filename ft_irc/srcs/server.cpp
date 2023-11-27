#include "../include/irc.hpp"
#include "../include/server.hpp"
#include "../include/client.hpp"

Server::Server(std::string const password)
{
	this->serverFd = -1;
	this->port = -1;
	this->password = password;
}
	
bool	Server::initializeServer(int port)
{
	struct sockaddr_in ServerAddr;

	this->port = port;
	//We could use 0 instead of IPPROTO_TCP for automatic kernel protocol recognition 
	serverFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverFd == -1) {
		throw SocketInitException("failed to create socket!");
		return false;
	}

	if (fcntl(serverFd, F_SETFL, O_NONBLOCK) == -1) {
		throw SocketInitException("failed to set-up NonBlocking mode");
		return false;
	}

	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(port);
	ServerAddr.sin_addr.s_addr = inet_addr("0.0.0.0");

	if(bind(serverFd, (sockaddr*)&ServerAddr, sizeof(ServerAddr)) == -1 ) {
		throw SocketInitException("failed to bind socket to port!");
		close(serverFd);
		return false;
	}

	if(listen(serverFd, 10) == -1) {
		throw SocketInitException("failed to set-up listener!");
		close(serverFd);
		return 1;
	}

	serverSocket.fd = serverFd;
	serverSocket.events = POLLIN;

	std::cout << "The Server is initialized and listening on port :" << this->port << std::endl;
	return true;
}

void	Server::startListening() {

	int filesnum;

	clientSockets.reserve(MAXCLIENTS);
	clientSockets.push_back(serverSocket);
	while(1) {

		filesnum = poll(clientSockets.data(), clientSockets.size(), -1);
		if(filesnum == -1) {

			std::cout << "Error polling the data from clients" << std::endl;
			break;
		}
		if (clientSockets[0].revents & POLLIN) {
			newClientConnections(clientSockets);
		} else {
			for (size_t i = 0; i < clientSockets.size(); ++i) {

				if (clientSockets[i].revents & POLLIN) {
					if (clientSockets[i].fd > 0) {
						clientData(clientSockets[i].fd);
					} else {
						std::cout << "invalid client socket";
						clientSockets.erase(clientSockets.begin() + i);
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

	if (clientFd < 0) {
		std::cout << "Error bad client file discriptor" << std::endl;
		return ;
	}
	bytes = recv(clientFd, buffer, sizeof(buffer), 0);
	if (bytes <= 0) {
		std::cout << "from here\n";
		if (bytes == 0) {
			std::cout << "Client closed connection" << std::endl;
		} else if (errno == EBADF) {
            std::cout << "Error: Bad file descriptor" << std::endl;
        } 
		else { 
			perror("Error occurred during Client connection");
		//	std::cout << "Error occurred during Client connection" << std::endl;
		}
		close(clientFd);
		return ;
	} else {
		// here i will handle the messeges and extraxt commands
		std::cout << "messeges here" << std::endl;
	}

}

void	Server::newClientConnections(std::vector<struct pollfd>& clientSockets)
{
	struct sockaddr_in	clientAddr;
	socklen_t	clientAddrlen;
	int clientFd;

	clientAddrlen = sizeof(clientAddr);
	clientFd = accept(serverFd, (sockaddr*)&clientAddr, &clientAddrlen);
	if (clientFd == -1) {
		std::cout << "Error accepting client connection" << std::endl;
	} else if (clientSockets.size() < (size_t)MAXCLIENTS) {
		struct pollfd clientSocket;
		clientSocket.fd = clientFd;
		clientSocket.events = POLLIN;
		clientSockets.push_back(clientSocket);
		std::cout << "New Client have been Added" << std::endl;
	} else {
		std::cout << "Reject Connection : Error max Clients." << std::endl;
		close(clientFd);
	}
}

Server::~Server()
{
	if(serverFd != -1)
		close(serverFd);
}
