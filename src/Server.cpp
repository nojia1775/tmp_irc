#include "../include/Server.hpp"
#include <string.h>

bool Server::Signal = false;
void Server::SignalHandler(int signum)
{
	(void)signum;
	std::cout << std::endl << "Signal Received!" << std::endl;
	Server::Signal = true;
}

void Server::CloseFds(){
	for(size_t i = 0; i < clients.size(); i++)
	{
		std::cout << RED << "Client <" << this->clients[i].getFd() << "> Disconnected" << WHITE << std::endl;
		close(this->clients[i].getFd());
	}
	if (this->ServSocket != -1)
	{
		std::cout << RED << "Server <" << this->ServSocket << "> Disconnected" << WHITE << std::endl;
		close(this->ServSocket);
	}
}

void Server::SerSocket()
{
	struct sockaddr_in add;
	struct pollfd NewPoll;
	add.sin_family = AF_INET;
	add.sin_port = htons(this->Port);
	add.sin_addr.s_addr = INADDR_ANY;
	int en = 1;

	this->ServSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(this->ServSocket == -1)
		throw(std::runtime_error("faild to create socket"));

	if(setsockopt(this->ServSocket, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1)
		throw(std::runtime_error("faild to set option (SO_REUSEADDR) on socket"));
	if (fcntl(this->ServSocket, F_SETFL, O_NONBLOCK) == -1)
		throw(std::runtime_error("faild to set option (O_NONBLOCK) on socket"));
	if (bind(this->ServSocket, (struct sockaddr *)&add, sizeof(add)) == -1)
		throw(std::runtime_error("faild to bind socket"));
	if (listen(this->ServSocket, SOMAXCONN) == -1)
		throw(std::runtime_error("listen() faild"));

	NewPoll.fd = ServSocket;
	NewPoll.events = POLLIN;
	NewPoll.revents = 0;
	this->fds.push_back(NewPoll);
}

void Server::AcceptIncomingClient()
{
	struct sockaddr_in cliadd;
	struct pollfd NewPoll;
	socklen_t len = sizeof(cliadd);

	int incofd = accept(this->ServSocket, (sockaddr *)&(cliadd), &len);
	if (incofd == -1)
	{
		std::cout << "accept() failed" << std::endl; 
		return;
	}
	if (fcntl(incofd, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cout << "fcntl() failed" << std::endl; 
		return;
	}
	NewPoll.fd = incofd;
	NewPoll.events = POLLIN;
	NewPoll.revents = 0;

	this->clients.push_back(Client(incofd, cliadd.sin_addr));
	this->fds.push_back(NewPoll);
	std::cout << GREEN << "Client <" << incofd << "> Connected" << WHITE << std::endl;
}

void	Server::handleCmd(const int& fd, const std::vector<std::string>& input)
{
	std::string cmd = input[0];
	std::transform(cmd.begin(), cmd.end(), cmd.begin(), toupper);
	if (cmd == "/CAP")
		return;
	else if (cmd == "/PASS")
	{
		if (strcmp(input[1].c_str(), Mdp))
			std::cout << "Wrong password" << std::endl;
		else
			std::cout << "Good password" << std::endl;
	}
	else if (cmd == "/NICK")
	{
		std::vector<Client>::iterator client = std::find(clients.begin(), clients.end(), fd);
		if (client != clients.end())
			client->setNick(input[1]);
		std::cout << "Nick name set to " << input[1] << std::endl;
	}
	else if (cmd == "/USER")
	{
		std::vector<Client>::iterator client = std::find(clients.begin(), clients.end(), fd);
		if (client != clients.end())
			client->setUser(input[1]);
		std::cout << "User name set to " << input[1] << std::endl;
	}
	else if (cmd == "/QUIT")
		ClearClients(fd);
	else
		std::cout << "Command unknown" << std::endl;
}

void	Server::HandleCmd(int fd, std::string str, int i, char *buff)
{
	if (str == "CAP")
		return ;
	else if (str == "PASS")
	{
		std::string arg;
		while (buff[i] != ' ' && buff[i] != '\n' && buff[i] != '\0')
		{
			arg += buff[i];
			i++;
		}
		arg[strlen(arg.c_str()) - 1] = '\0';
		if (strcmp((const char *)arg.c_str(), (const char *)this->Mdp))
			std::cout << "Wrong password" << std::endl;
	}
	else if (str == "NICK")
	{
		std::string arg;
		while (buff[i] != '\n' && buff[i] != '\0')
		{
			arg += buff[i];
			i++;
		}
		std::cout << arg << std::endl;
		std::vector<Client>::iterator client = std::find(clients.begin(), clients.end(), fd);
		if (client != clients.end())
			client->setNick(arg);
	}
	else if (str == "USER")
	{
		std::string arg;
		while (buff[i] != '\n' && buff[i] != '\0')
		{
			arg += buff[i];
			i++;
		}
		std::cout << arg << std::endl;
		std::vector<Client>::iterator client = std::find(clients.begin(), clients.end(), fd);
		if (client != clients.end())
			client->setUser(arg);
	}
	else if (str == "/QUIT")
		ClearClients(fd);
}

int Server::ParseData(int fd, char *buff)
{
	std::string str;
	std::cout << BLUE << std::find(clients.begin(), clients.end(), fd)->getNick() << RESET << buff;
	handleCmd(fd, splitInput(buff));
	return (0);
}

void Server::ReceiveDataClient(int fd)
{
	char buff[1024];
	memset(buff, 0, sizeof(buff));

	ssize_t bytes = recv(fd, buff, sizeof(buff) - 1 , 0);

	if(bytes <= 0)
	{
		std::cout << RED << "Client <" << fd << "> Disconnected" << WHITE << std::endl;
		ClearClients(fd);
		close(fd);
	}
	else
	{
		buff[bytes] = '\0';
		// std::cout << YEL << "Client <" << fd << "> Data: "<< std::endl << WHITE << buff;
		//here you can add your code to process the received data: parse, check, authenticate, handle the command, etc...
		ParseData(fd, buff);
	}
}

static void	parseArgs(const int& port, const char *mdp)
{
	if (port <= 1024 || port > 65535)
		throw std::runtime_error("port invalid");
	if (!(*mdp))
		throw std::runtime_error("the password cannot be empty");

}

void Server::ServerInit(int port, char *mdp)
{
	parseArgs(port, mdp);
	this->Port = port;
	this->Mdp = mdp;
	SerSocket();

	std::cout << GREEN << "Server <" << this->ServSocket << "> Connected" << WHITE << std::endl;
	std::cout << "Waiting to accept a connection...\n";

	while (Server::Signal == false)
	{
		if((poll(&this->fds[0], this->fds.size(), -1) == -1) && Server::Signal == false)
			throw(std::runtime_error("poll() faild"));

		for (size_t i = 0; i < this->fds.size(); i++)
		{
			if (this->fds[i].revents & POLLIN)
			{
				if (this->fds[i].fd == this->ServSocket)
					AcceptIncomingClient();
				else
					ReceiveDataClient(this->fds[i].fd); 
			}
		}
	}
	CloseFds();
}

void Server::ClearClients(int fd)
{
	std::cout << RED << "Client <" << fd << "> Disconnected" << WHITE << std::endl;
	for(size_t i = 0; i < this->fds.size(); i++)
	{
		if (this->fds[i].fd == fd)
		{
			this->fds.erase(this->fds.begin() + i); 
			break;
		}
	}
	for(size_t i = 0; i < this->clients.size(); i++)
	{
		if (this->clients[i].getFd() == fd)
		{
			this->clients.erase(this->clients.begin() + i); 
			break;
		}
	}
}
