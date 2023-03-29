/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jlopez-f <jlopez-f@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/24 18:21:21 by pvillena          #+#    #+#             */
/*   Updated: 2023/02/14 17:31:13 by jlopez-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "IRC.hpp"

class User;

class Channel
{
	public:
		
		/* Constructors */

		Channel(std::string name);
		Channel(const Channel &other);

		/* Destructor */

		~Channel();

		/* Operator assignation */

		Channel	&operator=(const Channel &other);

		/* Element access */

		std::string const	&getChannelName(void) const;
		std::string const	&getKey(void) const;
		std::string const	&getTopic(void) const;
		std::string const	&getTopicWho(void) const;
		std::string const	&getTopicTime(void) const;

		bool const		&getInvite(void) const;
		bool const		&getModer(void) const;
		bool const		&getSecret(void) const;
		bool const		&getTopicprot(void) const;
		bool const		&getExternal(void) const;
		time_t const	&getTime(void) const;
		size_t const	&getLimit(void) const;

		std::list<std::list<std::string> >		&getBanlist(void);
		std::list<std::string>					&getInvitedlist(void);
		std::map<User *, std::string>			&getUsersInChannel(void);

		void	setKey(std::string key);
		void	setTopic(std::string topic);
		void	setTopicWho(std::string topicwho);
		void	setTopicTime(std::string topictime);
		void	setInvite(bool invite);
		void	setModer(bool moder);
		void	setSecret(bool secret);
		void	setTopicprot(bool topicprot);
		void	setExternal(bool external);
		void	setTime(time_t time);
		void	setLimit(size_t limit);

		/* Member functions */

		void	addUser(User &user, std::string mode);
		void	removeUser(User &user);
		bool	empty(void);

	private:

		/* Member Variables */

		std::string	_channelName;
		std::string	_key;
		std::string	_topic;
		std::string	_topicwho;
		std::string	_topictime;

		bool	_invite;
		bool	_moder;
		bool	_secret;
		bool	_topicprot;
		bool	_external;
		time_t	_time;
		size_t	_limit;

		std::list<std::list<std::string> >	_banlist;
		std::list<std::string>				_invitedlist;
		std::map<User *, std::string>		_usersInChannel;

		/* Constructors */

		Channel();
};

#endif