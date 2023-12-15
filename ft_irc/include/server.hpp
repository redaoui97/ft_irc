#ifndef SERVER_HPP
# define SERVER_HPP

#include "../include/irc.hpp"

#ifndef MAXCLIENTS
# define MAXCLIENTS 100
#endif

//additional functions 
Client* create_client(int clientFd, char *ip);
class Server
{
	public:
		Server(std::string const password);
		~Server();
		bool	initializeServer(int port);
		void	startListening();
		void	clientData(int clientFd);
		bool	client_exists(std::string nick);
		std::string get_version();
		std::string get_time();
	
	private:
		int port;
		int serverFd;
		std::string password;
		struct pollfd serverSocket;
		std::string	  server_version;
		std::string	  make_time;
		std::vector<pollfd> clientSockets;
		std::vector<Client*> clients;
		Client *find_user(int clientFd);
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
