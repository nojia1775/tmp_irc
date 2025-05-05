#include "../include/Channel.hpp"

Channel::Channel(Client& client, const std::string& name) : _name(name)
{
	client.setAdmin(true);
	_clients.push_back(client);
	pollfd clientFd = {
		.fd = client.getFd(),
		.events = POLLIN,
		.revents = 0,
	};
	_pollfds.push_back(clientFd);
}

void	Channel::sendMessage(const char *message) const
{
	for (size_t i = 0 ; i < _clients.size() ; i++)
		send(_clients[i].getFd(), message, strlen(message), 0);
}

void	Channel::join(Client& client)
{
	_clients.push_back(client);
	pollfd clientFd = {
		.fd = client.getFd(),
		.events = POLLIN,
		.revents = 0,
	};
	_pollfds.push_back(clientFd);
}