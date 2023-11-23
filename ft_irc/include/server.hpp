#ifndef SERVER_HPP
# define SERVER_HPP

#include<iostream>
#include<unistd.h>
#include<string>
#include<stdio.h>
#include<cstring>
#include<stdlib.h>
#include<sys/types.h>
#include <netinet/in.h>
#include<sys/socket.h>
#include <arpa/inet.h>
#include<fcntl.h>
#include<vector>
#include<poll.h>
#include"client.hpp"
#include <map>


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
		std::string password;
		struct pollfd serverSocket;
		std::vector<pollfd> clientSockets;
		int maxClients;
		void	newClientConnections(std::vector<struct pollfd>&	clientSockets);

};
#endif