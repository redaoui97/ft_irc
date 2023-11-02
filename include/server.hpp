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


class Server {
	public:
		Server();
		~Server();

		bool	initializeServer(int port);
		void	startListening();
//		std::vector<pollfd> clientSockets;
		void	newClientConnections(std::vector<struct pollfd>&	Fds);
	
	private:
		int port;
		int serverFd;
		struct pollfd serverSocket;

};
