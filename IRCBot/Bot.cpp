/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jlopez-f <jlopez-f@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/15 17:11:46 by jlopez-f          #+#    #+#             */
/*   Updated: 2023/02/20 17:07:49 by jlopez-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Bot.hpp"

/* Constructors */

Bot::Bot(void) : _execute(true), _loged(false), _servlasttime(), _port(PORT_DEF), _password(), _ip(), _oldline(), _invitinguser() { execptr = &_execute; }

Bot::Bot(std::string const port, std::string const password) : _execute(true), _loged(false), _servlasttime(), _port(port), _password(password), _ip(), _oldline(), _invitinguser() { if (_port.empty()) _port = PORT_DEF; execptr = &_execute; }

Bot::Bot(const Bot &other) { *this = other; }

/* Destructor */

Bot::~Bot(void) {}

/* Operator assignation */

Bot	&Bot::operator=(const Bot &other)
{
	if (this != &other)
	{
		_execute = other._execute;
		_loged = other._loged;
		_port = other._port;
		_password = other._password;
		_ip = other._ip;
		_oldline = other._oldline;
		_invitinguser = other._invitinguser;
	}
	return (*this);
}

/* Bot Setup */

static void	signal_handler(int sig) { (void)sig; *execptr = false; }

static void	no_ctrlprint(struct termios *saved_tty)
{
	struct termios	t;

	tcgetattr(0, saved_tty);
	tcgetattr(0, &t);
	t.c_lflag &= ~ECHOCTL;
	tcsetattr(0, TCSANOW, &t);
}

static std::string	getip(int connectfd)
{
	char					ip[INET6_ADDRSTRLEN];
	socklen_t				addrlen = INET6_ADDRSTRLEN;
	struct sockaddr_storage	info;

	memset(&info, 0, sizeof(info));
	if (getsockname(connectfd, (struct sockaddr*)&info, &addrlen) != 0)
	{
		std::cerr << RED << "Error. Unable to get server address." << NOCOLOR << std::endl;
		close(connectfd);
		exit (ERR_BADSETUP);
	}
	if (info.ss_family == AF_INET)
		inet_ntop(info.ss_family, &((struct sockaddr_in *)&info.ss_family)->sin_addr, ip, addrlen);
	else
		inet_ntop(info.ss_family, &((struct sockaddr_in6 *)&info.ss_family)->sin6_addr, ip, addrlen);
	return (ip);
}

static int	get_connection(char const *port)
{
	int				connectfd;
	struct addrinfo	hints;
	struct addrinfo	*info;
	struct addrinfo	*temp;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if (getaddrinfo(NULL, port, &hints, &info) != 0)
	{
		std::cerr << RED << "Error. Unable to get server address." << NOCOLOR << std::endl;
		freeaddrinfo(info);
		exit (ERR_BADSETUP);
	}
	for (temp = info; temp != NULL; temp = temp->ai_next)
	{
		connectfd = socket(temp->ai_family, temp->ai_socktype, temp->ai_protocol);
		if (connectfd < 0)
			continue ;
		if (connect(connectfd, temp->ai_addr, temp->ai_addrlen) != 0) 
		{
			close(connectfd);
			continue ;
		}
		break ;
	}
	freeaddrinfo(info);
	if (!temp)
	{
		std::cerr << RED << "Error. Unable to connect to server." << NOCOLOR << std::endl;
		close(connectfd);
		exit (ERR_BADSETUP);
	}
	return (connectfd);
}

/* Bot Operation */

void	Bot::receive_data(int fd)
{
	char	buf[BUFF_SIZE + 1];
	int		nbytes;

	nbytes = recv(fd, buf, BUFF_SIZE, 0);
	if (nbytes <= 0)
	{
		std::cout << RED << "Error. Unexpected disconnection" << NOCOLOR << std::endl;
		close(fd);
		_execute = false;
	}
	else
	{
		Parser						*result;
		std::string					line;
		std::vector<std::string>	cmds;

		_servlasttime = time(NULL);
		buf[nbytes] = '\0';
		line = buf;
		//std::cout << BLUE << "Message rece: " << line << NOCOLOR;
		Parser::splitcmds(line, cmds);
		Parser::checkold(_oldline, cmds);
		for(std::vector<std::string>::const_iterator it = cmds.begin(); it != cmds.end(); it++)
		{
			result = Parser::parseline(*it);
			if (!result)
				continue ;
			execute(*result, fd);
			delete result;
		}
	}
}

void	Bot::start(void)
{
	int				connectfd;
	pollfd			pollfd;
	struct termios	saved_tty;
	time_t			starttime = time(NULL);

	connectfd = get_connection(_port.c_str());
	_ip = getip(connectfd);
	fcntl(connectfd, F_SETFL, O_NONBLOCK);
	pollfd.fd = connectfd;
	pollfd.events = POLLIN;
	pollfd.revents = 0;
	signal(SIGINT, signal_handler);
	no_ctrlprint(&saved_tty);
	std::cout << YELLOW << "WeabooSegfaultBot (" << _ip << ":" << _port << ") connected" << NOCOLOR << std::endl;
	sendmsgto(connectfd, "PASS " + _password + "\n" + "NICK " + NICK + "\nUSER " + USER + " 0 * :" + REALNAME + "\n");
	while (_execute)
	{
		if (poll(&pollfd, 1, 1000 * POLL_TIMEOUT) == -1)
		{
			if (errno != EINTR)
			{
				std::cerr << RED << "Error. Unable to poll incoming connections." << NOCOLOR << std::endl;
				close(pollfd.fd);
				exit (ERR_BADPOLL);
			}
		}
		if (pollfd.revents & POLLIN)
			receive_data(pollfd.fd);
		if (!_loged && std::difftime(time(NULL), starttime) > AFK_TIMEOUT)
		{
			std::cerr << RED << "Error. Unable to login." << NOCOLOR << std::endl;
			close(pollfd.fd);
			exit (ERR_BADLOGIN);
		}
	}
	std::cout << YELLOW << "Disconnecting bot. Thanks for using WeabooSegfaultBOT" << NOCOLOR << std::endl;
	close(connectfd);
	tcsetattr(0, TCSANOW, &saved_tty);	
}
