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


class Server {
	public:
		Server();
		~Server();

		bool	InitializeServer(int port);
		void	StartListening();
		void	ClientConnections(int ClientFd);
	
	private:
		int port;
		int ServerFd;

};
