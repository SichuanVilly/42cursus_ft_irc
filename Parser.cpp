/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jlopez-f <jlopez-f@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/16 16:14:10 by jlopez-f          #+#    #+#             */
/*   Updated: 2023/02/08 01:09:09 by jlopez-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

/* Constructors */

Parser::Parser(void) : _line(), _source(), _command(), _param() {}

Parser::Parser(std::string const line) : _line(line), _source(), _command(), _param()
{
	_line.erase(std::find(_line.begin(), _line.end(), '\n'), _line.end());
	_line.erase(std::find(_line.begin(), _line.end(), '\r'), _line.end());
}

Parser::Parser(const Parser &other) { *this = other; }

/* Destructor */

Parser::~Parser(void) {}

/* Operator assignation */

Parser	&Parser::operator=(const Parser &other)
{
	if (this != &other)
	{
		_line = other._line;
		_source = other._source;
		_command = other._command;
		_param = other._param;
	}
	return (*this);
}

/* Element access */

std::vector<std::string> const & Parser::getParam() const { return (_param); }

std::string const Parser::getParamSend(size_t i) const
{
	std::string	temp;

	if (!getParam().size() || i > getParam().size() - 1)
		return (temp);
	if (i == getParam().size() - 1)
		temp = ":" + getParam()[i];
	else
		temp = getParam()[i];
	return (temp);
}

std::string const &Parser::getCommand() const { return (_command); };

/* Member Functions */

void	Parser::splitcmds(std::string line, std::vector<std::string> &cmds)
{
	std::string				temp;
	std::string::iterator	it;

	temp = line;
	while (!line.empty())
	{
		it = std::find(temp.begin(), temp.end(), '\n');
		if (it != temp.end())
			it++;
		temp.erase(it, temp.end());
		cmds.push_back(temp);
		line.erase(0, temp.length());
		temp = line;
	}
}

void	Parser::checkold(User *user, std::vector<std::string> &cmds)
{
	std::string	old;

	old = user->getOldmsg();
	cmds.front() = old + cmds.front();
	if (cmds.front().length() > BUFF_SIZE)
	{
		cmds.front()[BUFF_SIZE - 2] = '\r';
		cmds.front()[BUFF_SIZE - 1] = '\n';
		cmds.front().erase(BUFF_SIZE, std::string::npos);
	}
	user->setOldmsg("");
	if (cmds.back()[cmds.back().length() - 1] != '\n')
	{
		user->setOldmsg(cmds.back());
		cmds.pop_back();
	}
}

void	Parser::print(void)
{
	size_t	i = 0;

	std::cout << std::endl << "Line    -> " << _line << "\nSource  -> " << _source << "\nCommand -> "
	<< _command << std::endl;
	while (i < _param.size())
	{
		std::cout << "Param[" << i << "]-> " << _param[i] << std::endl;
		i++;
	}
	std::cout << std::endl;
}

static bool	checkspace(char c) { return (c != ' '); }
static bool	checkalpha(char c) { return (!std::isalpha(c)); }
static bool	checkdigit(char c) { return (!std::isdigit(c)); }

bool	Parser::command_check(void)
{
	if (std::find_if(_command.begin(), _command.end(), checkalpha) != _command.end())
	{
		if (_command.length() == 3 && std::find_if(_command.begin(), _command.end(), checkdigit) == _command.end())
			return (true);
		return (false);
	}
	for (std::string::iterator it = _command.begin(); it != _command.end(); it++)
		*it = std::toupper(*it);
	return (true);
}

void	Parser::param(void)
{
	std::string	temp;

	temp = _line;
	if (_source.length())
		temp.erase(0, _source.length() + 1);
	temp.erase(temp.begin(), std::find_if(temp.begin(), temp.end(), checkspace));
	temp.erase(0, _command.length());
	while (!temp.empty())
	{
		temp.erase(temp.begin(), std::find_if(temp.begin(), temp.end(), checkspace));
		_param.push_back(temp);
		if (_param.back()[0] != ':')
			_param.back().erase(std::find(_param.back().begin(), _param.back().end(), ' '), _param.back().end());
		temp.erase(0, _param.back().length());
	}
	if (!_param.empty() && _param.back()[0] == ':')
		_param.back().erase(0, 1);
}

void	Parser::command(void)
{
	_command = _line;
	if (_source.length())
		_command.erase(0, _source.length() + 1);
	_command.erase(_command.begin(), std::find_if(_command.begin(), _command.end(), checkspace));
	_command.erase(std::find(_command.begin(), _command.end(), ' '), _command.end());
}

void	Parser::source(void)
{
	_source = _line;
	_source.erase(0, 1);
	_source.erase(std::find(_source.begin(), _source.end(), ' '), _source.end());
}

Parser	*Parser::parseline(std::string const line)
{
	Parser	*result = new Parser(line);

	if (result->_line[0] == ':')
		result->source();
	result->command();
	if (!result->_command.length())
	{
		delete result ;
		return (NULL);
	}
	result->param();
	if (result->_param.size() > 15)
	{
		delete result ;
		return (NULL);
	}
	return(result);
}

