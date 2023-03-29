/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRC.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jlopez-f <jlopez-f@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/28 14:24:52 by pvillena          #+#    #+#             */
/*   Updated: 2023/02/20 17:02:14 by jlopez-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRC_HPP
# define IRC_HPP

# include <sys/types.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <poll.h>
# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <errno.h>
# include <iostream>
# include <sstream>
# include <string.h>
# include <string>
# include <list>
# include <vector>
# include <map>
# include <algorithm>
# include <ctime>
# include <csignal>
# include <fcntl.h>
# include <termios.h>
# include "User.hpp"
# include "Channel.hpp"
# include "Server.hpp"
# include "Parser.hpp"

# define NOCOLOR "\033[0m"
# define BLUE "\033[0;36m"
# define GREEN "\033[0;32m"
# define RED "\033[0;31m"
# define YELLOW "\033[0;33m"

# define MIN_PORT 1023
# define MAX_PORT 65535
# define PASSLEN 100

extern bool	*execptr;

std::string	gettime(void);

/* Reply messages */

# define RPL_WELCOME "001"
# define RPL_WELCOME_MSG ":Welcome to the Weaboo Segfault Network, "

# define RPL_YOURHOST "002"
# define RPL_YOURHOST_MSG ":Your host is WeabooSegfaultIRC, running version 1.0"

# define RPL_CREATED "003"
# define RPL_CREATED_MSG ":This server was created "

# define RPL_MYINFO "004"
# define RPL_MYINFO_MSG ":ircserv 1.0 ovb mt"

# define RPL_ISUPPORT "005"
# define RPL_ISUPPORT_MSG ":AWAYLEN=100 CASEMAPPING=<ascii> CHANLIMIT=#&:10 CHANMODES=b,k,l,imstn CHANNELLEN=20 HOSTLEN=64 KICKLEN = 100 NICKLEN=20 PREFIX=~@+ TOPICLEN=100 USERLEN=20 :are supported by this server"

# define RPL_UMODEIS "221"

# define RPL_LUSERCLIENT "251"
# define RPL_LUSERCLIENT_MSG1 ":There are "
# define RPL_LUSERCLIENT_MSG2 " users and "
# define RPL_LUSERCLIENT_MSG3 " invisible"

# define RPL_LUSEROP "252"
# define RPL_LUSEROP_MSG ":operator(s) online"

# define RPL_LUSERCHANNELS "254"
# define RPL_LUSERCHANNELS_MSG ":channels formed"

# define RPL_LUSERME "255"
# define RPL_LUSERME_MSG1 ":I have "
# define RPL_LUSERME_MSG2 " clients"

# define RPL_ADMINME "256"
# define RPL_ADMINME_MSG "WeabooSegfaultIRC :Administrative info"

# define RPL_ADMINLOC1 "257"
# define RPL_ADMINLOC1_MSG ":Madrid (Spain)"

# define RPL_ADMINLOC2 "258"
# define RPL_ADMINLOC2_MSG ":jlopez-f & pvillena (42 Madrid)"

# define RPL_ADMINEMAIL "259"
# define RPL_ADMINEMAIL_MSG ":jlopez-f@student.42madrid.com & pvillena@student.42madrid.com"

# define RPL_AWAY "301"

# define RPL_UNAWAY "305"
# define RPL_UNAWAY_MSG ":You are no longer marked as being away"

# define RPL_NOWAWAY "306"
# define RPL_NOWAWAY_MSG ":You have been marked as being away"

# define RPL_WHOISUSER "311"

# define RPL_WHOISOPERATOR "313"
# define RPL_WHOISOPERATOR_MSG ":is an IRC operator"

# define RPL_ENDOFWHO "315"
# define RPL_ENDOFWHO_MSG "End of WHO list"

# define RPL_WHOISIDLE "317"
# define RPL_WHOISIDLE_MSG ":seconds idle, signon time"

# define RPL_ENDOFWHOIS "318"
# define RPL_ENDOFWHOIS_MSG ":End of /WHOIS list"

# define RPL_WHOISCHANNELS "319"

# define RPL_LISTSTART "321"
# define RPL_LISTSTART_MSG "Channel :Users  Name"

# define RPL_LIST "322"

# define RPL_LISTEND "323"
# define RPL_LISTEND_MSG ":End of /LIST"

# define RPL_CHANNELMODEIS "324"

# define RPL_CREATIONTIME "329"

# define RPL_NOTOPIC "331"
# define RPL_NOTOPIC_MSG ":No topic is set"

# define RPL_TOPIC "332"

# define RPL_TOPICWHOTIME "333"

# define RPL_INVITING "341"

# define RPL_VERSION "351"
# define RPL_VERSION_MSG "1.0 WeabooSegfaultIRC :Best 42 IRC server"

# define RPL_WHOREPLY "352"

# define RPL_NAMREPLY "353"

# define RPL_ENDOFNAMES "366"
# define RPL_ENDOFNAMES_MSG ":End of /NAMES list"

# define RPL_BANLIST "367"

# define RPL_ENDOFBANLIST "368"
# define RPL_ENDOFBANLIST_MSG ":End of channel ban list"

# define RPL_INFO "371"
# define RPL_INFO_MSG "WeabooSegfault IRC created by Javier López and Pablo Villena. Version 1.0. Best 42 IRC server"

# define RPL_ENDOFINFO "374"
# define RPL_ENDOFINFO_MSG ":End of INFO list"

# define RPL_MOTD "372"

# define RPL_MOTDSTART "375"
# define RPL_MOTDSTART_MSG ":- WeabooSegfaultIRC Message of the day -"

# define RPL_ENDOFMOTD "376"
# define RPL_ENDOFMOTD_MSG ":End of /MOTD command"

# define RPL_WHOISMODES "379"
# define RPL_WHOISMODES_MSG ":is using modes"

# define RPL_YOUREOPER "381"
# define RPL_YOUREOPER_MSG ":You are now an IRC operator"

# define RPL_TIME "391"
# define RPL_TIME_MSG "WeabooSegfaultIRC :"

/* Error messages */

# define ERR_BADSETUP 1
# define ERR_BADPOLL 2

# define ERR_NOSUCHNICK "401"
# define ERR_NOSUCHNICK_MSG ":No such nick"

# define ERR_NOSUCHCHANNEL "403"
# define ERR_NOSUCHCHANNEL_MSG ":No such channel"

# define ERR_CANNOTSENDTOCHAN "404"
# define ERR_CANNOTSENDTOCHAN_MSG ":Cannot send to channel"

# define ERR_TOOMANYCHANNELS "405"
# define ERR_TOOMANYCHANNELS_MSG ":You have joined too many channels"

# define ERR_UNKNOWNCOMMAND "421"
# define ERR_UNKNOWNCOMMAND_MSG ":Unknown command"

# define ERR_ERRONEUSNICKNAME "432"
# define ERR_ERRONEUSNICKNAME_MSG ":Erroneus nickname"

# define ERR_NICKNAMEINUSE "433"
# define ERR_NICKNAMEINUSE_MSG ":Nickname is already in use"

# define ERR_USERNOTINCHANNEL "441"
# define ERR_USERNOTINCHANNEL_MSG ":They aren't on that channel"

# define ERR_NOTONCHANNEL "442"
# define ERR_NOTONCHANNEL_MSG ":You're not on that channel"

# define ERR_USERONCHANNEL "443"
# define ERR_USERONCHANNEL_MSG ":is already on channel"

# define ERR_NEEDMOREPARAMS "461"
# define ERR_NEEDMOREPARAMS_MSG ":Not correct number of parameters"

# define ERR_ALREADYREGISTERED "462"
# define ERR_ALREADYREGISTERED_MSG ":You may not reregister"

# define ERR_PASSWDMISMATCH "464"
# define ERR_PASSWDMISMATCH_MSG ":Password incorrect"

# define ERR_CHANNELISFULL "471"
# define ERR_CHANNELISFULL_MSG ":Cannot join channel (+l)"

# define ERR_UNKNOWNMODE "472"
# define ERR_UNKNOWNMODE_MSG ":is unknown mode char to me"

# define ERR_INVITEONLYCHAN "473"
# define ERR_INVITEONLYCHAN_MSG ":Cannot join channel (+i)"

# define ERR_BANNEDFROMCHAN "474"
# define ERR_BANNEDFROMCHAN_MSG ":Cannot join channel (+b)"

# define ERR_BADCHANNELKEY "475"
# define ERR_BADCHANNELKEY_MSG ":Cannot join channel (+k)"

# define ERR_BADCHANMASK "476"
# define ERR_BADCHANMASK_MSG ":Bad Channel Mask"

# define ERR_NOPRIVILEGES "481"
# define ERR_NOPRIVILEGES_MSG ":Permission Denied- You're not an IRC operator"

# define ERR_CHANOPRIVSNEEDED "482"
# define ERR_CHANOPRIVSNEEDED_MSG ":You're not channel operator"

# define ERR_UMODEUNKNOWNFLAG "501"
# define ERR_UMODEUNKNOWNFLAG_MSG ":Unknown MODE flag"

# define ERR_USERSDONTMATCH "502"
# define ERR_USERSDONTMATCH_MSG ":Cant change mode for other users"

# define MOTD_GIRL01 "%&&/**////*********/(*******,*****,*,****,*,*//,**,*******************,**,*******/(,/*/////*/*//////\n"
# define MOTD_GIRL02 "%&&/////////////*/%****,****,***,***,**,,,,,***,,,,,,,,,,,*******,**,*,*,,**********#**/*//*********\n"
# define MOTD_GIRL03 "%&&//////(#%&@(*(*****,*****/(,,,,,/*,,,,,,,,,,,,,,,,,,,,,,,,,,,*,,,*(***,**********/@@@@&(((/***///\n"
# define MOTD_GIRL04 "%%%%%@@@@@@@@#/(,,**,,**,**/,*,,*,/*,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,/**,*******,*//&@@@(#@@@@@@@@%\n"
# define MOTD_GIRL05 "@@@@@@@@@@@%*(.,..,**,**,//,*,*,.*/,*,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,*(,,**,,**...*%@@@@**%@@@@@@@\n"
# define MOTD_GIRL06 "@@@@@@@@@@(*/.,...,.,*,,/*,,,*..,/,,,,,,,,,,,,,,,,,,,,,**,,,,,,,,,,,,,,,,/**,,***.,../@@@@&*&@@@@@@@\n"
# define MOTD_GIRL07 "@@@@@@@@@&%.,......,*,,/*,,,,..,(*,,,,,,,,,,,,,,,,,,,,,**,,,,,,,,,,,,,,..,/*,,,**...,.(@@@&#.%@@@@@@\n"
# define MOTD_GIRL08 "@@@@@@@/*(,,...*,,*,,,/*,,/,,.,,/,,,,,,,,,,,,,,,,,,,,,,/,,,,,,,,,,,,,,,,..,//,,,*,.....#@@@@/*@@@@@@\n"
# define MOTD_GIRL09 "@@@@@@%(#*,..,,*,,,,,/,,,(,,,,,*/,,,,,,,,,,,,,,,,,,,,,,/,,,,,,,,,,,,,,,,,,,,*/,,,*,...,*#@@@@(#&@@@@\n"
# define MOTD_GIRL10 "@@@@@%#(,*,*,,,,,,,,/*,,/,,,,,,*/,,,,,,,,,,,,,,,,,,,,,,/,,,,,,,,,,,,,,,,,,,,,/*,,,,*,,,**%@@@@/(#@@@\n"
# define MOTD_GIRL11 "%&@&/#(,*,,,,,,,,,,*/,,,/,,,,,,,(,,,,,,,,,,,,,,,,,,,,,,/*,,,,,,,,,,,,,,,,,,,,,(,,,,,,,,,*,%@@@@#%@@@\n"
# define MOTD_GIRL12 "%%@%/(**,*/,,,,,,,,(,,,,*,,,,,,,&*,,,,,,,,,,,,,,,,,,,,,*/,,,,,,,**,,,,,,,,,,,,,(,,,,,,,,,,*/@@@@%((#\n"
# define MOTD_GIRL13 "%&%#%*,,,/*,,,,,,,#/,,,,/,,,,,,**/,,,*,,,,,,,,,,,,,,,,,*#,,,,,,,,#/,,,,,,,,,,,*(*,,,,,,,,,***&@@@@#(\n"
# define MOTD_GIRL14 "%&%&,,,*,/*,,,,,,*#*,,,//*,,,,,//***,**,,,,,,**,,,,,,,,,/(,,,,,,,((#,,,,(*,,,,*//,,,,,,,,*(,,,#@@@&%\n"
# define MOTD_GIRL15 "&%&*,,,,*/,,,,,,,(/,,,#. **,,,,*/ ..(,*/.,,,,,(,,,,,,,,,**./,,,,,**.,(*,,/,,,,*,/,,,,,,,,,(,,,,/*,/&\n"
# define MOTD_GIRL16 "/((,,,,,*/,,,,,,*(/,/.   . **,,** . . **(,,,,,,(#,,,,,,,,/, ./,,,,*,.  /***,,,/,**,,,,,,,,(,,,**/,*/\n"
# define MOTD_GIRL17 "*#*,,,,,*/,,,,,,*(/* .    .  .(*/ ...   . *(,,*,***(,*,,*,/..  ,/*,/. .  ,**,//,**,,,,,,,,(,,,,,/***\n"
# define MOTD_GIRL18 "//,,,,,,*/,,,,,,(*   .    . .  ...   .  ..   .*/**(, .****,*,    . ,/.   **/.  **/,,,,,,,*(,,,,,,(,*\n"
# define MOTD_GIRL19 "/*,,,,,,,/,,,,,,*                   ..        ..    ....    ..   ..  ... ,.    ..*,,,,,,,/,,,,,,,*/,\n"
# define MOTD_GIRL20 "/,,,,,,,,**,,,,*,            . .   .           ..  ..     ...    ...  . .  .   ..*,,,,,,**,,,,,,,,/*\n"
# define MOTD_GIRL21 "***,,,,,,,(.,,,,,      .. .......   ,*, .               . .*,.   ........ .    ..*,,,,,,/,,*,,,*,,,/\n"
# define MOTD_GIRL22 "*,,,,,,,,,//,,,*.  . .*%&@@@@@@@@@&#*. .. .        ....    .*%&@@@@@@@@@&#%(.. ..*,,,,,**,,,,,,,,***\n"
# define MOTD_GIRL23 "/*,,,,,,,,,(*,,*,./&/*/((&&&&&&%%%&%(//%&* .        .  .(%(/######%%%%%%%(#(/*#@**,,,,*/,,,,,,,,,(,*\n"
# define MOTD_GIRL24 "*/,,,,,,,,,,/,,**     . ........... ....... ... . .     . ..  ........ ....     .*,,,*/,,,,,,,,,/*,,\n"
# define MOTD_GIRL25 ",/**,,,*,,,,*/,** . .. . .            ...   ..,.              .. .. .        .. **,,*/,,,,,,,,,/**,*\n"
# define MOTD_GIRL26 ",*/*,,,,,,,,,*/,*. ..,. , .. , ,. ... ....              ... .. ,. , ., ,... ..  (,,,*,,,,,,,,*(,,***\n"
# define MOTD_GIRL27 "*,,(,*,,,,,,,,*(*.  . .   .   . . ....                  . ... . . .  .. . ..   /,,,(,,,,,,*,/*,,,,**\n"
# define MOTD_GIRL28 ",*,,(*,,,,,,,,,,/* . . . . . ..                         .    ....... . .   . .**,**,,,,,,,*(*,,*,*,*\n"
# define MOTD_GIRL29 ",*,,***,*,,,,,,,,(*     .      .  . ..... . ...   ........ . .              .,*,/*,,*,,,*/,,,*,,,***\n"
# define MOTD_GIRL30 ",**,,*,/*,,,,,,,,,,/.. ..      .  */.,,,,................,.,,#.             ,**/,,,,,,(*,,*,,*,,,***\n"
# define MOTD_GIRL31 ",**,****,/*,,,,,,,,,/.         .    ,*,                  .*,.            . **/*,,,,,(*,,,,,,,,*,*,*,\n"
# define MOTD_GIRL32 "***,**,,,,,*(*,,,,*,,,(.        ..  ...    ....,,,,...   ...  .       .. .,(*,,*,(*,,,*,,,,,,*,,****\n"
# define MOTD_GIRL33 "#/,*,,,*,,,,*,,(/,,,,,,*,      .  ..    ..   .   .  .       .          . (/,*,*(,,*,,,,,,,**,******/\n"
# define MOTD_GIRL34 "(((*********,,*,,,,*///,,(. .....  .. .    ... ...         .     .     ,(,,*/*,*,*,,,,,,******,***(#\n"
# define MOTD_GIRL35 "%((((*******,,,**,,,,*,,,*,((((/,.    ...         ...........    ,/((((,,(*,,**,,,,,,,,********/(((/\n"
# define MOTD_GIRL36 "(%#((((****,**,,**,,,,,,,,,,,%/(((/##((#&%#/**,....,*/(#%&&&@@(*(/(((#*/,,,,,,,,,,,**********/(((%#,\n"
# define MOTD_GIRL37 "((&((#(#(/********,**,,,,,,,*,*#((##%/#%/*,.,/%@@@@@@@%/,.,***,*/./&/(,,,*,,,,,,*,********,(((((%(/*\n"
# define MOTD_GIRL38 "(#%&%#(#(((#/************,***,,,/%(#(/,#(,**,,,,******,******((,******,*,,,*************(#((#(#%((/*\n"
# define MOTD_GIRL39 "###&&&%#(#(((((#/************,****## .***,/(*,***,***,,**(/*,*,**,(,****************(((#((#(#%(#((**\n"
# define MOTD_GIRL40 "&#(%@&&&&%####(##(#(((/*,*******,**/    .,****(*,***/(/*,*******.,/************/(##(#((#((%%(##(#/,#\n"

#endif

/*
Pass		x
Nick		x
User		x
Ping		x
Pong		x
Oper		x
Quit		x
Error		x
Join		x
Part		x
Topic		x
Names		x
List		x
Invite		x
Kick		x
MOTD		x
Version		x
Admin		x
Lusers		x
Time		x
Info		x
Mode		x
Notice		x
Who			x
Whois		x
Kill		x
Away		x
Wallops		x


User Modes
Away		x
Time		x
-Invisi +i	x
-Oper +o	x
-Wallop +w	x

Channel Mode
Time		x
-Ban +b		x
-Limit +l	x
-Invite +i	x
-Key +k		x
-Moder +m	x
-Secret +s	x
-Protect +t	x
-Extern +n	x

Channel member
-Founder +q (~)		x
-Operator +o (@)	x
-Voice +v (+)		x


AWAYLEN=100				x
CASEMAPPING=ascii		x
CHANLIMIT=#&:10			x
CHANMODES=b,k,l,imstn	x
CHANNELLEN=20			x
HOSTLEN=64				x
KICKLEN=100				x
NICKLEN=20				x
PREFIX=~@+				x
TOPICLEN=100			x
USERLEN=20				x

* o ? ¿Implementar?

*/ 
