/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerExecute.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jlopez-f <jlopez-f@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/18 12:45:29 by pvillena          #+#    #+#             */
/*   Updated: 2023/02/20 17:15:19 by jlopez-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IRC.hpp"

void	Server::sendToAll(User const &user, std::string const &msg) const
{
	for (std::vector<User *>::const_iterator it = getAllUsers().begin(); it != getAllUsers().end(); it++)
	{
		if ((*it)->getFd() != user.getFd() && (*it)->shareChannel(user))
			sendmsgto(**it, msg);
	}
}

void	Server::sendToChannel(User &user, Channel &channel, std::string const &msg) const
{
	for (std::map<User *, std::string>::const_iterator it = channel.getUsersInChannel().begin(); it != channel.getUsersInChannel().end(); it++)
	{
		if (it->first != &user)
			sendmsgto(*it->first, msg);
	}
}

void	Server::sendservmsg(User const &user, std::string const &msg, std::string const &code) const
{
	std::string	temp;

	if (!user.getNickName().empty())
		temp = ":" + _ip + " " + code + " " + user.getNickName() + " " + msg;
	else
		temp = ":" + _ip + " " + code + " " + msg;
	sendmsgto(user, temp);
}

void	Server::sendmsgto(User const &user, std::string const &msg) const
{
	std::string				temp;
	std::string::size_type	nb = 0;

	temp = msg + "\n";
	//std::cout << GREEN << "Message sent: " << temp << NOCOLOR;
	nb = send(user.getFd(), temp.c_str(), temp.length(), 0);
	while (nb != temp.length())
	{
		temp.erase(temp.begin(), temp.begin() + nb);
		nb = send(user.getFd(), temp.c_str(), temp.length(), 0);
		std::cerr << RED << "Send error. Sending in packets" << NOCOLOR << std::endl;
	}
}

void	Server::sendmotd(User const &user) const
{
	sendservmsg(user, MOTD_GIRL01, RPL_MOTD);
	sendservmsg(user, MOTD_GIRL02, RPL_MOTD);
	sendservmsg(user, MOTD_GIRL03, RPL_MOTD);
	sendservmsg(user, MOTD_GIRL04, RPL_MOTD);
	sendservmsg(user, MOTD_GIRL05, RPL_MOTD);
	sendservmsg(user, MOTD_GIRL06, RPL_MOTD);
	sendservmsg(user, MOTD_GIRL07, RPL_MOTD);
	sendservmsg(user, MOTD_GIRL08, RPL_MOTD);
	sendservmsg(user, MOTD_GIRL09, RPL_MOTD);
	sendservmsg(user, MOTD_GIRL10, RPL_MOTD);
	sendservmsg(user, MOTD_GIRL11, RPL_MOTD);
	sendservmsg(user, MOTD_GIRL12, RPL_MOTD);
	sendservmsg(user, MOTD_GIRL13, RPL_MOTD);
	sendservmsg(user, MOTD_GIRL14, RPL_MOTD);
	sendservmsg(user, MOTD_GIRL15, RPL_MOTD);
	sendservmsg(user, MOTD_GIRL16, RPL_MOTD);
	sendservmsg(user, MOTD_GIRL17, RPL_MOTD);
	sendservmsg(user, MOTD_GIRL18, RPL_MOTD);
	sendservmsg(user, MOTD_GIRL19, RPL_MOTD);
	sendservmsg(user, MOTD_GIRL20, RPL_MOTD);
	sendservmsg(user, MOTD_GIRL21, RPL_MOTD);
	sendservmsg(user, MOTD_GIRL22, RPL_MOTD);
	sendservmsg(user, MOTD_GIRL23, RPL_MOTD);
	sendservmsg(user, MOTD_GIRL24, RPL_MOTD);
	sendservmsg(user, MOTD_GIRL25, RPL_MOTD);
	sendservmsg(user, MOTD_GIRL26, RPL_MOTD);
	sendservmsg(user, MOTD_GIRL27, RPL_MOTD);
	sendservmsg(user, MOTD_GIRL28, RPL_MOTD);
	sendservmsg(user, MOTD_GIRL29, RPL_MOTD);
	sendservmsg(user, MOTD_GIRL30, RPL_MOTD);
	sendservmsg(user, MOTD_GIRL31, RPL_MOTD);
	sendservmsg(user, MOTD_GIRL32, RPL_MOTD);
	sendservmsg(user, MOTD_GIRL33, RPL_MOTD);
	sendservmsg(user, MOTD_GIRL34, RPL_MOTD);
	sendservmsg(user, MOTD_GIRL35, RPL_MOTD);
	sendservmsg(user, MOTD_GIRL36, RPL_MOTD);
	sendservmsg(user, MOTD_GIRL37, RPL_MOTD);
	sendservmsg(user, MOTD_GIRL38, RPL_MOTD);
	sendservmsg(user, MOTD_GIRL39, RPL_MOTD);
	sendservmsg(user, MOTD_GIRL40, RPL_MOTD);
}

void	Server::connaccept(User const &user) const
{
	std::string	msg;

	msg = RPL_WELCOME_MSG + user.getNickName() + "!" + user.getUserName() + "@" + user.getHost();
	sendservmsg(user, msg, RPL_WELCOME);
	sendservmsg(user, RPL_YOURHOST_MSG, RPL_YOURHOST);
	msg = RPL_CREATED_MSG + _date;
	sendservmsg(user, msg, RPL_CREATED);
	sendservmsg(user, RPL_MYINFO_MSG, RPL_MYINFO);
	sendservmsg(user, RPL_ISUPPORT_MSG, RPL_ISUPPORT);
	sendservmsg(user, RPL_MOTDSTART_MSG, RPL_MOTDSTART);
	sendmotd(user);
	sendservmsg(user, RPL_ENDOFMOTD_MSG, RPL_ENDOFMOTD);
}

bool	Server::is_user_valid(User &user, Parser const &result)
{
	if (user.getValid())
		return (true);
	if (!user.getPass())
	{
		if (result.getCommand() != "PASS")
			return (false);
		pass(result, user);
		if (!user.getPass())
			return (false);
	}
	if (user.getNickName().empty())
	{
		if (result.getCommand() != "NICK")
			return (false);
		nick(result, user);
		if (user.getNickName().empty())
			return (false);
	}
	if (user.getUserName().empty())
	{
		if (result.getCommand() != "USER")
			return (false);
		usercmd(result, user);
		if (user.getUserName().empty())
			return (false);
	}
	user.setValid(true);
	connaccept(user);
	return (false);
}

void	Server::pingusers(void)
{
	for (std::vector<User *>::iterator it = _all_users.begin(); it != _all_users.end(); )
	{
		if (!(*it)->getValid() && std::difftime(time(NULL), (*it)->getConnectTime()) > AFK_TIMEOUT)
		{
			sendmsgto(**it, ":" + _ip + " ERROR :Connection timeout");
			deleteUser(*it);
			if (!_all_users.size())
				return ;
			it = _all_users.begin();
		}
		else if ((*it)->getValid() && std::difftime(time(NULL), (*it)->getLastTime()) > AFK_TIMEOUT)
		{
			std::string	temp = ":" + _ip + " ERROR :Connection timeout";

			sendmsgto(**it, temp);
			temp = fullname(**it) + " QUIT :Quit: Connection timeout";
			sendToAll(**it, temp);
			deleteUser(*it);
			if (!_all_users.size())
				return ;
			it = _all_users.begin();
		}
		else if (!(*it)->getPinged() && std::difftime(time(NULL), (*it)->getLastTime()) >= PING_FQ)
		{
			sendmsgto(**it, ":" + _ip + " PING :" + gettime());
			(*it)->setPinged(true);
			it++;
		}
		else if ((*it)->getPinged() && std::difftime(time(NULL), (*it)->getLastTime()) < PING_FQ)
		{
			(*it)->setPinged(false);
			it++;
		}
		else
			it++;
	}
}

void	Server::commands(void)
{
	_commands["PASS"] = &Server::pass;
	_commands["NICK"] = &Server::nick;
	_commands["USER"] = &Server::usercmd;
	_commands["PING"] = &Server::ping;
	_commands["OPER"] = &Server::oper;
	_commands["QUIT"] = &Server::quit;
	_commands["JOIN"] = &Server::join;
	_commands["PART"] = &Server::part;
	_commands["TOPIC"] = &Server::topic;
	_commands["NAMES"] = &Server::names;
	_commands["LIST"] = &Server::list;
	_commands["INVITE"] = &Server::invite;
	_commands["KICK"] = &Server::kick;
	_commands["MOTD"] = &Server::motd;
	_commands["VERSION"] = &Server::version;
	_commands["ADMIN"] = &Server::admin;
	_commands["LUSERS"] = &Server::lusers;
	_commands["TIME"] = &Server::timecmd;
	_commands["INFO"] = &Server::info;
	_commands["MODE"] = &Server::mode;
	_commands["PRIVMSG"] = &Server::privmsg;
	_commands["NOTICE"] = &Server::notice;
	_commands["WHO"] = &Server::who;
	_commands["WHOIS"] = &Server::whois;
	_commands["KILL"] = &Server::kill;
	_commands["AWAY"] = &Server::away;
	_commands["WALLOPS"] = &Server::wallops;
}

void	Server::execute(Parser &result, User &user)
{
	if (!result.command_check())
	{
		if (user.getValid())
			sendservmsg(user, ERR_UNKNOWNCOMMAND_MSG, ERR_UNKNOWNCOMMAND);
		return ;
	}
	if (!is_user_valid(user, result))
		return ;
	if (result.getCommand() == "PASS" || result.getCommand() == "USER")
		sendservmsg(user, ERR_ALREADYREGISTERED_MSG, ERR_ALREADYREGISTERED);
	else if (_commands.find(result.getCommand()) != _commands.end())
		(this->*_commands.find(result.getCommand())->second)(result, user);
	else if (result.getCommand() != "PONG")
		sendservmsg(user, result.getCommand() + " " + ERR_UNKNOWNCOMMAND_MSG,ERR_UNKNOWNCOMMAND);
}
