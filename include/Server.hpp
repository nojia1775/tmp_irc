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

class	Server
{
	private:
		int Port;
		int ServSocket;
		static bool Signal;
		char *Mdp;
		std::vector<Client> clients;
		std::vector<struct pollfd> fds;

	public:
		Server() { ServSocket = -1; }
		~Server(void) { CloseFds(); }

		void ServerInit(int port, char *mdp);
		void SerSocket();
		void AcceptIncomingClient();
		void ReceiveDataClient(int fd);
		int	ParseData(int fd, char *buff);
		void HandleCmd(int fd, std::string str, int i, char *buffer);
		void	handleCmd(const int& fd, const std::vector<std::string>& input);
		void	nick(const int& fd, const std::vector<std::string>& input);
		void	user(const int& fd, const std::vector<std::string>& input);
		void	quit(const int& fd);
		void	pass(const int& fd, const std::vector<std::string>& input);

		static void SignalHandler(int signum);
	
		void CloseFds();
		void ClearClients(int fd);
};
