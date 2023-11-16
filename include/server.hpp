#pragma once

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
#include <cerrno>

const int					MAX_CONNECTED_CLIENTS = 10;

class Server {
	public:
		Server(int maxClients,const std::string &serverPassword);
		~Server();

		bool				initializeServer(int port);
		void				startListening();
		void				clientData(int clientFd) ;
		bool				checkClientAuthorization(int clientFd);
	
	private:
		int					port;
		int					serverFd;
		std::string			serverPassword;
		struct pollfd		serverSocket;
		std::vector<pollfd>	connectedClients;
		long unsigned int	maxClients;
		static std::string	incompleteBuffer;

		void				newClientConnections(std::vector<struct pollfd>&	clientSockets);
		void				processData(const std::string& data);
		void				CompleteMessage(const std::string& completeMessage);

};

extern std::string Server_incompleteBuffer;
