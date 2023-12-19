#include "../include/server.hpp"
#include "../include/client.hpp"
#include "../include/irc.hpp"

Server::Server(std::string const password)
{
	this->serverFd = -1;
	this->port = -1;
	this->password = password;
	this->server_version = "0.01v";
	this->make_time = get_date();
}
	
bool	Server::client_exists(std::string nick)
{
	std::vector<Client*>::iterator it;

	for (it = clients.begin(); it != clients.end(); ++it)
	{
        if (nick.compare((*it)->getNickname()) == 0) {
			return (true);
		}
    }
	return (false);
}

bool	Server::channel_exists(std::string chann)
{
	std::map<std::string, channel*>::iterator it = channels.find(chann);

	if (channels.empty())
		return (false);
	if (it != channels.end())
		return (true);
	return (false);
}

bool	Server::initializeServer(int port)
{
	struct sockaddr_in ServerAddr;

	this->port = port;
	serverFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverFd == -1) {
		close(serverFd);
		throw SocketInitException("failed to create socket!");
		return false;
	}
	int r = 1;
	if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &r, sizeof(r)) == -1)
	{
		close(serverFd);
		throw SocketInitException("failed to set-up NonBlocking mode");
		return false;
	}

	if (fcntl(serverFd, F_SETFL, O_NONBLOCK) == -1) {
		close(serverFd);
		throw SocketInitException("failed to set-up NonBlocking mode");
		return false;
	}

	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(port);
	ServerAddr.sin_addr.s_addr = inet_addr("0.0.0.0");

	if(bind(serverFd, (sockaddr*)&ServerAddr, sizeof(ServerAddr)) == -1 ) {
		close(serverFd);
		throw SocketInitException("failed to bind socket to port!");
		close(serverFd);
		return false;
	}

	if(listen(serverFd, 10) == -1) {
		close(serverFd);
		throw SocketInitException("failed to set-up listener!");
		return 1;
	}

	serverSocket.fd = serverFd;
	serverSocket.events = POLLIN;

	std::cout << "The Server is initialized and listening on port :" << this->port << std::endl;
	return true;
}


void	Server::startListening() {

	int filesnum;

	clientSockets.reserve(MAXCLIENTS);
	clientSockets.push_back(serverSocket);
	while(1)
	{
		filesnum = poll(clientSockets.data(), clientSockets.size(), 50000);
		if(filesnum == -1) {
			normal_error("Error polling the data from clients");
			break;
		}

		if (clientSockets[0].revents & POLLIN) {
			newClientConnections(clientSockets);
		} else {
			for (size_t i = 0; i < clientSockets.size(); ++i) {
				if (clientSockets[i].revents & (POLLHUP | POLLERR))
					clientDiscon(clientSockets[i].fd);
				if (clientSockets[i].revents & POLLIN) {
					if (clientSockets[i].fd > 0) {
						clientData(clientSockets[i].fd);
					} else {
						normal_error("invalid client socket");
						clientSockets.erase(clientSockets.begin() + i);
						--i;
					}
				}
			}
		}
	}
}

Client* create_client(int clientFd, char *ip)
{
    return (new Client(clientFd, ip));
}

void Server::newClientConnections(std::vector<struct pollfd>& clientSockets)
{
    int clientFd;
    socklen_t clientAddrlen;
    struct sockaddr_in clientAddr;
    struct in_addr ip;

    clientAddrlen = sizeof(clientAddr);
    clientFd = accept(serverFd, (sockaddr*)&clientAddr, &clientAddrlen);
    if (clientFd == -1) {
        normal_error("Error accepting client connection");
    } else if (clientSockets.size() < (size_t)MAXCLIENTS) {
        struct pollfd clientSocket;
        clientSocket.fd = clientFd;
        clientSocket.events = POLLIN;
        clientSockets.push_back(clientSocket);
        ip = clientAddr.sin_addr;

        // Retrieve and print the IP correctly
        char str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &ip, str, INET_ADDRSTRLEN);
		clients.push_back(create_client(clientFd, str));
		this->buffers.insert(std::make_pair(clientFd, ""));
        std::cout << "New Client added. IP: " << str << std::endl;

    } else {
        normal_error("Reject Connection: Error max Clients.");
        close(clientFd);
    }
}

channel* Server::find_channel(std::string name)
{
	for (std::map<std::string, channel*>::const_iterator it = channels.begin(); it != channels.end(); ++it) {
        if (it->first == name)
		{
            return it->second;
        }
    }
    return NULL; 
}

Client* Server::find_user_bynick(std::string nick)
{
	std::vector<Client*>::iterator it;
    
    for (it = clients.begin(); it != clients.end(); ++it)
	{
        if ((*it)->getNickname() == nick) {
            return *it;
        }
    }
	return (NULL);
}
Client* Server::find_user(int clientFd)
{
	std::vector<Client*>::iterator it;
    
    for (it = clients.begin(); it != clients.end(); ++it)
	{
        if ((*it)->getClientFd() == clientFd) {
            return *it;
        }
    }
    return (NULL);
}

void	Server::clientData(int clientFd)
{
	ssize_t		bytes;
	char 		buffer[512];
	std::string	full_buffer;

	if (clientFd < 0) {
		normal_error("Error bad client file discriptor");
		return ;
	}
	do {
		clear_buffer(buffer, 512);
		bytes = recv(clientFd, buffer, sizeof(buffer), 0);
		if (bytes <= 0) {
			if (bytes == 0) {
				clientDiscon(clientFd);
			}
		} else {
			full_buffer += buffer;
			find_user(clientFd)->SetServer(this);
			if (bytes != 512)
				process_command(full_buffer, find_user(clientFd), this->password);
		}
	} while (bytes == 512);
}


void	Server::new_channel(std::string name, Client *client, std::string password)
{
	channel *chann = create_channel(name, client, password);

	channels.insert(std::make_pair(name, chann));
}

channel	*create_channel(std::string name, Client *client, std::string password)
{
	return (new channel(name, client, password));
}

std::string Server::get_time()
{
	return (make_time);
}

std::string Server::get_version()
{
	return (server_version);
}

//take the commands and parse them then execute
//craete vector for the cmd and his params -
std::vector<std::string> Server::process_single_command(std::string command)
{
    std::vector<std::string> command_vector;
    std::istringstream iss(command);
    std::string token;
    std::string iteration_token;

    //need to fix parser -> many spaces break the trailing and takes every space as arguemnt -> ':' shouldn't be part of the param
    //this function will be replaced by the trailing handler function by athmane 
    while (std::getline(iss, token, ' '))
    {
        if (!command_vector.empty() && command_vector.back().front() == ':' && !token.empty())
        {
            command_vector.back() += ' ' + token;
        }
        else
        {
            command_vector.push_back(token);
        }
    }
    return (command_vector);
}

// handle ctrl d buffring and proccess cmd -
void   Server::process_command(std::string buffer, Client *client, std::string password)
{
    std::vector<std::string> prepared_command;
    std::string              command;
	
	if (buffer.size() > 0 && buffer[buffer.size() - 1] != '\n')
        this->buffers[client->getClientFd()] += buffer;
    else
    {
        this->buffers[client->getClientFd()] += buffer;
        std::string proc_cmd = this->buffers[client->getClientFd()];
        std::stringstream cmd(proc_cmd);
        this->buffers[client->getClientFd()].clear();
        while (std::getline(cmd, proc_cmd, '\n'))
        {
            if (proc_cmd.find('\r') != std::string::npos)
                proc_cmd = proc_cmd.substr(0, proc_cmd.find('\r'));
			prepared_command = process_single_command(proc_cmd);
			std::cout << "command: " << proc_cmd << std::endl;
            execute_commands(prepared_command, client, password);
        }
		cmd.clear();
    }
}


//execute the function
void  Server::execute_commands(std::vector<std::string>args, Client* client, std::string password)
{
    if (!(args.front()).compare("QUIT"))
	{
        quit_cmd(client);

		return;
	}
	if (args.empty()) {
		return ;
	}
    else if (!(client->IsAuthenticated()))
    {
        if (!(args.front()).compare("PASS") || !(args.front()).compare("NICK") || !(args.front()).compare("USER"))
            authentication(args, client, password);
        else
            send_err(client, ERR_NOTREGISTERED, ":You have not registered");
    }
    else
    {
		if (!(args.front()).compare("USER"))
		{
			user_cmd(client, args);
		}
		else if (!(args.front()).compare("NICK"))
        {
            nick_cmd(client, args);
        }
        else if (!(args.front()).compare("PRIVMSG"))
        {
            privmsg_cmd(client, args);
        }
        else if (!(args.front()).compare("JOIN"))
        {
            join_cmd(client, args);
        }
		else if (!(args.front()).compare("NOTICE"))
        {
            privmsg_cmd(client, args);
        }
        else if (!(args.front()).compare("KICK") || !(args.front()).compare("INVITE") || !(args.front()).compare("TOPIC") || !(args.front()).compare("MODE"))
        {
            mod_commands(args, client);
        }
		else if (!(args.front()).compare("PING") || !(args.front()).compare("PONG")) {
			return ;
		} 
		else 
        {
			//:irc.example.com 421 your_nick COMMAND :Unknown command
            send_message((":" + host_name() + " 421 " + client->getNickname() + " " + args.at(0) + " :Unknown command" + "\r\n"), client);
            return ;
        }
    }
}

void Server::clientDiscon( int clientFd )
{
	this->buffers.erase(clientFd);
	for (size_t i = 0; i < this->clients.size(); i++)
	{
		if ((this->clients[i])->getClientFd() == clientFd)
			(this->clients).erase(this->clients.begin() + i);
	}
	for (size_t i = 0; i < this->clientSockets.size(); i++)
	{
		if (this->clientSockets[i].fd == clientFd)
			this->clientSockets.erase(this->clientSockets.begin() + i);
	}
	close(clientFd);
}

void	Server::delete_client(Client *client)
{
	std::map<std::string, channel*>::iterator it;

    for (it = channels.begin(); it != channels.end(); ++it)
	{
		if ((it->second)->is_member(client->getNickname()))
			(it->second)->delete_client(client);
    }
	clientDiscon(client->getClientFd());
}

Server::~Server()
{
	if(serverFd != -1)
		close(serverFd);
	for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
        delete *it;
	for (std::map<std::string, channel *>::iterator it = channels.begin(); it != channels.end(); ++it) {
        delete it->second; 
    }
	channels.clear();
	clients.clear();
}