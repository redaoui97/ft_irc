#ifndef SERVER_HPP
# define SERVER_HPP
//always go for ifndef over "pragma once" to avoid compiler compatibility issues

#include "irc.hpp"

class Server {
	public:
		Server(int maxClients, std::string const password);
		~Server();
		bool	initializeServer(int port);
		void	startListening();
		void	clientData(int clientFd) const;
	
	private:
		int port;
		int serverFd;
		int maxClients;
		std::string password;
		struct pollfd serverSocket;
		std::vector<pollfd> clientSockets;
		void	newClientConnections(std::vector<struct pollfd>&	clientSockets);
};
#endif