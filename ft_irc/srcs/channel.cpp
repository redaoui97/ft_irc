#include "client.hpp"

channel::channel(std::string name, std::string password)
{
	m_name = name;
	m_topic= "";
	m_password = password;
	m_invite_only = false;
}

channel::~channel()
{
	std::cout << "Channel deleted!" << std::endl;
}