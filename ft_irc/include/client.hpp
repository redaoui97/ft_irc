#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "../include/irc.hpp"
#include "../include/server.hpp"

class Client {

	private:
		int			m_clientFd;
		std::string m_username;
		std::string m_hostname;
		std::string m_nickname;
		std::string	m_servername;
		std::string	m_realname;
		std::string m_password_provided;
		std::string	m_ip;
		bool		m_isauthenticated;
		bool		m_right_password;
		Server		*current_server;
		
	public:
		Client(int clientFd, char* ip);
		~Client();
		//getters
		int 		getClientFd() const;
		std::string getUsername() const;
		std::string getHostname() const;
		std::string getNickname() const;
		std::string	getServername() const;
		std::string	getRealname() const;
		std::string isPasswordProvided() const;
		std::string	getIp() const;
		bool		IsAuthenticated() const;
		bool		HasRightPassword() const;
		Server		*GetServer();
		//setters
		void setClientFd(int fd);
		void setUsername(std::string username);
		void setHostname(std::string hostname);
		void setNickname(std::string nickname);
		void setServername(std::string servername);
		void setRealname(std::string realname);
		void setIp(std::string ip);
		void AuthenticationStatus(bool status);
		void SetRightPassword(bool status);
		void SetServer(Server *server);
};

#endif
