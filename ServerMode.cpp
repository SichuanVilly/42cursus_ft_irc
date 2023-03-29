/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_mode.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jlopez-f <jlopez-f@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/14 16:25:42 by jlopez-f          #+#    #+#             */
/*   Updated: 2023/02/17 13:15:37 by jlopez-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/* Mode */

static void	addargument(std::pair<std::string, std::string> &mode, std::vector<std::string> &args)
{
	if (args.size() && (mode.first[1] == 'b' || mode.first == "+l" || mode.first[1] == 'k' || mode.first[1] == 'o' || mode.first[1] == 'v'))
	{
		mode.second = args.front();
		args.erase(args.begin());
	}
}

bool	Server::parsemodes(std::string &modes, User &user, Channel *chan)
{
	std::string	modechars;
	std::string	temp;
	std::string	parse;

	if (chan)
	{
		modechars = MODE_CHANN;
		if (modes.size() == 1 && modes[0] == 'b')
		{
			modes.insert(0, "+");
			return (true);
		}
	}
	else
		modechars = MODE_USER;
	for (std::string::iterator it = modes.begin(); it != modes.end(); )
	{
		temp = *it;
		if (parse.empty() && (temp != "+" && temp != "-"))
		{
			sendservmsg(user, temp + " " + ERR_UNKNOWNMODE_MSG, ERR_UNKNOWNMODE);
			it = modes.erase(it);
			if (!modes.size())
				return (false);
		}
		else if (temp == "+" || temp == "-")
		{
			if (parse == "sign")
			{
				sendservmsg(user, temp + " " + ERR_UNKNOWNMODE_MSG, ERR_UNKNOWNMODE);
				it = modes.erase(it);
				if (!modes.size())
					return (false);
			}
			else
			{
				parse = "sign";
				it++;
			}
		}
		else if (std::find(modechars.begin(), modechars.end(), *it) == modechars.end())
		{
			sendservmsg(user, temp + " " + ERR_UNKNOWNMODE_MSG, ERR_UNKNOWNMODE);
			it = modes.erase(it);
			if (!modes.size())
				return (false);
		}
		else
		{
			parse = "letter";
			it++;
		}
	}
	if (modes.size() > 1)
		return (true);
	return (false);
}

std::string	Server::getmodesUser(User &user)
{
	std::string	s = "+";
	std::string	param;

	if (user.getInvis())
		s += "i";
	if (user.getOper())
		s += "o";
	if (user.getWall())
		s += "w";
	if (s == "+")
		s.clear();
	return (s);
}

std::string	Server::getmodesChan(Channel *chan, User *user)
{
	std::string	s = " +";
	std::string	param;

	if (chan->getBanlist().size())
		s += "b";
	if (chan->getLimit())
	{
		std::stringstream	temp;
		
		s += "l";
		temp << chan->getLimit();
		param += temp.str();
	}
	if (chan->getInvite())
		s += "i";
	if (!chan->getKey().empty())
	{
		s += "k";
		if (isoper(user, chan))
		{
			if (!param.empty())
				param += " ";
			param += chan->getKey();
		}
	}
	if (chan->getModer())
		s += "m";
	if (chan->getSecret())
		s += "s";
	if (chan->getTopicprot())
		s += "t";
	if (chan->getExternal())
		s += "n";
	if (!param.empty())
		s += " " + param;
	if (s == " +")
		s.clear();
	return (s);
}

bool	Server::invismode(User &user, std::string mode)
{
	if (mode[0] == '+' && !user.getInvis())
		user.setInvis(true);
	else if (mode[0] == '-' && user.getInvis())
		user.setInvis(false);
	else
		return (false);
	return (true);
}

bool	Server::wallmode(User &user, std::string mode)
{
	if (mode[0] == '+' && !user.getWall())
		user.setWall(true);
	else if (mode[0] == '-' && user.getWall())
		user.setWall(false);
	else
		return (false);
	return (true);
}

bool	Server::voicechanmode(Channel &chan, std::pair<std::string, std::string> mode)
{
	User		*user = find_user(mode.second);
	std::string	*chanmode;

	if (!user || !user->isinChannel(chan.getChannelName()))
		return (false);
	chanmode = &chan.getUsersInChannel().find(user)->second;
	if (*chanmode == "~" || *chanmode == "@")
		return (false);
	if (mode.first[0] == '+' && *chanmode != "+")
		*chanmode = "+";
	else if (mode.first[0] == '-' && *chanmode == "+")
		*chanmode = "";
	else
		return (false);
	return (true);
}

bool	Server::operchanmode(Channel &chan, std::pair<std::string, std::string> mode)
{
	User		*user = find_user(mode.second);
	std::string	*chanmode;

	if (!user || !user->isinChannel(chan.getChannelName()))
		return (false);
	chanmode = &chan.getUsersInChannel().find(user)->second;
	if (*chanmode == "~")
		return (false);
	if (mode.first[0] == '+' && *chanmode != "@")
		*chanmode = "@";
	else if (mode.first[0] == '-' && *chanmode == "@")
		*chanmode = "";
	else
		return (false);
	return (true);
}

bool	Server::externalmode(Channel &chan, std::pair<std::string, std::string> mode)
{
	if (mode.first[0] == '+' && !chan.getExternal())
		chan.setExternal(true);
	else if (mode.first[0] == '-' && chan.getExternal())
		chan.setExternal(false);
	else
		return (false);
	return (true);
}

bool	Server::protetopicmode(Channel &chan, std::pair<std::string, std::string> mode)
{
	if (mode.first[0] == '+' && !chan.getTopicprot())
		chan.setTopicprot(true);
	else if (mode.first[0] == '-' && chan.getTopicprot())
		chan.setTopicprot(false);
	else
		return (false);
	return (true);
}

bool	Server::secretmode(Channel &chan, std::pair<std::string, std::string> mode)
{
	if (mode.first[0] == '+' && !chan.getSecret())
		chan.setSecret(true);
	else if (mode.first[0] == '-' && chan.getSecret())
		chan.setSecret(false);
	else
		return (false);
	return (true);
}

bool	Server::modermode(Channel &chan, std::pair<std::string, std::string> mode)
{
	if (mode.first[0] == '+' && !chan.getModer())
		chan.setModer(true);
	else if (mode.first[0] == '-' && chan.getModer())
		chan.setModer(false);
	else
		return (false);
	return (true);
}

bool	Server::keymode(Channel &chan, std::pair<std::string, std::string> mode)
{
	if (mode.first[0] == '+' && mode.second.empty())
		return (false);
	else if (mode.first[0] == '+')
		chan.setKey(mode.second);
	else if (mode.first[0] == '-' && !mode.second.empty())
		chan.setKey("");
	else
		return (false);
	return (true);
}

bool	Server::invitemode(Channel &chan, std::pair<std::string, std::string> mode)
{	
	if (mode.first[0] == '+' && !chan.getInvite())
		chan.setInvite(true);
	else if (mode.first[0] == '-' && chan.getInvite())
		chan.setInvite(false);
	else
		return (false);
	return (true);
}

bool	Server::limitmode(Channel &chan, std::pair<std::string, std::string> mode)
{
	if (mode.first[0] == '+' && (mode.second.empty() || mode.second.size() > 2 || !isdigit(mode.second[0]) || (mode.second.size() == 2 && !isdigit(mode.second[1]))))
		return (false);
	else if (mode.first[0] == '+')
		chan.setLimit(atoi(mode.second.c_str()));
	else if (mode.first[0] == '-' && chan.getLimit())
		chan.setLimit(0);
	else
		return (false);
	return (true);
}

bool	Server::banmode(User &user, Channel &chan, std::pair<std::string, std::string> mode)
{
	if (mode.second.empty() && mode.first[0] == '+')
	{
		for (std::list<std::list<std::string> >::const_iterator it = chan.getBanlist().begin(); it != chan.getBanlist().end(); it++)
			sendservmsg(user, chan.getChannelName() + " " + *it->begin() + " " + *(++it->begin()) + " " + *(++(++it->begin())), RPL_BANLIST);
		sendservmsg(user, chan.getChannelName() + " " + RPL_ENDOFBANLIST_MSG, RPL_ENDOFBANLIST);
		return (false);
	}
	else if (mode.first[0] == '+')
	{
		std::list<std::string>	newban;
		std::stringstream		s;

		for (std::list<std::list<std::string> >::const_iterator it = chan.getBanlist().begin(); it != chan.getBanlist().end(); it++)
		{
			if (it->front() == mode.second)
				return (false);
		}
		newban.push_back(mode.second);
		newban.push_back(user.getNickName());
		s << time(NULL);
		newban.push_back(s.str());
		chan.getBanlist().push_back(newban);
		return (true);
	}
	else if (mode.first[0] == '-')
	{
		for (std::list<std::list<std::string> >::iterator it = chan.getBanlist().begin(); it != chan.getBanlist().end(); it++)
		{
			if (mode.second == it->front())
			{
				chan.getBanlist().erase(it);
				return (true);
			}
		}
	}
	return (false);
}

void	Server::execmodesUser(User &user, std::list<std::string> modes)
{
	std::string	add = "+";
	std::string	remove = "-";
	std::string	msg = fullname(user) + " MODE ";
	bool		done = false;

	for (std::list<std::string>::const_iterator it = modes.begin(); it != modes.end(); it++)
	{
		if ((*it)[1] == 'i')
			done = invismode(user, *it);
		else if ((*it)[1] == 'w')
			done = wallmode(user, *it);
		if (done)
		{
			if ((*it)[0] == '+')
				add += (*it)[1];
			else
				remove += (*it)[1];
		}
	}
	if (add != "+")
		msg += add;
	if (remove != "-")
		msg += remove;
	if (msg != fullname(user) + " MODE ")
		sendmsgto(user, msg);
}

void	Server::execmodesChannel(User &user, Channel &chan, std::list<std::pair<std::string, std::string> > modes)
{
	std::string	add = "+";
	std::string	remove = "-";
	std::string	argsadd, argsremove;
	std::string	msg = fullname(user) + " MODE ";
	bool		done = false;

	for (std::list<std::pair<std::string, std::string> >::const_iterator it = modes.begin(); it != modes.end(); it++)
	{
		if (it->first[1] == 'b')
			done = banmode(user, chan, *it);
		else if (it->first[1] == 'l')
			done = limitmode(chan, *it);
		else if (it->first[1] == 'i')
			done = invitemode(chan, *it);
		else if (it->first[1] == 'k')
			done = keymode(chan, *it);
		else if (it->first[1] == 'm')
			done = modermode(chan, *it);
		else if (it->first[1] == 's')
			done = secretmode(chan, *it);
		else if (it->first[1] == 't')
			done = protetopicmode(chan, *it);
		else if (it->first[1] == 'n')
			done = externalmode(chan, *it);
		else if (it->first[1] == 'o')
			done = operchanmode(chan, *it);
		else if (it->first[1] == 'v')
			done = voicechanmode(chan, *it);
		if (done)
		{
			if (it->first[0] == '+')
			{
				add += it->first[1];
				if (!it->second.empty())
				{
					if (!argsadd.empty())
						argsadd += " ";
					argsadd += it->second;
				}
			}
			else
			{
				remove += it->first[1];
				if (!it->second.empty())
				{
					if (!argsremove.empty())
						argsremove += " ";
					argsremove += it->second;
				}
			}
		}
	}
	if (!argsremove.empty() && !argsadd.empty())
		argsadd += " " + argsremove;
	else if (argsadd.empty() && !argsremove.empty())
		argsadd = argsremove;
	if (add != "+")
		msg += add;
	if (remove != "-")
		msg += remove;
	if (!argsadd.empty())
		msg += " " + argsadd;
	if (msg != fullname(user) + " MODE ")
	{
		sendToChannel(user, chan, msg);
		if (user.isinChannel(chan.getChannelName()))
			sendmsgto(user, msg);
	}
}

void	Server::modeuser(Parser const &result, User &user)
{
	std::string	modestring;

	if (result.getParam().size() > 1)
		modestring = result.getParam()[1];
	if (result.getParam().size() > 2)
		sendservmsg(user, result.getCommand() + " " + ERR_NEEDMOREPARAMS_MSG, ERR_NEEDMOREPARAMS);
	else if (!find_user(result.getParam()[0]))
		sendservmsg(user, result.getParam()[0] + " " + ERR_NOSUCHNICK_MSG, ERR_NOSUCHNICK);
	else if (result.getParam()[0] != user.getNickName())
		sendservmsg(user, ERR_USERSDONTMATCH_MSG, ERR_USERSDONTMATCH);
	else if (result.getParam().size() < 2 || result.getParam()[1].empty())
		sendservmsg(user, getmodesUser(user), RPL_UMODEIS);
	else if (!parsemodes(modestring, user, NULL))
		return ;
	else
	{
		std::list<std::string>	modes;
		std::string				temp;
		bool					add;

		for (std::string::const_iterator it = modestring.begin(); it != modestring.end(); it++)
		{
			temp = *it;
			if (temp == "+")
			{
				add = true;
				continue ;
			}
			else if (temp == "-")
			{
				add = false;
				continue ;
			}
			else if (add == true)
				modes.push_back("+" + temp);
			else
				modes.push_back("-" + temp);
		}
		execmodesUser(user, modes);
	}
	
}

void	Server::modechann(Parser const &result, User &user)
{
	Channel		*chan = find_channel(result.getParam()[0]);
	std::string	modestring;

	if (result.getParam().size() > 1)
		modestring = result.getParam()[1];
	if (!chan)
		sendservmsg(user, result.getParam()[0] + " " + ERR_NOSUCHCHANNEL_MSG, ERR_NOSUCHCHANNEL);
	else if (result.getParam().size() < 2 || result.getParam()[1].empty())
	{
		std::stringstream	s;

		s << chan->getTime();
		sendservmsg(user, chan->getChannelName() + getmodesChan(chan, &user), RPL_CHANNELMODEIS);
		sendservmsg(user, chan->getChannelName() + " " + s.str(), RPL_CREATIONTIME);
	}
	else if (!isoper(&user, chan))
		sendservmsg(user, chan->getChannelName() + " " + ERR_CHANOPRIVSNEEDED_MSG, ERR_CHANOPRIVSNEEDED);
	else if (!parsemodes(modestring, user, chan))
		return ;
	else
	{
		std::list<std::pair<std::string, std::string> >	modes;
		std::vector<std::string>						args = result.getParam();
		std::string										temp;
		bool											add;

		args.erase(args.begin(), args.begin() + 2);
		for (std::string::const_iterator it = modestring.begin(); it != modestring.end(); it++)
		{
			temp = *it;
			if (temp == "+")
			{
				add = true;
				continue ;
			}
			else if (temp == "-")
			{
				add = false;
				continue ;
			}
			else if (add == true)
				modes.push_back(std::make_pair("+" + temp, std::string()));
			else
				modes.push_back(std::make_pair("-" + temp, std::string()));
			addargument(modes.back(), args);
		}
		execmodesChannel(user, *chan, modes);
	}
}

void	Server::mode(Parser const &result, User &user)
{
	if (!result.getParam().size() || result.getParam()[0].empty())
		sendservmsg(user, result.getCommand() + " " + ERR_NEEDMOREPARAMS_MSG, ERR_NEEDMOREPARAMS);
	else if (result.getParam()[0][0] == '&' || result.getParam()[0][0] == '#')
		modechann(result, user);
	else
		modeuser(result, user);
}