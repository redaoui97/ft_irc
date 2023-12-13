#include"../include/client.hpp"
#include "channel.hpp"

Client::Client(int clientFd)
{
	m_isauthenticated = 0;
	m_clientFd = clientFd;
	m_username = "";
	m_hostname = "";
	m_nickname = "";
	m_servername = "";
	m_realname = "";
	m_password_provided = "";
	m_ip = "";
}

int Client::getClientFd() const
{
	return m_clientFd;
}

//getters
std::string Client::getUsername() const
{
	return (this->m_username);
}
std::string Client::getHostname() const
{
	return(this->m_hostname);
}
std::string Client::getNickname() const
{
	return (this->m_nickname);
}
std::string	Client::getServername() const
{
	return (this->m_servername);
}
std::string	Client::getRealname() const
{
	return (this->m_realname);
}
std::string Client::isPasswordProvided() const
{
	return (this->m_password_provided);
}
std::string	Client::getIp() const
{
	return (this->m_ip);
}
bool		Client::IsAuthenticated() const
{
	return (this->m_isauthenticated);
}

//setters
void Client::setClientFd(int fd)
{
	this->m_clientFd = fd;
}
void Client::setUsername(std::string username)
{
	this->m_username = username;
}
void Client::setHostname(std::string hostname)
{
	this->m_hostname = hostname;
}
void Client::setNickname(std::string nickname)
{
	this->m_nickname = nickname;
}
void Client::setServername(std::string servername)
{
	this->m_servername = servername;
}
void Client::setRealname(std::string realname)
{
	this->m_realname = realname;
}
void Client::setIp(std::string ip)
{
	this->m_ip = ip;
}
void Client::AuthenticationStatus(bool status)
{
	this->m_isauthenticated = status;
}

Client::~Client()
{
	close(m_clientFd);
}
