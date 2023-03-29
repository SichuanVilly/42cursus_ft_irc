/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_utils.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jlopez-f <jlopez-f@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/28 16:40:17 by pvillena          #+#    #+#             */
/*   Updated: 2023/02/15 00:38:22 by jlopez-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

std::string	Server::strtoupper(std::string const &str)
{
	std::string	temp = str;

	for (std::string::iterator it = temp.begin(); it != temp.end(); it++)
	{
		if (std::isalpha(*it))
			*it = std::toupper(*it);
	}
	return (temp);
}

std::string	gettime(void)
{
	char		date[22];
	time_t		currentdate;
	struct tm*	time_struct;

	time(&currentdate);
	time_struct = localtime(&currentdate);
	strftime(date, 22, "[%d/%m/%Y %H:%M:%S]", time_struct);
	return (std::string(date));
}

std::string	Server::fullname(User &user)
{
	std::string	name;

	name = ":" + user.getNickName() + "!" + user.getUserName() + "@" + user.getHost();
	return (name);
}

User	*Server::find_user(std::string const &nick)
{
	std::string	temp;
	std::string	tempnick;

	tempnick = strtoupper(nick);
	for (std::vector<User *>::iterator iter = _all_users.begin(); iter != _all_users.end(); iter++)
	{
		temp = strtoupper((*iter)->getNickName());
		if (temp == tempnick)
			return (*iter);
	}
	return (NULL);
}

User	*Server::find_user(int fd)
{
	for (std::vector<User *>::iterator iter = _all_users.begin(); iter != _all_users.end(); iter++)
	{
		if ((*iter)->getFd() == fd)
			return (*iter);
	}
	return (NULL);
}

std::list<std::string>	adv_tokenizer(std::string s, char del)
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

Channel *Server::newChannel(std::string channelName, User &newUser)
{
	Channel		*channel;
	std::string	name = channelName;

	if (name.size() > CHANNELLEN)
			name.erase(CHANNELLEN, std::string::npos);
	channel = new Channel(name);
	channel->addUser(newUser, "~");
	return (channel);
}

void	Server::deleteChannels(void)
{
	for (std::list<Channel *>::iterator iter = _all_channels.begin(); iter != _all_channels.end(); )
	{
		if ((*iter)->getUsersInChannel().empty())
		{
			delete *iter;
			iter = _all_channels.erase(iter);
			if (!_all_channels.size())
				return ;
		}
		else
			iter++;
	}
}

bool	Server::isoper(User *user, Channel *channel)
{
	if (user && user->getOper())
		return (true);
	if (channel && user->isinChannel(channel->getChannelName()))
	{
		if (channel->getUsersInChannel().find(user)->second == "~" || channel->getUsersInChannel().find(user)->second == "@")
			return (true);
	}
	return (false);
}

void	Server::deleteUser(User * user)
{
	if (!user)
		return ;
	if (!user->getChannels().empty())
	{
		for (std::list<Channel *>::iterator iter = _all_channels.begin(); iter != _all_channels.end(); iter++)
			(*iter)->removeUser(*user);
		deleteChannels();
	}
	for (std::vector<pollfd>::iterator it = _pollfds.begin(); it != _pollfds.end(); it++)
	{
		if (user->getFd() == (*it).fd)
		{
			close((*it).fd);
			_pollfds.erase(it);
			break ;
		}
	}
	_all_users.erase(std::find (_all_users.begin(), _all_users.end(), user));
	delete user;
}
