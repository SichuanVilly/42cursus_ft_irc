SRCS	= main.cpp Server.cpp User.cpp Channel.cpp ServerUtils.cpp ServerCmds.cpp \
		ServerMode.cpp Parser.cpp ServerExecute.cpp

OBJS	= ${SRCS:.cpp=.o}

NAME	= ircserv

CXX	= c++

RM	= rm -f

CXXFLAGS	= -Wall -Wextra -Werror -std=c++98 #-fsanitize=address -g3

PATHBOT = IRCBot/

.cpp.o:
		${CXX} ${CXXFLAGS} -c $< -o ${<:.cpp=.o}

${NAME}:	${OBJS}
				${CXX} ${CXXFLAGS} ${OBJS} -o ${NAME}

bot:
				@make -C ${PATHBOT} 

all:		${NAME}

clean:
				@make fclean -C ${PATHBOT}
				${RM} ${OBJBS} ${OBJS}

fclean: 	clean
				@make fclean -C ${PATHBOT}
				${RM} ${NAME}

re: 		fclean all

.PHONY:		all clean fclean re
