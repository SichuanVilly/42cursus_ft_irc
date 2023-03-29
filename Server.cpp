/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jlopez-f <jlopez-f@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/16 16:14:10 by jlopez-f          #+#    #+#             */
/*   Updated: 2023/02/20 17:15:24 by jlopez-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/* Constructors */

Server::Server(void) : _execute(true), _port(PORT_DEF), _password(), _ip(), _date(), _pollfds(), _all_users(), _all_channels(), _commands() { execptr = &_execute; }

Server::Server(std::string const port, std::string const password) : _execute(true), _port(port), _password(password),
_ip(), _date(), _pollfds(), _all_users(), _all_channels(), _commands() { if (_port.empty()) _port = PORT_DEF; execptr = &_execute; }

Server::Server(const Server &other) { *this = other; }

/* Destructor */

Server::~Server(void) {}

/* Operator assignation */

Server	&Server::operator=(const Server &other)
{
	if (this != &other)
	{
		_execute = other._execute;
		_port = other._port;
		_password = other._password;
		_ip = other._ip;
		_date = other._date;
		_pollfds = other._pollfds;
		_all_users = other._all_users;
		_all_channels = other._all_channels;
	}
	return (*this);
}

/* Element access */

std::vector<User *> const	&Server::getAllUsers(void) const { return (_all_users); }

std::list<Channel *> const	&Server::getAllChannels(void) const { return (_all_channels); }

/* Server Setup */

static void	signal_handler(int sig) { (void)sig; *execptr = false; }

static void	no_ctrlprint(struct termios *saved_tty)
{
	struct termios	t;

	tcgetattr(0, saved_tty);
	tcgetattr(0, &t);
	t.c_lflag &= ~ECHOCTL;
	tcsetattr(0, TCSANOW, &t);
}

static std::string	getip(void)
{
	char			host[NI_MAXHOST];
	char			ip[INET6_ADDRSTRLEN];
	struct addrinfo	hints;
	struct addrinfo	*info;
	struct addrinfo	*temp;

	gethostname(host, sizeof(host));
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	if (getaddrinfo(host, NULL, &hints, &info) != 0)
	{
		std::cerr << RED << "Error. Unable to retrieve hostname." << NOCOLOR << std::endl;
		freeaddrinfo(info);
		exit (ERR_BADSETUP);
	}
	for(temp = info; temp != NULL; temp = temp->ai_next)
	{
		if (temp->ai_family == AF_INET)
			inet_ntop(temp->ai_family, &((struct sockaddr_in *)temp->ai_addr)->sin_addr, ip, sizeof(ip));
		else
			inet_ntop(temp->ai_family, &((struct sockaddr_in6 *)temp->ai_addr)->sin6_addr, ip, sizeof(ip));
	}
	freeaddrinfo(info);
	return (ip);
}

static int	get_listener(int const queue_size, char const *port)
{
	int				listenfd;
	int				yes = 1;
	struct addrinfo	hints;
	struct addrinfo	*info;
	struct addrinfo	*temp;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if (getaddrinfo(NULL, port, &hints, &info) != 0)
	{
		std::cerr << RED << "Error. Unable to get network address." << NOCOLOR << std::endl;
		freeaddrinfo(info);
		exit (ERR_BADSETUP);
	}
	for (temp = info; temp != NULL; temp = temp->ai_next)
	{
		listenfd = socket(temp->ai_family, temp->ai_socktype, temp->ai_protocol);
		if (listenfd < 0)
			continue ;
		setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
		if (bind(listenfd, temp->ai_addr, temp->ai_addrlen) != 0)
		{
			close(listenfd);
			continue ;
		}
		break ;
	}
	freeaddrinfo(info);
	if (!temp)
	{
		std::cerr << RED << "Error. Unable to bind socket." << NOCOLOR << std::endl;
		close(listenfd);
		exit (ERR_BADSETUP);
	}
	if (listen(listenfd, queue_size) != 0)
	{
		std::cerr << RED << "Error. Unable to setup socket." << NOCOLOR << std::endl;
		close(listenfd);
		exit (ERR_BADSETUP);
	}
	return (listenfd);
}

/* Server Operation */

void	Server::receive_data(std::vector<pollfd> &_pollfds, int const i)
{
		char	buf[BUFF_SIZE + 1];
		int		nbytes;
		int		sender_fd;
		User	*user;

		sender_fd = _pollfds[i].fd;
		user = find_user(sender_fd);
		nbytes = recv(sender_fd, buf, BUFF_SIZE, 0);
		if (nbytes <= 0)
		{
			std::string	temp;

			if (user->getValid())
				temp = ":" + user->getNickName() + "!" + user->getUserName() + "@" + user->getHost();
			else
				temp = ":user!@" + user->getHost();
			if (nbytes == 0)
				temp += " QUIT :Quit: Disconnected";
			else
				temp += " QUIT :Quit: Error. Unexpected disconnection";
			sendToAll(*user, temp);
			std::cout << RED << temp << NOCOLOR << std::endl;
			deleteUser(user);
		}
		else
		{
			Parser						*result;
			std::string					line;
			std::vector<std::string>	cmds;

			user->setLastTime(time(NULL));
			buf[nbytes] = '\0';
			line = buf;
			//std::cout << BLUE << "Message rece: " << line << NOCOLOR;
			Parser::splitcmds(line, cmds);
			Parser::checkold(user, cmds);
			for(std::vector<std::string>::const_iterator it = cmds.begin(); it != cmds.end(); it++)
			{
				result = Parser::parseline(*it);
				if (!result)
					continue ;
				execute(*result, *user);
				delete result;
			}
		}
}

void	Server::start(void)
{
	int						listenfd, newfd;
	struct sockaddr_storage	clientaddr;
	socklen_t				addrlen;
	struct termios			saved_tty;

	_ip = getip();
	listenfd = get_listener(LISTENQ, _port.c_str());
	fcntl(listenfd, F_SETFL, O_NONBLOCK);
	_pollfds.push_back(pollfd());
	_pollfds[0].fd = listenfd;
	_pollfds[0].events = POLLIN;
	_date = gettime();
	commands();
	signal(SIGINT, signal_handler);
	no_ctrlprint(&saved_tty);
	std::cout << YELLOW << "WeabooSegfaultIRC (" << _ip << ":" << _port << ") connected" << NOCOLOR << std::endl;
	while (_execute)
	{
		if (poll(&_pollfds[0], _pollfds.size(), 1000 * POLL_TIMEOUT) == -1)
		{
			if (errno != EINTR)
			{
				std::cerr << RED << "Error. Unable to poll incoming connections." << NOCOLOR << std::endl;
				for(std::vector<User *>::iterator it = _all_users.begin(); it != _all_users.end(); )
				{
					deleteUser(*it);
					it = _all_users.begin();
					if (!_all_users.size())
						break ;
				}
				close(_pollfds[0].fd);
				exit (ERR_BADPOLL);
			}
		}
		for (size_t i = 0; i < _pollfds.size(); i++)
		{
			if (_pollfds[i].revents & POLLIN)
			{
				if (_pollfds[i].fd == listenfd)
				{
					addrlen = sizeof(clientaddr);
					newfd = accept(listenfd,(struct sockaddr *)&clientaddr, &addrlen);
					if (newfd == -1)
					{
						std::cerr << RED << "Error. Connection error." << NOCOLOR << std::endl;
						continue ;
					}
					if (User::newUser(newfd, (struct sockaddr *)&clientaddr, addrlen, &_all_users) != 0)
						continue ;
					std::cout << YELLOW << "New user connected: User " << find_user(newfd)->getFd();
					std::cout << " (" << find_user(newfd)->getHost() << ":" << find_user(newfd)->getService() << ")" << NOCOLOR << std::endl;
					_pollfds.push_back(pollfd());
					_pollfds.back().fd = newfd;
					_pollfds.back().events = POLLIN;
				}
				else
					receive_data(_pollfds, i);
			}
		}
		pingusers();
	}
	std::cout << YELLOW << "Disconnecting server. Thanks for using WeabooSegfaultIRC" << NOCOLOR << std::endl;
	for(std::vector<User *>::iterator it = _all_users.begin(); it != _all_users.end(); )
	{
		deleteUser(*it);
		it = _all_users.begin();
		if (!_all_users.size())
			break ;
	}
	close(_pollfds[0].fd);
	tcsetattr(0, TCSANOW, &saved_tty);
}
