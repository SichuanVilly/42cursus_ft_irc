/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jlopez-f <jlopez-f@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/24 18:33:08 by pvillena          #+#    #+#             */
/*   Updated: 2023/02/17 19:35:31 by jlopez-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "User.hpp"

/* Constructors */

User::User() : _userFd(0), _username(), _realname(), _nickname(), _host(), _service(), _oldmsg(), _time(gettime()), _away(), _connecttime(time(NULL)),
_lasttime(time(NULL)), _pass(false), _valid(false), _inv(false), _oper(false), _wall(false), _pinged(false), _Channels() ,_joinedChannels(0) {}

User::User(int fd) : _userFd(fd), _username(), _realname(), _nickname(), _host(), _service(), _oldmsg(), _time(gettime()), _away(), _connecttime(time(NULL)),
_lasttime(time(NULL)), _pass(false), _valid(false), _inv(false), _oper(false), _wall(false), _pinged(false), _Channels(), _joinedChannels(0) {}

User::User(const User &other) { *this = other; }

/* Destructor */

User::~User() {}

/* Operator assignation */

User	&User::operator=(const User &other)
{
	if (this != &other)
	{
		_userFd = other._userFd;
		_username = other._username;
		_realname = other._realname;
		_nickname = other._nickname;
		_host = other._host;
		_service = other._service;
		_oldmsg = other._oldmsg;
		_time = other._time;
		_away = other._away;
		_connecttime = other._connecttime;
		_lasttime = other._lasttime;
		_pass = other._pass;
		_valid = other._valid;
		_inv = other._inv;
		_oper = other._oper;
		_wall = other._wall;
		_pinged = other._pinged;
		_Channels = other._Channels;
		_joinedChannels = other._joinedChannels;
	}
	return (*this);
}

/* Element access */

int const	&User::getFd(void) const { return (_userFd); }

std::string const	&User::getUserName(void) const { return (_username); }

std::string const	&User::getRealName(void) const { return (_realname); }

std::string const	&User::getNickName(void) const { return (_nickname); }

std::string const	&User::getHost(void) const { return (_host); }

std::string const	&User::getService(void) const { return (_service); }

std::string const	&User::getOldmsg(void) const { return (_oldmsg); }

std::string const	&User::getTime(void) const { return (_time); }

std::string const	&User::getAway(void) const { return (_away); }

time_t const	&User::getConnectTime(void) const { return (_connecttime); }

time_t const	&User::getLastTime(void) const { return (_lasttime); }

bool const	&User::getPass(void) const { return (_pass); }

bool const	&User::getValid(void) const { return (_valid); }

bool const	&User::getInvis(void) const { return (_inv); }

bool const	&User::getOper(void) const { return (_oper); }

bool const	&User::getWall(void) const { return (_wall); }

bool const	&User::getPinged(void) const { return (_pinged); }

size_t const	&User::getJoinChann(void) const { return (_joinedChannels); }

std::list<Channel *> const	&User::getChannels(void) const { return (_Channels); }

void	User::setUserName(std::string username) { _username = username; }

void	User::setRealName(std::string realname) { _realname = realname; }

void	User::setNickName(std::string nickname) { _nickname = nickname; }

void	User::setHost(std::string host) { _host = host; }

void	User::setService(std::string service) { _service = service; }

void	User::setOldmsg(std::string oldmsg) { _oldmsg = oldmsg; }

void	User::setAway(std::string away) { _away = away; }

void	User::setLastTime(time_t lasttime) { _lasttime = lasttime; }

void	User::setPass(bool pass) { _pass = pass; }

void	User::setValid(bool valid) { _valid = valid; }

void	User::setInvis(bool inv) { _inv = inv; }

void	User::setOper(bool oper) { _oper = oper; }

void	User::setWall(bool wall) { _wall = wall; }

void	User::setPinged(bool pinged) { _pinged = pinged; }

void	User::addedToChannel(void) { _joinedChannels++; }

void	User::removedFromChannel(void) { _joinedChannels--; }

/* Member functions */

int	User::newUser(int fd, struct sockaddr * addr, socklen_t addrlen, std::vector<User *> *all_users)
{
	User		*temp = new User(fd);
	char		host[NI_MAXHOST];
	char		service[NI_MAXSERV];
	std::string	hoststring;

	if (getnameinfo(addr, addrlen, host, sizeof(host), service, sizeof(service), NI_NUMERICHOST) != 0)
	{
		std::cerr << RED << "Error. Couldn't retrieve hostname." << NOCOLOR << std::endl;
		close(fd);
		delete temp;
		return (ERR_BADSETUP) ;
	}
	hoststring = host;
	if (hoststring.size() > HOSTLEN)
			hoststring.erase(HOSTLEN, std::string::npos);
	temp->setHost(hoststring);
	temp->setService(std::string (service));
	all_users->push_back(temp);
	return (0);
}

void User::addChannel(Channel &channel)
{
	_Channels.push_back(&channel);
	_joinedChannels++;
}

void User::deleteUserfromChannel(Channel &channel)
{
	_Channels.erase(std::find (_Channels.begin(), _Channels.end(), &channel));
	_joinedChannels--;
}

bool	User::shareChannel(const User &other)
{
	for (std::list<Channel *>::const_iterator it = _Channels.begin(); it != _Channels.end(); it++)
	{
		for (std::list<Channel *>::const_iterator it2 = other._Channels.begin(); it2 != other._Channels.end(); it2++)
		{
			if ((*it)->getChannelName() == (*it2)->getChannelName())
				return (true);
		}
	}
	return (false);
}

bool	User::isinChannel(std::string const &channel)
{
	for (std::list<Channel *>::const_iterator it = _Channels.begin(); it != _Channels.end(); it++)
	{
		if ((*it)->getChannelName() == channel)
			return (true);
	}
	return (false);
}

void User::print_all() const
{
	std::cout << _userFd << std::endl;
	std::cout << _username << std::endl;
	std::cout << _nickname << std::endl;
	std::cout << _host << std::endl;
	std::cout << _service << std::endl;
	std::cout << _joinedChannels << std::endl;
}
