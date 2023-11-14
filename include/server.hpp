#pragma 

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
		Server(int maxClients,const std::string &serverPassword);
		~Server();

		bool	initializeServer(int port);
		void	startListening();
		void	clientData(int clientFd) const;
		bool	checkClientAuthorization(int clientFd);
	
	private:
		int port;
		int serverFd;
		std::string serverPassword;
		struct pollfd serverSocket;
		std::vector<pollfd> connectedClients;
		int maxClients;
		void	newClientConnections(std::vector<struct pollfd>&	clientSockets);

};
