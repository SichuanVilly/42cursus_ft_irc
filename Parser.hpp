/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jlopez-f <jlopez-f@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/16 15:56:38 by jlopez-f          #+#    #+#             */
/*   Updated: 2023/02/20 07:09:12 by jlopez-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
# define PARSER_HPP

# include "IRC.hpp"

class Parser
{

	public:

		/* Constructors */

		Parser(std::string const line);
		Parser(const Parser &other);

		/* Destructor */

		~Parser();

		/* Operator assignation */

		Parser	&operator=(const Parser &other);

		/* Element access */

		std::vector<std::string> const	&getParam(void) const;
		std::string const				getParamSend(size_t i) const;
		std::string const				&getCommand(void) const;

		/* Member Functions */

		static void		splitcmds(std::string line, std::vector<std::string> &cmds);
		static void		checkold(User *user, std::vector<std::string> &cmds);

		static Parser	*parseline(std::string const line);
		void			source(void);
		void			command(void);
		void			param(void);

		bool			command_check(void);
		void			print(void);

	private:

		/* Member Variables */

		std::string					_line;
		std::string					_source;
		std::string					_command;
		std::vector<std::string>	_param;

		/* Constructors */

		Parser();

};

#endif
