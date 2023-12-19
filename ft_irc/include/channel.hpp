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
		std::map<std::string, Client *> m_moderators;
		std::map<std::string, Client *> m_members;
		std::vector<std::string> m_invited_members;
		bool		m_invite_only;
		bool		m_password_restrict;
		bool		m_topic_restricted;
		bool		m_user_restricted;
		
	public:
		channel(std::string name, Client *client, std::string password);
		void change_topic(std::string topic);
		std::string get_topic();
		void add_client(Client *client);
		void add_mod(Client *client);
		void delete_client(Client *client);
		void delete_mod(Client *client);
		bool is_invited(std::string nick);
		bool is_mod(std::string nick);
		bool is_member(std::string nick);
		bool is_right_pw(std::string password);
		bool require_pw();
		bool require_invite();
		void add_toinvite(Client *client);
		void set_inv_status(bool status);
		void set_topic_restriction(bool status);
		void set_user_restriction(bool status);
		void remove_mod(Client *client);
		void set_newpw(std::string password);
		void pw_restriction_status(bool status);
		std::map<std::string, Client *> all_clients();
		bool is_user_restricted();
		~channel();
		
};
#endif