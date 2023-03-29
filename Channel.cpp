/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jlopez-f <jlopez-f@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/24 18:36:14 by pvillena          #+#    #+#             */
/*   Updated: 2023/02/14 17:33:13 by jlopez-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

/* Constructors */

Channel::Channel() : _channelName(), _key(), _topic(), _topicwho(), _topictime(), _invite(false), _moder(false),
_secret(false), _topicprot(false), _external(false), _time(time(NULL)), _limit(0), _banlist(), _invitedlist(), _usersInChannel() {}

Channel::Channel(std::string name) : _channelName(name), _key(), _topic(), _topicwho(), _topictime(), _invite(false),
_moder(false), _secret(false), _topicprot(false), _external(false), _time(time(NULL)), _limit(0), _banlist(), _invitedlist(), _usersInChannel() {}

Channel::Channel(const Channel &other) { *this = other; }

/* Destructor */

Channel::~Channel() {}

/* Operator assignation */

Channel	&Channel::operator=(const Channel &other)
{
	if (this != &other)
	{
		_channelName = other._channelName;
		_key = other._key;
		_topic = other._topic;
		_topicwho = other._topicwho;
		_topictime = other._topictime;
		_invite = other._invite;
		_moder = other._moder;
		_secret = other._secret;
		_topicprot = other._topicprot;
		_external = other._external;
		_time = other._time;
		_limit = other._limit;
		_banlist = other._banlist;
		_invitedlist = other._invitedlist;
		_usersInChannel = other._usersInChannel;
	}
	return (*this);
}

/* Element access */

std::string const	&Channel::getChannelName(void) const { return (_channelName); }

std::string const	&Channel::getKey(void) const { return (_key); }

std::string const	&Channel::getTopic(void) const { return (_topic); }

std::string const	&Channel::getTopicWho(void) const { return (_topicwho); }

std::string const	&Channel::getTopicTime(void) const { return (_topictime); }

bool const	&Channel::getInvite(void) const { return (_invite); }

bool const	&Channel::getModer(void) const { return (_moder); }

bool const	&Channel::getSecret(void) const { return (_secret); }

bool const	&Channel::getTopicprot(void) const { return (_topicprot); }

bool const	&Channel::getExternal(void) const { return (_external); }

time_t const	&Channel::getTime(void) const { return (_time); }

size_t const	&Channel::getLimit(void) const { return (_limit); }

std::list<std::list<std::string> >	&Channel::getBanlist(void) {return (_banlist);}

std::list<std::string>	&Channel::getInvitedlist(void) {return (_invitedlist);}

std::map<User *, std::string>	&Channel::getUsersInChannel(void) {return (_usersInChannel);}

void	Channel::setKey(std::string key) { _key = key; }

void	Channel::setTopic(std::string topic) { _topic = topic; }

void	Channel::setTopicWho(std::string topicwho) { _topicwho = topicwho; }

void	Channel::setTopicTime(std::string topictime) { _topictime = topictime; }

void	Channel::setInvite(bool invite) { _invite = invite; }

void	Channel::setModer(bool moder) { _moder = moder; }

void	Channel::setSecret(bool secret) { _secret = secret; }

void	Channel::setTopicprot(bool topicprot) { _topicprot = topicprot; }

void	Channel::setExternal(bool external) { _external = external; }

void	Channel::setTime(time_t time) { _time = time; }

void	Channel::setLimit(size_t limit) { _limit = limit; }


/* Member functions */

void	Channel::addUser(User &user, std::string mode)
{
	_usersInChannel.insert(std::make_pair(&user, mode));
	user.addChannel(*this);
}

void	Channel::removeUser(User &user)
{
	if (_usersInChannel.find(&user) != _usersInChannel.end())
		user.deleteUserfromChannel(*this);
	_usersInChannel.erase(&user);
}

bool	Channel::empty(void)
{
	if (_usersInChannel.empty())
		return (true);
	return (false);
}
