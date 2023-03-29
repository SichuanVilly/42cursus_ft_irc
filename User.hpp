/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jlopez-f <jlopez-f@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/24 18:17:14 by pvillena          #+#    #+#             */
/*   Updated: 2023/02/14 15:42:34 by jlopez-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef USER_HPP
# define USER_HPP

# include "IRC.hpp"

class Channel;

class User
{
	public:

		/* Constructors */

		User(int fd);
		User(const User &other);

		/* Destructor */
		
		~User();

		/* Operator assignation */

		User &operator=(const User &other);

		/* Element access */

		int const					&getFd(void) const;
		std::string const			&getUserName(void) const;
		std::string const			&getRealName(void) const;
		std::string const			&getNickName(void) const;
		std::string const			&getHost(void) const;
		std::string const			&getService(void) const;
		std::string const			&getOldmsg(void) const;
		std::string const			&getTime(void) const;
		std::string const			&getAway(void) const;
		time_t const				&getConnectTime(void) const;
		time_t const				&getLastTime(void) const;
		bool const					&getPass(void) const;
		bool const					&getValid(void) const;
		bool const					&getInvis(void) const;
		bool const					&getOper(void) const;
		bool const					&getWall(void) const;
		bool const					&getPinged(void) const;
		std::list<Channel *> const	&getChannels(void) const;
		size_t const				&getJoinChann(void) const;

		void	setUserName(std::string username);
		void	setRealName(std::string realname);
		void	setNickName(std::string nickname);
		void	setHost(std::string host);
		void	setService(std::string service);
		void	setOldmsg(std::string oldmsg);
		void	setLastTime(time_t lasttime);
		void	setPass(bool pass);
		void	setValid(bool valid);
		void	setAway(std::string away);
		void	setInvis(bool inv);
		void	setOper(bool oper);
		void	setWall(bool wall);
		void	setPinged(bool pinged);
		void	removedFromChannel(void);
		void	addedToChannel(void);

		/* Member functions */

		void	deleteUserfromChannel(Channel & channel);
		void	addChannel(Channel & channel);
		bool	shareChannel(const User &other);
		bool	isinChannel(std::string const &channel);

		void	print_all(void) const;

		static int	newUser(int fd, struct sockaddr *addr, socklen_t addrlen, std::vector<User *> *all_users);

	private:

		/* Member Variables */

		int						_userFd;
		std::string				_username;
		std::string				_realname;
		std::string				_nickname;
		std::string				_host;
		std::string				_service;
		std::string				_oldmsg;
		std::string				_time;
		std::string				_away;
		time_t					_connecttime;
		time_t					_lasttime;
		bool					_pass;
		bool					_valid;
		bool					_inv;
		bool					_oper;
		bool					_wall;
		bool					_pinged;
		std::list<Channel *>	_Channels;
		size_t 					_joinedChannels;

		/* Constructors */

		User();
};


#endif