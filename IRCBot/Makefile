SRCS	= main.cpp Bot.cpp BotExecute.cpp Parser.cpp

OBJS	= ${SRCS:.cpp=.o}

NAME	= ircbot

CXX	= c++

RM	= rm -f

CXXFLAGS	= -Wall -Wextra -Werror -std=c++98 #-fsanitize=address -g3

.cpp.o:
		${CXX} ${CXXFLAGS} -c $< -o ${<:.cpp=.o}

${NAME}:	${OBJS}
				${CXX} ${CXXFLAGS} ${OBJS} -o ${NAME}

all:		${NAME}

clean:
				${RM} ${OBJBS} ${OBJS}

fclean: 	clean
				${RM} ${NAME}

re: 		fclean all

.PHONY:		all clean fclean re
