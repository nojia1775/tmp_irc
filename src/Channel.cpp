#include "../include/Channel.hpp"

Channel::Channel(Client& client, const std::string& name) : _name(name)
{
	client.setAdmin(true);
}

void	Channel::sendMessage(const char *message) const
{
	for (size_t i = 0 ; i < _clients.size() ; i++)
		send(_clients[i].getFd(), message, strlen(message), 0);
}