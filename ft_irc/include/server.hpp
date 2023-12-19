#ifndef SERVER_HPP
# define SERVER_HPP

#include "../include/irc.hpp"
#ifndef MAXCLIENTS
# define MAXCLIENTS 100
#endif

//additional functions 
Client* create_client(int clientFd, char *ip);
channel	*create_channel(std::string name, Client *client, std::string password);

class Server
{
	private:
		int port;
		int serverFd;
		std::string password;
		struct pollfd serverSocket;
		std::string	  server_version;
		std::string	  make_time;
		std::vector<pollfd> clientSockets;
		std::vector<Client*> clients;
		std::map<std::string, channel *> channels;
		std::map<int, std::string> buffers;
		void	newClientConnections(std::vector<struct pollfd>&	clientSockets);

	public:
		Server(std::string const password);
		~Server();
		bool	initializeServer(int port);
		void	startListening();
		void	clientData(int clientFd);
		bool	client_exists(std::string nick);
		bool	channel_exists(std::string channel);
		std::string get_version();
		std::string get_time();
		void	new_channel(std::string name, Client *client, std::string password);
		Client *find_user(int clientFd);
		channel *find_channel(std::string name);
		Client* find_user_bynick(std::string nick);
		std::vector<std::string> process_single_command(std::string command);
		void	process_command(std::string buffer, Client *client, std::string password);
		void 	execute_commands(std::vector<std::string>args, Client* client, std::string password);
		void    clientDiscon(int clientFd);
		void	delete_client(Client *client);

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
