/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jlopez-f <jlopez-f@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/17 13:01:27 by jlopez-f          #+#    #+#             */
/*   Updated: 2023/02/20 17:05:05 by jlopez-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BOT_HPP
# define BOT_HPP

# include <sys/types.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <poll.h>
# include <string>
# include <string.h>
# include <iostream>
# include <sstream>
# include <unistd.h>
# include <stdlib.h>
# include <termios.h>
# include <errno.h>
# include <csignal>
# include <ctime>
# include <fcntl.h>
# include <vector>
# include <list>
# include <map>
# include <algorithm>
# include "Parser.hpp"

# define NOCOLOR "\033[0m"
# define BLUE "\033[0;36m"
# define GREEN "\033[0;32m"
# define RED "\033[0;31m"
# define YELLOW "\033[0;33m"

# define PORT_DEF "6667"
# define MIN_PORT 1023
# define MAX_PORT 65535
# define PASSLEN 100
# define POLL_TIMEOUT 15
# define AFK_TIMEOUT 90
# define BUFF_SIZE 512

# define NICK "WeabSegBot"
# define USER "WeabooSegfaultBot"
# define REALNAME "IRCBot"

# define ERR_BADSETUP 1
# define ERR_BADPOLL 2
# define ERR_BADLOGIN 2

# define RPL_WELCOME "001"
# define RPL_YOURHOST "002"
# define RPL_CREATED "003"
# define RPL_MYINFO "004"
# define RPL_ISUPPORT "005"
# define RPL_MOTD "372"
# define RPL_MOTDSTART "375"
# define RPL_ENDOFMOTD "376"

# define ERR_NOSUCHCHANNEL "403"
# define ERR_TOOMANYCHANNELS "405"
# define ERR_UNKNOWNCOMMAND "421"
# define ERR_ERRONEUSNICKNAME "432"
# define ERR_NICKNAMEINUSE "433"
# define ERR_NEEDMOREPARAMS "461"
# define ERR_PASSWDMISMATCH "464"
# define ERR_BANNEDFROMCHAN "475"
# define ERR_BADCHANMASK "476"
# define ERR_CHANOPRIVSNEEDED "482"

extern bool	*execptr;

class Parser;

class Bot
{
	public:

		/* Constructors */

		Bot(std::string const port, std::string const password);

		/* Destructor */

		~Bot(void);

		/* Bot Operation */

		void	start(void);
		void	login(Parser &result, int fd);
		void	receive_data(int fd);
		void	execute(Parser &result, int fd);
		void	sendmsgto(int fd, std::string const &msg) const;

		void	privmsg(Parser &result, int fd);
		void	invite(Parser &result, int fd);
		void	ping(Parser &result, int fd);
		bool	getmode(Parser &result, std::string &channel, std::string &nick);

	private:

		/* Member Variables */
		
		bool		_execute;
		bool		_loged;
		bool		_loginsent;
		time_t		_servlasttime;
		std::string	_port;
		std::string	_password;
		std::string	_ip;
		std::string	_oldline;
		
		std::map<std::string, std::string>	_invitinguser;

		/* Constructors */

		Bot(void);
		Bot(const Bot &other);

		/* Operator assignation */

		Bot	&operator=(const Bot &other);

};

#endif
