/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BotExecute.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jlopez-f <jlopez-f@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/17 20:03:31 by jlopez-f          #+#    #+#             */
/*   Updated: 2023/02/20 17:07:44 by jlopez-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Bot.hpp"

void	Bot::sendmsgto(int fd, std::string const &msg) const
{
	std::string				temp;
	std::string::size_type	nb = 0;

	temp = msg + "\n";
	//std::cout << GREEN << "Message sent: " << temp << NOCOLOR;
	nb = send(fd, temp.c_str(), temp.length(), 0);
	while (nb != temp.length())
	{
		temp.erase(temp.begin(), temp.begin() + nb);
		nb = send(fd, temp.c_str(), temp.length(), 0);
		std::cerr << RED << "Send error. Sending in packets" << NOCOLOR << std::endl;
	}
}

static std::string	getNick(Parser &result)
{
	std::string	nick = result.getSource();

	if (!nick.empty())
		nick.erase(std::find(nick.begin(), nick.end(), '!'), nick.end());
	return (nick);
}

void	Bot::login(Parser &result, int fd)
{
	static bool	logmsg[5];

	if (result.getCommand() == RPL_WELCOME)
		logmsg[0] = true;
	else if (result.getCommand() == RPL_YOURHOST)
		logmsg[1] = true;
	else if (result.getCommand() == RPL_CREATED)
		logmsg[2] = true;
	else if (result.getCommand() == RPL_MYINFO)
		logmsg[3] = true;
	else if (result.getCommand() == RPL_ISUPPORT)
		logmsg[4] = true;
	else
	{
		if (result.getCommand() == ERR_NEEDMOREPARAMS)
			std::cerr << RED << "Login error. Wrong parameters" << NOCOLOR << std::endl;
		else if (result.getCommand() == ERR_PASSWDMISMATCH)
			std::cerr << RED << "Login error. Wrong password" << NOCOLOR << std::endl;
		else if (result.getCommand() == ERR_ERRONEUSNICKNAME)
			std::cerr << RED << "Login error. Erroneous nick" << NOCOLOR << std::endl;
		else if (result.getCommand() == ERR_NICKNAMEINUSE)
			std::cerr << RED << "Login error. Nick already in use" << NOCOLOR << std::endl;
		close(fd);
		exit(ERR_BADLOGIN);
	}
	if (logmsg[0] && logmsg[1] && logmsg[2] && logmsg[3] && logmsg[4])
		_loged = true;
}

static std::list<std::string>	adv_tokenizer(std::string s, char del)
{
	std::list<std::string>	res;
	std::stringstream		ss(s);
	std::string				word;

	while (!ss.eof())
	{
		std::getline(ss, word, del);
		res.push_back(word);
	}
	return (res);
}

bool	Bot::getmode(Parser &result, std::string &channel, std::string &nick)
{
	std::list<std::string> mode = adv_tokenizer(result.getParam()[1], ' ');

	if (mode.size() == 2 && mode.front() == "MODE")
	{
		std::map<std::string, std::string>::iterator	it = _invitinguser.find(mode.back());

		if (it != _invitinguser.end())
		{
			if (it->second == nick)
				channel = _invitinguser.find(mode.back())->first;
			return (true);
		}
	}
	return (false);
}

void	Bot::privmsg(Parser &result, int fd)
{
	std::string	nick = getNick(result);
	std::string	modechannel;

	if (nick.empty() || result.getParam().size() < 2)
		return ;
	if (result.getCommand() == "PRIVMSG" && getmode(result, modechannel, nick))
	{
		if (!modechannel.empty())
			sendmsgto(fd, "MODE " + modechannel + " +o " + nick);
		else
			sendmsgto(fd, "PRIVMSG " + nick + " :Sorry, but you didn't invite me to this channel. I can't give you any permissions");
	}
	else if (result.getCommand() == ERR_NOSUCHCHANNEL)
	{
		std::map<std::string, std::string>::iterator	it = _invitinguser.find(result.getParam()[1]);

		if (nick.empty() || it == _invitinguser.end())
			return ;
		nick = it->second;
		sendmsgto(fd, "PRIVMSG " + nick + " :Sorry, channel doesn't exist anymore. I can't give you any permissions");
	}
	else if (result.getCommand() == ERR_CHANOPRIVSNEEDED)
	{
		std::map<std::string, std::string>::iterator	it = _invitinguser.find(result.getParam()[1]);

		if (nick.empty() || it == _invitinguser.end())
			return ;
		nick = it->second;
		sendmsgto(fd, "PRIVMSG " + nick + " :Sorry, I'm not a channel operator. I can't give you any permissions");
	}
	else
		sendmsgto(fd, "PRIVMSG " + nick + " :I can only keep your channels up, and if you give me oper permissions, give it back to you after that with \"MODE channel\". For just chatting, please, try with ChatGPT");
}

void	Bot::invite(Parser &result, int fd)
{
	std::string	nick = getNick(result);
	
	if (nick.empty())
		return ;
	if (result.getCommand() == "INVITE" && result.getParam().size() == 2)
	{
		_invitinguser.insert(std::make_pair(result.getParam()[1], nick));
		sendmsgto(fd, "JOIN " + result.getParam()[1]);
	}
	else if (result.getParam().size())
	{
		std::map<std::string, std::string>::iterator	it = _invitinguser.find(result.getParam()[0]);

		if (it != _invitinguser.end())
		{
			if (result.getCommand() == "JOIN")
			{
				sendmsgto(fd, "PRIVMSG " + it->second + " :Thanks for inviting me to " + it->first + "! I'll keep this channel warm for you!");
				return ;
			}
			else if (result.getCommand() == ERR_TOOMANYCHANNELS)
				sendmsgto(fd, "PRIVMSG " + it->second + " :Thanks for inviting me to " + it->first + ", but I'm on too many channels right now :(");
			else if (result.getCommand() == ERR_BANNEDFROMCHAN)
				sendmsgto(fd, "PRIVMSG " + it->second + " :Thanks for inviting me to " + it->first + ", but I think someone banned me :(");
			else if (result.getCommand() == ERR_BADCHANMASK)
				sendmsgto(fd, "PRIVMSG " + it->second + " :Thanks for inviting me to " + it->first + ", but it doesn't seem to be a right channel name :(");
			_invitinguser.erase(it);
		}
	}
}

void	Bot::ping(Parser &result, int fd)
{
	if (result.getParam().size())
		sendmsgto(fd, "PONG :" + result.getParam()[0]);
	else
		sendmsgto(fd, "PONG :");
}

void	Bot::execute(Parser &result, int fd)
{
	if (!_loged)
		login(result, fd);
	if (result.getCommand() == "PRIVMSG" || result.getCommand() == ERR_NOSUCHCHANNEL || result.getCommand() == ERR_CHANOPRIVSNEEDED)
		privmsg(result, fd);
	else if (result.getCommand() == "INVITE" || (result.getCommand() == "JOIN" && getNick(result) == "WeabSegBot") || result.getCommand() == ERR_TOOMANYCHANNELS ||
	result.getCommand() == ERR_BANNEDFROMCHAN || result.getCommand() == ERR_BADCHANMASK)
		invite(result, fd);
	else if (result.getCommand() == "PING")
		ping(result, fd);
}
