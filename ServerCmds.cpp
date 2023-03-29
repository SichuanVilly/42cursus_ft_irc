/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerCmds.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jlopez-f <jlopez-f@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/17 18:39:49 by pvillena          #+#    #+#             */
/*   Updated: 2023/02/20 17:14:22 by jlopez-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/* Pass */

void	Server::pass(Parser const &result, User &user)
{
	if (result.getParam().size() != 1 || result.getParam()[0].empty())
		sendservmsg(user, result.getCommand() + " " + ERR_NEEDMOREPARAMS_MSG, ERR_NEEDMOREPARAMS);
	else if (_password.empty() || _password == result.getParam()[0])
		user.setPass(true);
	else
		sendservmsg(user, ERR_PASSWDMISMATCH_MSG, ERR_PASSWDMISMATCH);
}

/* Nick */

static bool	parsenick(std::string const&nick)
{
	std::string	forbchar = FORBNICKCHAR;
	std::string	forbstart = FORBSTARTNICKCHAR;

	if (nick.empty() || std::find(forbstart.begin(), forbstart.end(), nick[0]) != forbstart.end())
		return (false);
	else
	{
		for (std::string::const_iterator it = nick.begin(); it != nick.end(); it++)
		{
			if (std::find(forbchar.begin(), forbchar.end(), *it) != forbchar.end())
				return (false);
		}
	}
	return (true);
}

void	Server::nick(Parser const &result, User &user)
{
	if (result.getParam().size() != 1 || result.getParam()[0].empty())
		sendservmsg(user, result.getCommand() + " " + ERR_NEEDMOREPARAMS_MSG, ERR_NEEDMOREPARAMS);
	else if (find_user(result.getParam()[0]))
		sendservmsg(user, result.getParam()[0] + " " + ERR_NICKNAMEINUSE_MSG, ERR_NICKNAMEINUSE);
	else if (!parsenick(result.getParam()[0]))
		sendservmsg(user, result.getParam()[0] + " " + ERR_ERRONEUSNICKNAME_MSG, ERR_ERRONEUSNICKNAME);
	else
	{
		std::string	temp = user.getNickName();
		std::string	nickname = result.getParam()[0];

		if (nickname.size() > NICKLEN)
			nickname.erase(NICKLEN, std::string::npos);
		user.setNickName(nickname);
		if (!temp.empty())
		{
			std::string	msg;

			msg = ":" + temp + "!" + user.getUserName() + "@" + user.getHost() + " NICK " +  nickname;
			sendmsgto(user, msg);
			sendToAll(user, msg);
		}
	}
}

/* User */

void	Server::usercmd(Parser const &result, User &user)
{
	if (result.getParam().size() != 4 || result.getParam()[0].empty() || result.getParam()[3].empty())
		sendservmsg(user, result.getCommand() + " " + ERR_NEEDMOREPARAMS_MSG, ERR_NEEDMOREPARAMS);
	else
	{
		std::string	username = result.getParam()[0];

		if (username.size() > USERLEN)
			username.erase(USERLEN, std::string::npos);
		user.setUserName(result.getParam()[0]);
		username = result.getParam()[3];
		if (username.size() > USERLEN)
			username.erase(USERLEN, std::string::npos);
		user.setRealName(result.getParam()[3]);
	}
}

/* Ping */

void	Server::ping(Parser const &result, User &user)
{	
	if (result.getParam().size() != 1 || result.getParam()[0].empty())
		sendservmsg(user, result.getCommand() + " " + ERR_NEEDMOREPARAMS_MSG, ERR_NEEDMOREPARAMS);
	else
	{
		std::string	temp;

		temp = ":" + _ip + " PONG WeabooSegfaultIRC " + result.getParamSend(0);
		sendmsgto(user, temp);
	}
}

/* Oper */

void	Server::oper(Parser const &result, User &user)
{	
	if (result.getParam().size() != 2 || result.getParam()[0].empty() || result.getParam()[1].empty())
		sendservmsg(user, result.getCommand() + " " + ERR_NEEDMOREPARAMS_MSG, ERR_NEEDMOREPARAMS);
	else if (result.getParam()[0] == OPER_NAME && result.getParam()[1] == OPER_PASS) 
	{
		std::string	temp;

		user.setOper(true);
		sendservmsg(user, RPL_YOUREOPER_MSG, RPL_YOUREOPER);
		temp = ":" + _ip + " MODE " + user.getNickName() + " +o";
		sendmsgto(user, temp);
	}
	else
		sendservmsg(user, ERR_PASSWDMISMATCH_MSG, ERR_PASSWDMISMATCH);
}

/* Quit */

void	Server::quit(Parser const &result, User &user)
{	
	std::string	temp;

	if (result.getParam().size() > 1)
	{
		sendservmsg(user, result.getCommand() + " " + ERR_NEEDMOREPARAMS_MSG, ERR_NEEDMOREPARAMS);
		return ;
	}
	temp = ":" + _ip + " ERROR :Disconnected";
	sendmsgto(user, temp);
	temp = fullname(user) + " QUIT :Quit: ";
	if (result.getParam().size())
		temp += result.getParam()[0];
	sendToAll(user, temp);
	std::cout << RED << temp << NOCOLOR << std::endl;
	deleteUser(&user);
}

/* Join */

void	Server::channelJoinInfo(User &user, Channel &channel)
{
	std::string s = fullname(user) + " JOIN " + channel.getChannelName();

	sendmsgto(user, s);
	sendToChannel(user, channel, s);
	if (!channel.getTopic().empty())
	{
		sendservmsg(user, channel.getChannelName() + " :" + channel.getTopic(), RPL_TOPIC);
		sendservmsg(user, channel.getChannelName() + " " + channel.getTopicWho() + " " + channel.getTopicTime(), RPL_TOPICWHOTIME);
	}
	if (channel.getSecret())
		s = "@ " + channel.getChannelName() + " :";
	else
		s = "= " + channel.getChannelName() + " :";
	for (std::map<User *, std::string>::const_iterator iter = channel.getUsersInChannel().begin(); iter != channel.getUsersInChannel().end(); iter++)
	{
		s += iter->second + iter->first->getNickName();
		if (iter != channel.getUsersInChannel().end())
			s += " ";
	}
	sendservmsg(user, s, RPL_NAMREPLY);
	sendservmsg(user, channel.getChannelName() + " " + RPL_ENDOFNAMES_MSG, RPL_ENDOFNAMES);
}

void	Server::join_to_channel(Channel *chan, User *user, std::list<std::string>::iterator ikeys, bool is_end)
{
	if (user->isinChannel(chan->getChannelName()))
		return ;
	for (std::list<std::list<std::string> >::const_iterator it = chan->getBanlist().begin(); it != chan->getBanlist().end(); it++)
	{
		if (strtoupper(it->front()) == strtoupper(user->getNickName()))
		{
			sendservmsg(*user, chan->getChannelName() + " " + ERR_BANNEDFROMCHAN_MSG, ERR_BANNEDFROMCHAN);
			return ;
		}
	}
	if (chan->getLimit() && chan->getLimit() == chan->getUsersInChannel().size() + 1)
		sendservmsg(*user, chan->getChannelName() + " " + ERR_CHANNELISFULL_MSG, ERR_CHANNELISFULL);
	else if (chan->getInvite())
	{
		for (std::list<std::string>::const_iterator it = chan->getInvitedlist().begin(); it != chan->getInvitedlist().end(); it++)
		{
			if (strtoupper(*it) == strtoupper(user->getNickName()))
			{
				chan->addUser(*user, "");
				this->channelJoinInfo(*user, *chan);
				return ;
			}
		}
		sendservmsg(*user, chan->getChannelName() + " " + ERR_INVITEONLYCHAN_MSG, ERR_INVITEONLYCHAN);
		return ;
	}
	else if (chan->getKey().length() != 0 && (is_end == true || *ikeys != chan->getKey()))
	{
		sendservmsg(*user, chan->getChannelName() + " " + ERR_BADCHANNELKEY_MSG, ERR_BADCHANNELKEY);
		return ;
	}
	else if (user->getJoinChann() >= CHANLIMIT)
		sendservmsg(*user, chan->getChannelName() + " " + ERR_TOOMANYCHANNELS_MSG, ERR_TOOMANYCHANNELS);
	else
	{
		chan->addUser(*user, "");
		this->channelJoinInfo(*user, *chan);
	}
}

void	Server::create_channel(std::string name, User *user)
{
	if (user->getJoinChann() >= CHANLIMIT)
	{
		sendservmsg(*user, name + " " + ERR_TOOMANYCHANNELS_MSG, ERR_TOOMANYCHANNELS);
		return ;
	}
	Channel * newChan = newChannel(name, *user);

	_all_channels.push_back(newChan);
	this->channelJoinInfo(*user, *newChan);
}

Channel	*Server::find_channel(std::string const &channel)
{
	for (std::list<Channel *>::iterator iter = _all_channels.begin(); iter != _all_channels.end(); iter++)
	{
		if ((*iter)->getChannelName() == channel)
			return (*iter);
	}
	return (NULL);
}

void	Server::leaveallChannels(User &user)
{
	for (std::list<Channel *>::const_iterator it = user.getChannels().begin(); it != user.getChannels().end(); )
	{
		sendmsgto(user, fullname(user) + " PART " + (*it)->getChannelName());
		sendToChannel(user, **it, fullname(user) + " PART " + (*it)->getChannelName());
		(*it)->removeUser(user);
		deleteChannels();
		it = user.getChannels().begin();
		if (!user.getJoinChann())
			return ;
	}
}

void	Server::join(Parser const &result, User &user)
{
	std::list<std::string>	channels;
	std::list<std::string>	keys;

	if (result.getParam().size() < 1 || result.getParam().size() > 2 || result.getParam()[0].empty())
	{
		sendservmsg(user, result.getCommand() + " " + ERR_NEEDMOREPARAMS_MSG, ERR_NEEDMOREPARAMS);
		return ;
	}
	if (result.getParam()[0] == "0")
	{
		leaveallChannels(user);
		return ;
	}
	channels = adv_tokenizer(result.getParam()[0], ',');
	if (result.getParam().size() == 2 && !result.getParam()[1].empty())
		keys = adv_tokenizer(result.getParam()[1], ',');

	std::list<std::string>::iterator	ichan, ikeys;
	std::list<Channel *>::iterator		iChannel;
	Channel								*chan;

	iChannel = _all_channels.begin();
	ikeys = keys.begin();
	for (ichan = channels.begin(); ichan != channels.end(); ichan++)
	{
		if ((*ichan).empty() || ((*ichan)[0] != '#' && (*ichan)[0] != '&'))
		{
			sendservmsg(user, ERR_BADCHANMASK_MSG, ERR_BADCHANMASK);
			return ;
		}
		chan = find_channel(*ichan);
		if (chan)
			join_to_channel(chan, &user, ikeys, keys.end() == ikeys);
		else
			create_channel(*ichan, &user);
		if (!keys.empty() && ikeys != keys.end())
			ikeys++;
	}
}

/* Part */

void	Server::part(Parser const &result, User &user)
{
	std::list<std::string>	channels;
	std::string				msg = fullname(user) + " PART ";
	std::string				reason;

	if (!result.getParam().size() || result.getParam().size() > 2 || result.getParam()[0].empty())
	{
		sendservmsg(user, result.getCommand() + " " + ERR_NEEDMOREPARAMS_MSG, ERR_NEEDMOREPARAMS);
		return ;
	}
	if (result.getParam().size() == 2 && !result.getParam()[1].empty())
		reason = " " + result.getParam()[1];
	channels = adv_tokenizer(result.getParam()[0], ',');
	for (std::list<std::string>::iterator iter = channels.begin(); iter != channels.end(); iter++)
	{
		if (!find_channel(*iter))
			sendservmsg(user, *iter + " " + ERR_NOSUCHCHANNEL_MSG, ERR_NOSUCHCHANNEL);
		else
		{
			bool	deleted = false;

			for (std::list<Channel *>::const_iterator it2 = user.getChannels().begin(); it2 != user.getChannels().end(); it2++)
			{
				if ((*it2)->getChannelName() == *iter)
				{
					sendmsgto(user, msg + *iter + reason);
					sendToChannel(user, **it2, msg + *iter + reason);
					(*it2)->removeUser(user);
					deleteChannels();
					deleted = true;
					break ;
				}
			}
			if (!deleted)
				sendservmsg(user, *iter + " " + ERR_NOTONCHANNEL_MSG, ERR_NOTONCHANNEL);
		}
	}
}

/* Topic */

void	Server::topic(Parser const &result, User &user)
{
	Channel	*chan;

	if (!result.getParam().size() || result.getParam().size() > 2 || result.getParam()[0].empty())
	{
		sendservmsg(user, result.getCommand() + " " + ERR_NEEDMOREPARAMS_MSG, ERR_NEEDMOREPARAMS);
		return ;
	}
	chan = find_channel(result.getParam()[0]);
	if (chan)
	{
		if (!user.isinChannel(result.getParam()[0]))
		{
			sendservmsg(user, result.getParam()[0] + " " + ERR_NOTONCHANNEL_MSG, ERR_NOTONCHANNEL);
			return ;
		}
		if (result.getParam().size() == 1)
		{
			if (chan->getTopic().empty())
				sendservmsg(user, result.getParam()[0] + " " + RPL_NOTOPIC_MSG, RPL_NOTOPIC);
			else
			{
				sendservmsg(user, result.getParam()[0] + " :" + chan->getTopic(), RPL_TOPIC);
				sendservmsg(user, result.getParam()[0] + " " + chan->getTopicWho() + " " + chan->getTopicTime(), RPL_TOPICWHOTIME);
			}
			return ;
		}
		if (chan->getTopicprot() && !isoper(&user, chan))
		{
			sendservmsg(user, result.getParam()[0] + " " + ERR_CHANOPRIVSNEEDED_MSG, ERR_CHANOPRIVSNEEDED);
			return ;
		}
		std::string	topicstr = result.getParam()[1];

		if (topicstr.size() > TOPICLEN)
			topicstr.erase(TOPICLEN, std::string::npos);
		chan->setTopic(topicstr);
		chan->setTopicTime(gettime());
		sendmsgto(user, fullname(user) + " TOPIC " + result.getParam()[0] + " " + topicstr);
		sendToChannel(user, *chan, fullname(user) + " TOPIC " + result.getParam()[0] + " " + topicstr);
	}
	else
		sendservmsg(user, result.getParam()[0] + " " + ERR_NOSUCHCHANNEL_MSG, ERR_NOSUCHCHANNEL);
}

/* Names */

void	Server::names(Parser const &result, User &user)
{
	std::list<std::string>	channels;
	Channel					*temp;
	std::string				s;

	if (result.getParam().size() != 1 || result.getParam()[0].empty())
	{
		sendservmsg(user, result.getCommand() + " " + ERR_NEEDMOREPARAMS_MSG, ERR_NEEDMOREPARAMS);
		return ;
	}
	channels = adv_tokenizer(result.getParam()[0], ',');
	for (std::list<std::string>::iterator iter = channels.begin(); iter != channels.end(); iter++)
	{
		temp = find_channel(*iter);
		if (!temp || (temp->getSecret() && !user.isinChannel(*iter)))
		{
			sendservmsg(user, *iter + " " + RPL_ENDOFNAMES_MSG, RPL_ENDOFNAMES);
			continue ;
		}
		if (temp->getSecret())
			s = "@ " + temp->getChannelName() + " :";
		else
			s = "= " + temp->getChannelName() + " :";
		for (std::map<User *, std::string>::const_iterator it2 = temp->getUsersInChannel().begin(); it2 != temp->getUsersInChannel().end(); it2++)
		{
			if (!it2->first->getInvis() || (it2->first->getInvis() && it2->first->shareChannel(user)))
				s += it2->second + it2->first->getNickName();
			if (it2 != temp->getUsersInChannel().end())
				s += " ";
		}
		sendservmsg(user, s, RPL_NAMREPLY);
		sendservmsg(user, temp->getChannelName() + " " + RPL_ENDOFNAMES_MSG, RPL_ENDOFNAMES);
	}
}

/* List */

void	Server::list(Parser const &result, User &user)
{
	std::list<std::string>	channels;

	if (result.getParam().size() > 1)
	{
		sendservmsg(user, result.getCommand() + " " + ERR_NEEDMOREPARAMS_MSG, ERR_NEEDMOREPARAMS);
		return ;
	}
	sendservmsg(user, RPL_LISTSTART_MSG, RPL_LISTSTART);
	if (result.getParam().size() && !result.getParam()[0].empty())
	{
		channels = adv_tokenizer(result.getParam()[0], ',');
		for (std::list<std::string>::const_iterator it = channels.begin(); it != channels.end(); it++)
		{
			std::stringstream	s;
			Channel				*temp = find_channel(*it);

			if (!temp || (temp->getSecret() && !user.isinChannel(*it)))
				continue ;
			s << temp->getUsersInChannel().size();
			sendservmsg(user, temp->getChannelName() + " " + s.str() + " :" + temp->getTopic(), RPL_LIST);
		}
	}
	else
	{
		for (std::list<Channel *>::const_iterator it = _all_channels.begin(); it != _all_channels.end(); it++)
		{
			std::stringstream	s;

			if ((*it)->getSecret() && !user.isinChannel((*it)->getChannelName()))
				continue ;
			s << (*it)->getUsersInChannel().size();
			sendservmsg(user, (*it)->getChannelName() + " " + s.str() + " :" + (*it)->getTopic(), RPL_LIST);
		}
	}
	sendservmsg(user, RPL_LISTEND_MSG, RPL_LISTEND);
}

/* Invite */

void	Server::invite(Parser const &result, User &user)
{
	User	*inv;
	Channel	*chan;

	if (result.getParam().size() != 2 || result.getParam()[0].empty() || result.getParam()[1].empty())
	{
		sendservmsg(user, result.getCommand() + " " + ERR_NEEDMOREPARAMS_MSG, ERR_NEEDMOREPARAMS);
		return ;
	}
	inv = find_user(result.getParam()[0]);
	chan = find_channel(result.getParam()[1]);
	if (!inv)
		sendservmsg(user, result.getParam()[0] + " " + ERR_NOSUCHNICK_MSG, ERR_NOSUCHNICK);
	else if (!chan)
		sendservmsg(user, result.getParam()[1] + " " + ERR_NOSUCHCHANNEL_MSG, ERR_NOSUCHCHANNEL);
	else if (!user.isinChannel(result.getParam()[1]))
		sendservmsg(user, result.getParam()[1] + " " + ERR_NOTONCHANNEL_MSG, ERR_NOTONCHANNEL);
	else if (chan->getInvite() && !isoper(&user, chan))
		sendservmsg(user, result.getParam()[1] + " " + ERR_CHANOPRIVSNEEDED_MSG, ERR_CHANOPRIVSNEEDED);
	else if (inv->isinChannel(result.getParam()[1]))
		sendservmsg(user, result.getParam()[0] + " " + result.getParam()[1] + " " + ERR_USERONCHANNEL_MSG, ERR_USERONCHANNEL);
	else
	{
		chan->getInvitedlist().push_back(inv->getNickName());
		sendservmsg(user, result.getParam()[0] + " " + result.getParam()[1], RPL_INVITING);
		sendmsgto(*inv, fullname(user) + " INVITE " + result.getParam()[0] + " " + result.getParam()[1]);
	}
}

/* Kick */

void	Server::kick(Parser const &result, User &user)
{
	std::list<std::string>	kicklist;
	Channel					*chan;

	if (result.getParam().size() < 2 || result.getParam().size() > 3 || result.getParam()[0].empty() || result.getParam()[1].empty())
	{
		sendservmsg(user, result.getCommand() + " " + ERR_NEEDMOREPARAMS_MSG, ERR_NEEDMOREPARAMS);
		return ;
	}
	chan = find_channel(result.getParam()[0]);
	if (!chan)
	{
		sendservmsg(user, result.getParam()[0] + " " + ERR_NOSUCHCHANNEL_MSG, ERR_NOSUCHCHANNEL);
		return ;
	}
	kicklist = adv_tokenizer(result.getParam()[1], ',');
	for (std::list<std::string>::const_iterator it = kicklist.begin(); it != kicklist.end(); it++)
	{
		User	*kick = find_user(*it);

		if (!kick)
			sendservmsg(user, *it + " " + ERR_NOSUCHNICK_MSG, ERR_NOSUCHNICK);
		else if (!kick->isinChannel(chan->getChannelName()))
			sendservmsg(user, *it + " " + chan->getChannelName() + " " + ERR_USERNOTINCHANNEL_MSG, ERR_USERNOTINCHANNEL);
		else if (!isoper(&user, chan))
			sendservmsg(user, chan->getChannelName() + " " + ERR_CHANOPRIVSNEEDED_MSG, ERR_CHANOPRIVSNEEDED);
		else
		{
			std::string	msg = fullname(user) + " KICK " + chan->getChannelName() + " " + *it;
			std::string	kickmsg;
			
			if (result.getParam().size() == 3)
				kickmsg = result.getParam()[2];
			if (kickmsg.size() > KICKLEN)
				kickmsg.erase(KICKLEN, std::string::npos);
			if (!kickmsg.empty())
				msg += " :" + kickmsg;
			sendmsgto(*kick, msg);
			sendToChannel(*kick, *chan, msg);
			chan->removeUser(*kick);
			deleteChannels();
		}
	}
}

/* MOTD */

void	Server::motd(Parser const &result, User &user)
{
	if (result.getParam().size())
	{
		sendservmsg(user, result.getCommand() + " " + ERR_NEEDMOREPARAMS_MSG, ERR_NEEDMOREPARAMS);
		return ;
	}
	sendservmsg(user, RPL_MOTDSTART_MSG, RPL_MOTDSTART);
	sendmotd(user);
	sendservmsg(user, RPL_ENDOFMOTD_MSG, RPL_ENDOFMOTD);
}

/* Version */

void	Server::version(Parser const &result, User &user)
{
	if (result.getParam().size())
	{
		sendservmsg(user, result.getCommand() + " " + ERR_NEEDMOREPARAMS_MSG, ERR_NEEDMOREPARAMS);
		return ;
	}
	sendservmsg(user, RPL_VERSION_MSG, RPL_VERSION);
	sendservmsg(user, RPL_ISUPPORT_MSG, RPL_ISUPPORT);
}

/* Admin */

void	Server::admin(Parser const &result, User &user)
{
	if (result.getParam().size())
	{
		sendservmsg(user, result.getCommand() + " " + ERR_NEEDMOREPARAMS_MSG, ERR_NEEDMOREPARAMS);
		return ;
	}
	sendservmsg(user, RPL_ADMINME_MSG, RPL_ADMINME);
	sendservmsg(user, RPL_ADMINLOC1_MSG, RPL_ADMINLOC1);
	sendservmsg(user, RPL_ADMINLOC2_MSG, RPL_ADMINLOC2);
	sendservmsg(user, RPL_ADMINEMAIL_MSG, RPL_ADMINEMAIL);
}

/* Lusers */

void	Server::lusers(Parser const &result, User &user)
{
	int					u = _all_users.size();
	int					channels = _all_channels.size();
	int					i = 0, ops = 0;
	std::stringstream	ustring, channelsstring, istring, opsstring;

	if (result.getParam().size())
	{
		sendservmsg(user, result.getCommand() + " " + ERR_NEEDMOREPARAMS_MSG, ERR_NEEDMOREPARAMS);
		return ;
	}
	for (std::vector<User *>::const_iterator it = _all_users.begin(); it != _all_users.end(); it++)
	{
		if ((*it)->getInvis())
			i++;
		if ((*it)->getOper())
			ops++;
	}
	ustring << u;
	channelsstring << channels;
	istring << i;
	opsstring << ops;
	sendservmsg(user, RPL_LUSERCLIENT_MSG1 + ustring.str() + RPL_LUSERCLIENT_MSG2 + istring.str() + RPL_LUSERCLIENT_MSG3, RPL_LUSERCLIENT);
	sendservmsg(user, opsstring.str() + " " + RPL_LUSEROP_MSG, RPL_LUSEROP);
	sendservmsg(user, channelsstring.str() + " " + RPL_LUSERCHANNELS_MSG, RPL_LUSERCHANNELS);
	sendservmsg(user, RPL_LUSERME_MSG1 + ustring.str() + RPL_LUSERME_MSG2, RPL_LUSERME);
}

/* Time */

void	Server::timecmd(Parser const &result, User &user)
{
	if (result.getParam().size())
	{
		sendservmsg(user, result.getCommand() + " " + ERR_NEEDMOREPARAMS_MSG, ERR_NEEDMOREPARAMS);
		return ;
	}
	sendservmsg(user, RPL_TIME_MSG + _date, RPL_TIME);
}

/* Info */

void	Server::info(Parser const &result, User &user)
{
	if (result.getParam().size())
	{
		sendservmsg(user, result.getCommand() + " " + ERR_NEEDMOREPARAMS_MSG, ERR_NEEDMOREPARAMS);
		return ;
	}
	sendservmsg(user, RPL_INFO_MSG, RPL_INFO);
}

/* Privmsg */

void Server::privmsg(const Parser &result, User &user)
{
	std::list<std::string>	whereToSend;
	std::string				s = fullname(user) + " PRIVMSG ";

	if (result.getParam().size() != 2 || result.getParam()[0].empty() || result.getParam()[1].empty())
	{
		sendservmsg(user, result.getCommand() + " " + ERR_NEEDMOREPARAMS_MSG, ERR_NEEDMOREPARAMS);
		return ;
	}
	whereToSend = adv_tokenizer(result.getParam()[0], ',');
	for (std::list<std::string>::iterator iter = whereToSend.begin(); iter != whereToSend.end(); iter++)
	{
		if ((*iter)[0] == '&' || (*iter)[0] == '#')
		{
			Channel	*chan = find_channel(*iter);

			if (chan)
			{
				if ((chan->getExternal() && (!user.getOper() && !user.isinChannel(chan->getChannelName()))) ||
				(chan->getModer() && (!user.getOper() && (!user.isinChannel(chan->getChannelName()) || (user.isinChannel(chan->getChannelName()) && chan->getUsersInChannel().find(&user)->second.empty())))))
					sendservmsg(user, *iter + " " + ERR_CANNOTSENDTOCHAN_MSG, ERR_CANNOTSENDTOCHAN);
				else
					sendToChannel(user, *chan, s + *iter + " :" + result.getParam()[1]);
			}
			else
				sendservmsg(user, *iter + " " + ERR_NOSUCHCHANNEL_MSG, ERR_NOSUCHCHANNEL);
		}
		else if (find_user(*iter))
		{
			sendmsgto(*find_user(*iter), s + *iter + " :" + result.getParam()[1]);
			if (!find_user(*iter)->getAway().empty())
				sendservmsg(user, find_user(*iter)->getNickName() + " :" + find_user(*iter)->getAway(), RPL_AWAY);
		}
		else
			sendservmsg(user, *iter + " " + ERR_NOSUCHNICK_MSG, ERR_NOSUCHNICK);
	}
}

/* Notice */

void	Server::notice(Parser const &result, User &user)
{
	std::list<std::string>	whereToSend;
	std::string				s = fullname(user) + " NOTICE ";

	if (result.getParam().size() != 2 || result.getParam()[0].empty() || result.getParam()[1].empty())
		return ;
	whereToSend = adv_tokenizer(result.getParam()[0], ',');
	for (std::list<std::string>::iterator iter = whereToSend.begin(); iter != whereToSend.end(); iter++)
	{
		if (((*iter)[0] == '&' || (*iter)[0] == '#'))
		{
			Channel	*chan = find_channel(*iter);

			if (chan)
			{
				if ((chan->getExternal() && (!user.getOper() && !user.isinChannel(chan->getChannelName()))) ||
				(chan->getModer() && (!user.getOper() && (!user.isinChannel(chan->getChannelName()) || (user.isinChannel(chan->getChannelName()) && chan->getUsersInChannel().find(&user)->second.empty())))))
					continue ;
				else
					sendToChannel(user, *chan, s + *iter + " " + result.getParam()[1]);
			}
		}
		else if (find_user(*iter))
			sendmsgto(*find_user(*iter), s + *iter + " " + result.getParam()[1]);
	}
}

/* Who */

void	Server::whouser(std::pair<User *, std::string> user, User &whoask, Channel *chan)
{
	std::string	msg;

	if (user.first->getInvis() && !whoask.shareChannel(*user.first) && whoask.getNickName() != user.first->getNickName())
		return ;
	if (chan)
		msg += chan->getChannelName() + " ";
	else
		msg += "* ";
	msg += user.first->getUserName() + " ";
	if (user.first->getHost()[0] == ':')
		msg += "0";
	msg += user.first->getHost() + " WeabooSegfaultIRC " + user.first->getNickName() + " ";
	if (!user.first->getAway().empty())
		msg += "G";
	else
		msg += "H";
	if (user.first->getOper())
		msg += "*";
	if (chan)
		msg += user.second;
	if (user.first->getInvis())
		msg += "i";
	if (user.first->getWall())
		msg += "w";
	msg += " :0 " + user.first->getRealName();
	sendservmsg(whoask, msg, RPL_WHOREPLY);
}

void	Server::who(Parser const &result, User &user)
{
	if (result.getParam().size() != 1 || result.getParam()[0].empty())
	{
		sendservmsg(user, result.getCommand() + " " + ERR_NEEDMOREPARAMS_MSG, ERR_NEEDMOREPARAMS);
		return ;
	}
	if (result.getParam()[0][0] == '&' || result.getParam()[0][0] == '#')
	{
		Channel	*chan = find_channel(result.getParam()[0]);
		if (chan)
		{
			for (std::map<User *, std::string>::const_iterator it = chan->getUsersInChannel().begin(); it != chan->getUsersInChannel().end(); it++)
				whouser(*it, user, chan);
		}
	}
	else if (find_user(result.getParam()[0]))
	{
		User	*tempuser = find_user(result.getParam()[0]);

		if (tempuser->getChannels().size())
			whouser(*(tempuser->getChannels().front()->getUsersInChannel().find(tempuser)), user, tempuser->getChannels().front());
		else
			whouser(std::make_pair(tempuser, ""), user, NULL);
	}
	sendservmsg(user, result.getParam()[0] + " " + RPL_ENDOFWHO_MSG, RPL_ENDOFWHO);
}

/* Whois */

void	Server::whoischannel(User &user, User &targetuser)
{
	std::string	msg;

	for (std::list<Channel *>::const_iterator it = targetuser.getChannels().begin(); it != targetuser.getChannels().end(); it++)
	{
		if (((*it)->getSecret() || targetuser.getInvis()) && !user.isinChannel((*it)->getChannelName()))
			continue ;
		if (!msg.empty())
			msg += " ";
		msg += (*it)->getUsersInChannel().find(&targetuser)->second + (*it)->getChannelName();
	}
	if (!msg.empty())
		sendservmsg(user, targetuser.getNickName() + " :" + msg, RPL_WHOISCHANNELS);
}

void	Server::whois(Parser const &result, User &user)
{
	User				*targetuser;
	std::string			msg;
	std::stringstream	s;

	if (result.getParam().size() != 1 || result.getParam()[0].empty())
	{
		sendservmsg(user, result.getCommand() + " " + ERR_NEEDMOREPARAMS_MSG, ERR_NEEDMOREPARAMS);
		return ;
	}
	targetuser = find_user(result.getParam()[0]);
	if (!targetuser)
	{
		sendservmsg(user, result.getParam()[0] + " " + ERR_NOSUCHNICK_MSG, ERR_NOSUCHNICK);
		return ;
	}
	msg = targetuser->getNickName() + " " + targetuser->getUserName() + " ";
	if (targetuser->getHost()[0] == ':')
		msg += "0";
	msg += targetuser->getHost() + " * :" + targetuser->getUserName();
	sendservmsg(user, msg, RPL_WHOISUSER);
	if (targetuser->getOper())
		sendservmsg(user, targetuser->getNickName() + " " + RPL_WHOISOPERATOR_MSG, RPL_WHOISOPERATOR);
	s << std::difftime(time(NULL), targetuser->getLastTime());
	s << " ";
	s << targetuser->getConnectTime();
	sendservmsg(user, targetuser->getNickName() + " " + s.str() + " " + RPL_WHOISIDLE_MSG, RPL_WHOISIDLE);
	whoischannel(user, *targetuser);
	if (!getmodesUser(*targetuser).empty())
	{
		msg = targetuser->getNickName() + " " RPL_WHOISMODES_MSG + " " + getmodesUser(*targetuser);
		sendservmsg(user, msg, RPL_WHOISMODES);
	}
	if (!targetuser->getAway().empty())
		sendservmsg(user, targetuser->getNickName() + " :" + targetuser->getAway(), RPL_AWAY);
	sendservmsg(user, targetuser->getNickName() + " " + RPL_ENDOFWHOIS_MSG, RPL_ENDOFWHOIS);
}

/* Kill */

void	Server::kill(Parser const &result, User &user)
{
	User	*killed;

	if (result.getParam().size() < 1 || result.getParam().size() > 2 || result.getParam()[0].empty())
	{
		sendservmsg(user, result.getCommand() + " " + ERR_NEEDMOREPARAMS_MSG, ERR_NEEDMOREPARAMS);
		return ;
	}
	if (!user.getOper())
	{
		sendservmsg(user, ERR_NOPRIVILEGES_MSG, ERR_NOPRIVILEGES);
		return ;
	}
	killed = find_user(result.getParam()[0]);
	if (!killed)
	{
		sendservmsg(user, result.getParam()[0] + " " + ERR_NOSUCHNICK_MSG, ERR_NOSUCHNICK);
		return ;
	}
	else
	{
		std::string	msg;

		msg = fullname(user) + " KILL " + result.getParam()[0];
		if (result.getParam().size() == 2)
			msg += " :" + result.getParam()[1];
		sendmsgto(*killed, msg);
		msg = fullname(*killed) + " QUIT :Quit: Killed (" + user.getNickName();
		if (result.getParam().size() == 2)
			msg += " (" + result.getParam()[1] + "))";
		else
			msg += ")";
		sendmsgto(*killed, msg);
		sendToAll(*killed, msg);
		msg = ":" + _ip + " ERROR Closing Link: WeabooSegfaultIRC (Killed (" + user.getNickName();
		if (result.getParam().size() == 2)
			msg += " (" + result.getParam()[1] + ")))";
		else
			msg += "))";
		sendmsgto(*killed, msg);
		deleteUser(killed);
	}
}

/* Away */

void	Server::away(Parser const &result, User &user)
{
	if (result.getParam().size() > 1)
	{
		sendservmsg(user, result.getCommand() + " " + ERR_NEEDMOREPARAMS_MSG, ERR_NEEDMOREPARAMS);
		return ;
	}
	else if (result.getParam().size() == 1 && !result.getParam()[0].empty())
	{
		std::string	temp = result.getParam()[0];

		if (temp.size() > AWAYLEN)
			temp.erase(AWAYLEN, std::string::npos);
		user.setAway(temp);
		sendservmsg(user, RPL_NOWAWAY_MSG, RPL_NOWAWAY);
	}
	else
	{
		user.setAway("");
		sendservmsg(user, RPL_UNAWAY_MSG, RPL_UNAWAY);
	}
}

/* Wallops */

void	Server::wallops(Parser const &result, User &user)
{
	if (result.getParam().size() != 1 && result.getParam()[0].empty())
	{
		sendservmsg(user, result.getCommand() + " " + ERR_NEEDMOREPARAMS_MSG, ERR_NEEDMOREPARAMS);
		return ;
	}
	if (!user.getOper())
	{
		sendservmsg(user, ERR_NOPRIVILEGES_MSG, ERR_NOPRIVILEGES);
		return ;
	}
	for (std::vector<User *>::const_iterator it = _all_users.begin(); it != _all_users.end(); it++)
	{
		if ((*it)->getWall())
			sendmsgto(**it, fullname(user) + " WALLOPS " + result.getParamSend(0));
	}
}
