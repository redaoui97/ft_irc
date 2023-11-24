#ifndef SERVER_HPP
# define SERVER_HPP
//always go for ifndef over "pragma once" to avoid compiler compatibility issues

#include "irc.hpp"

#ifndef MAXCLIENTS
# define MAXCLIENTS 100
#endif

class Server
{
	public:
		Server(std::string const password);
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
		void	newClientConnections(std::vector<struct pollfd>&	clientSockets);

	class SocketInitException : public std::exception
	{
		private:
			std::string errorMessage;
		public:
			SocketInitException(const std::string& message) : errorMessage("Socket Initialization exception triggered: " + message) {}
			const char* what() const throw()
			{
				return errorMessage.c_str();
			}
			virtual ~SocketInitException() throw() {}
	};
};
#endif