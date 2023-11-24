#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "irc.hpp"

class Client {

	public:
		Client(int clientFd);
		~Client();
		int getClientFd() const;

	private:
		int	clientFd;
};

#endif