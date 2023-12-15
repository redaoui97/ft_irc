#include "client.hpp"


channel::channel(std::string name, Client *client)
{
	m_name = name;
	m_topic= "";
	m_invite_only = false;
	add_mod(client);
}

void channel::change_topic(std::string topic)
{
	m_topic = topic;
}

std::string channel::get_topic()
{
	return (m_topic);
}

void channel::add_client(Client *client)
{
	m_members.insert(std::make_pair(client->getNickname(), client));
}

void channel::add_mod(Client *client)
{
	m_moderators.insert(std::make_pair(client->getNickname(), client));
}
void channel::delete_client(Client *client)
{
	m_members.erase(client->getNickname());
}
void channel::delete_mod(Client *client)
{
	m_moderators.erase(client->getNickname());
}

channel::~channel()
{
	std::cout << "Channel deleted!" << std::endl;
}