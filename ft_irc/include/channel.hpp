#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include "../include/irc.hpp"

class channel
{
	private:
		std::string m_name;
		std::string m_topic;
		bool		m_invite_only;
		std::map<std::string, Client *> m_moderators;
		std::map<std::string, Client *> m_members;
	
	public:
		channel(std::string name, Client *client);
		void add_client(Client *client);
		void add_mod(Client *client);
		void delete_client(Client *client);
		void delete_mod(Client *client);
		void change_topic(std::string topic);
		std::string get_topic();
		~channel();
		
};
#endif