#include "../include/server.hpp"

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
        if (nick.compare((*it)->getNickname()) == 0)
		{
			return (true);
		}
    }
	return (false);
}

bool	Server::initializeServer(int port)
{
	struct sockaddr_in ServerAddr;

	this->port = port;
	serverFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverFd == -1) {
		throw SocketInitException("failed to create socket!");
		return false;
	}

	if (fcntl(serverFd, F_SETFL, O_NONBLOCK) == -1) {
		throw SocketInitException("failed to set-up NonBlocking mode");
		return false;
	}

	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(port);
	ServerAddr.sin_addr.s_addr = inet_addr("0.0.0.0");

	if(bind(serverFd, (sockaddr*)&ServerAddr, sizeof(ServerAddr)) == -1 ) {
		throw SocketInitException("failed to bind socket to port!");
		close(serverFd);
		return false;
	}

	if(listen(serverFd, 10) == -1) {
		throw SocketInitException("failed to set-up listener!");
		close(serverFd);
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

channel* create_channel(std::string name, Client *client)
{
	return (new channel(name, client));
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
        std::cout << "New Client added. IP: " << str << std::endl;

    } else {
        normal_error("Reject Connection: Error max Clients.");
        close(clientFd);
    }
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
    return (nullptr);
}

void	Server::clientData(int clientFd)
{
	ssize_t		bytes;
	char 		buffer[512];
	std::string	full_buffer;

	if (clientFd < 0)
	{
		normal_error("Error bad client file discriptor");
		return ;
	}
	//clear buffer after each iteration
	do
	{
		clear_buffer(buffer, 512);
		bytes = recv(clientFd, buffer, sizeof(buffer), 0);
		if (bytes <= 0)
		{
			if (bytes == 0)
			{
				normal_error("Client closed connection");
			}
			else if (errno == EBADF)
			{
				normal_error("Error: Bad file descriptor");
			} 
			else
			{ 
				normal_error("Error occurred during Client connection");
			}
			close(clientFd);
			return ;
		}
		else
		{
			full_buffer += buffer;
			find_user(clientFd)->SetServer(this);
			if (bytes != 512)
				process_command(full_buffer, find_user(clientFd), this->password);
		}
	} while (bytes == 512);
}

Server::~Server()
{
	if(serverFd != -1)
		close(serverFd);
	for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
        delete *it;
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