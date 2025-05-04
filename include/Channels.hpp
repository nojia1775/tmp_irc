#pragma once

#include <iostream>
#include <cstdlib>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <csignal>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <algorithm>
#include <vector>
#include <poll.h>
#include <cctype>
#include "Clients.hpp"
#include "Colors.h"
#include "tools.hpp"

class	Channel
{
	private:
		std::vector<Client>	_clients;
		std::string		_name;

	public:
					Channel(const std::string& name) : _name(name) {}
					~Channel(void) {}
					Channel(const Channel& channel) : _name(channel._name) {}

		Channel&		operator=(const Channel& channel) { (void)channel; return *this; }

		const std::string&	getName(void) const { return _name; }

		void			setName(const std::string& name) { _name = name; }
};