#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include "../include/irc.hpp"
#include "../include/client.hpp"

class channel
{
	private:
		std::string m_name;
		std::string m_topic;
		std::string	m_password;
		bool		m_invite_only;
		std::map<std::string, Client *> m_moderators;
		std::map<std::string, Client *> m_members;
	
	public:
		channel(std::string name, std::string password);
		~channel();
		
};
#endif