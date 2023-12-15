#include "client.hpp"

channel::channel(std::string name, Client *client, std::string password)
{
    if (password.empty())
        m_password_restrict = false;
    else
    {
        m_password_restrict = true;
        m_password = password;
    }
    m_name = name;
    m_topic= "";
    m_invite_only = false;
    add_mod(client);
    add_client(client);
}

bool    channel::is_mod(std::string nick)
{
    for (std::map<std::string, Client*>::const_iterator it = m_moderators.begin(); it != m_moderators.end(); ++it)
    {
        if (it->first == nick)
        {
            return true;
        }
    }
    return false; 
}

bool    channel::require_pw()
{
    if (m_password_restrict)
        return (true);
    return (false);
}

bool    channel::require_invite()
{
    if (m_invite_only)
        return (true);
    return (false);
}

bool    channel::is_member(std::string nick)
{
    for (std::map<std::string, Client*>::const_iterator it = m_members.begin(); it != m_members.end(); ++it)
    {
        if (it->first == nick)
        {
            return true;
        }
    }
    return false; 
}

bool    channel::is_right_pw(std::string password)
{
    if (password.compare(m_password) == 0)
        return (true);
    return (false);
}
bool    channel::is_invited(std::string nick)
{
    for (std::vector<std::string>::const_iterator it = m_invited_members.begin(); it != m_invited_members.end(); ++it) {
        if (*it == nick)
        {
            return true; 
        }
    }
    return false;
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