#include "../include/irc.hpp"
#include "../include/server.hpp"
#include "../include/client.hpp"

Client::Client(int clientFd) {

	std::cout << "Client constructor" << std::endl;
	this->clientFd = clientFd;
}

int Client::getClientFd() const {
	return clientFd;
}

Client::~Client() {

	std::cout << "Client destructor" << std::endl;
	close(clientFd);
}
