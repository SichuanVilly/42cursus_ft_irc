/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jlopez-f <jlopez-f@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/16 16:06:21 by jlopez-f          #+#    #+#             */
/*   Updated: 2023/02/20 17:02:58 by jlopez-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "IRC.hpp"

# define PORT_DEF "6667"
# define LISTENQ 10
# define POLL_TIMEOUT 15
# define AFK_TIMEOUT 90
# define PING_FQ 40
# define BUFF_SIZE 512
# define FORBNICKCHAR " ,*?!@."
# define FORBSTARTNICKCHAR "$:&#"

# define OPER_NAME "oper"
# define OPER_PASS "pass"
# define AWAYLEN 100
# define CHANLIMIT 10
# define MODE_USER "iw"
# define MODE_CHANN "blikmstnov"
# define CHANNELLEN 20
# define HOSTLEN 64
# define KICKLEN 100
# define NICKLEN 20
# define TOPICLEN 100
# define USERLEN 20

class User;
class Parser;

class Server
{
	public:

		/* Constructors */

		Server(std::string const port, std::string const password);

		/* Destructor */

		~Server(void);

		/* Element access */

		std::vector<User *> const	&getAllUsers(void) const;
		std::list<Channel *> const	&getAllChannels(void) const;

		/* Server Operation */

		void	start(void);
		void	receive_data(std::vector<pollfd> &pollfds, int const i);
		void	execute(Parser &result, User &user);
		void	connaccept(User const &user) const;
		void	sendmsgto(User const &user, std::string const &msg) const;
		void	sendservmsg(User const &user, std::string const &msg, std::string const &code) const;
		void	sendToAll(User const &user, std::string const &msg) const;
		void	sendToChannel(User &user, Channel &channel, std::string const &msg) const;
		void	sendmotd(User const &user) const;
		bool	is_user_valid(User &user, Parser const &result);

		/* Server Utils */

		Channel		*find_channel(std::string const &channel);
		User		*find_user(std::string const &nick);
		User		*find_user(const int fd);
		bool		isoper(User *user, Channel *channel);
		std::string	fullname(User &user);

		Channel	*newChannel(std::string channelName, User &newUser);
		void	deleteChannels(void);
		void	deleteUser(User * user);
		void	leaveallChannels(User &user);

		void	pingusers(void);

		/* Commands */

		void	pass(Parser const &result, User &user);
		void	nick(Parser const &result, User &user);
		void	usercmd(Parser const &result, User &user);
		void	ping(Parser const &result, User &user);
		void	oper(Parser const &result, User &user);
		void	quit(Parser const &result, User &user);
		void	join(Parser const &result, User &user);
		void	part(Parser const &result, User &user);
		void	topic(Parser const &result, User &user);
		void	names(Parser const &result, User &user);
		void	list(Parser const &result, User &user);
		void	invite(Parser const &result, User &user);
		void	kick(Parser const &result, User &user);
		void	motd(Parser const &result, User &user);
		void	version(Parser const &result, User &user);
		void	admin(Parser const &result, User &user);
		void	lusers(Parser const &result, User &user);
		void	timecmd(Parser const &result, User &user);
		void	info(Parser const &result, User &user);
		void	mode(Parser const &result, User &user);
		void	privmsg(Parser const &result, User &user);
		void	notice(Parser const &result, User &user);
		void	who(Parser const &result, User &user);
		void	whois(Parser const &result, User &user);
		void	kill(Parser const &result, User &user);
		void	away(Parser const &result, User &user);
		void	wallops(Parser const &result, User &user);
		
		void	join_to_channel(Channel *chan, User *user, std::list<std::string>::iterator ikeys, bool is_end);
		void	create_channel(std::string name, User *user);
		void	channelJoinInfo(User &user, Channel &Channel);

		void		modeuser(Parser const &result, User &user);
		void		modechann(Parser const &result, User &user);
		void		execmodesChannel(User &user, Channel &chan, std::list<std::pair<std::string, std::string> > modes);
		void		execmodesUser(User &user, std::list<std::string> modes);
		bool		banmode(User &user, Channel &chan, std::pair<std::string, std::string> mode);
		bool		limitmode(Channel &chan, std::pair<std::string, std::string> mode);
		bool		invitemode(Channel &chan, std::pair<std::string, std::string> mode);
		bool		keymode(Channel &chan, std::pair<std::string, std::string> mode);
		bool		modermode(Channel &chan, std::pair<std::string, std::string> mode);
		bool		secretmode(Channel &chan, std::pair<std::string, std::string> mode);
		bool		protetopicmode(Channel &chan, std::pair<std::string, std::string> mode);
		bool		externalmode(Channel &chan, std::pair<std::string, std::string> mode);
		bool		operchanmode(Channel &chan, std::pair<std::string, std::string> mode);
		bool		voicechanmode(Channel &chan, std::pair<std::string, std::string> mode);
		bool		invismode(User &user, std::string mode);
		bool		wallmode(User &user, std::string mode);
		bool		parsemodes(std::string &modes, User &user, Channel *chan);
		std::string	getmodesUser(User &user);
		std::string	getmodesChan(Channel *chan, User *user);

		void	whouser(std::pair<User *, std::string> user, User &whoask, Channel *chan);
		void	whoischannel(User &user, User &targetuser);
		void	commands(void);

		/* Static Members */

		static std::string	strtoupper(std::string const &str);

	private:

		/* Member Variables */

		bool					_execute;
		std::string				_port;
		std::string				_password;
		std::string				_ip;
		std::string				_date;
		std::vector<pollfd>		_pollfds;
		std::vector<User *>		_all_users;
		std::list<Channel *>	_all_channels;

		std::map<std::string, void(Server::*)(Parser const &result, User &user)>	_commands;

		/* Constructors */

		Server(void);
		Server(const Server &other);

		/* Operator assignation */

		Server	&operator=(const Server &other);

};

std::list<std::string>	adv_tokenizer(std::string s, char del);

#endif
