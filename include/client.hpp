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

class Client {

	public:
		Client(int clientFd);
		~Client();

		void	clientData();

	private:
		int	clientFd;
};
