#pragma once

#include <iostream>

class Client
{
	private:
		std::string		_nick;
		std::string		_user;
		bool			connected;
		bool			admin;
		int			Fd;
		std::string		IPadd;

	public:
					Client(const int& fd, const in_addr& sin_addr) : connected(false), admin(false), Fd(fd), IPadd(inet_ntoa(sin_addr)) {}

		const int& 		getFd(void) const { return Fd; }
		const std::string&	getNick(void) const { return _nick; }
		const std::string&	getUser(void) const { return _user; }

		const bool&		isConnected(void) const { return connected; }
		const bool&		isAdmin(void) const { return admin; }

		void			setFd(int fd){Fd = fd;}
		void			setNick(const std::string& nick) { _nick = nick; }
		void			setUser(const std::string& user) { _user = user; }
		void			setIpAdd(std::string ipadd) { IPadd = ipadd; }

		bool 			operator==(const Client& client) const { return Fd == client.Fd; }
		bool 			operator==(const int& fd) const { return Fd == fd; }
};