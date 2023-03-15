# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: estarck <estarck@student.42mulhouse.fr>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/11/10 15:31:10 by estarck           #+#    #+#              #
#    Updated: 2023/03/15 14:25:41 by estarck          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		=	webserv

INCLUDES	=	-I./include

SDIR		=	./srcs
_SRCS		=	main.cpp \
				ParsConfig.cpp \
				Server.cpp \
				Connection.cpp
SRCS		=	$(patsubst %,$(SDIR)/%,$(_SRCS))

ODIR		=	./bin
_OBJS		=	$(_SRCS:.cpp=.o)
OBJS		=	$(patsubst %,$(ODIR)/%,$(_OBJS))

# Commands bash
RM			=	rm -f
MKODIR		=	if [ -d $(ODIR) ]; then :; else mkdir $(ODIR); fi
RMODIR		=	if [ -d $(ODIR) ]; then rm -rf $(ODIR); else :; fi

# Message
DELOBJS		=	printf "$(_YELLOW)Deleting .o files\n$(_END)"
DELNAME		=	printf "$(_RED)$(_BOLD)Remove $(NAME)\n$(_END)"

# Compilation
CC			=	c++
FLAGS		=	-Wall -Wextra -Werror -std=c++98
FLAGS		+=	-g

# Formatting
_RETURN		=	$'\33[2K\r
_END		=	$'\x1b[0m
_BOLD		=	$'\x1b[1m
_UNDER		=	$'\x1b[4m
_REV		=	$'\x1b[7m

# Colors
_GREY		=	$'\x1b[30m
_RED		=	$'\x1b[31m
_GREEN		=	$'\x1b[32m
_YELLOW		=	$'\x1b[33m
_BLUE		=	$'\x1b[34m
_PURPLE		=	$'\x1b[35m
_CYAN		=	$'\x1b[36m
_WHITE		=	$'\x1b[37m

# **************************************************************************** #

all			:	$(NAME)

$(NAME)		:	$(OBJS)
				@$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
				@printf "\n$(_GREEN)$(_BOLD)$(NAME) is compiled.$(_END)\n"

$(ODIR)/%.o	:	$(SDIR)/%.cpp
				@$(MKODIR)
				@printf "$(_RETURN)$(_BLUE)Compiling $(NAME) in progress... $< $(_END)"
				@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean		:
				@if [ -d $(ODIR) ]; \
					then $(RM) $(OBJS) && $(DELOBJS) && $(RMODIR); \
					else printf "$(_GREEN)Already clean .o files\n$(_END)"; \
				fi

fclean		:	clean
				@if [ -f $(NAME) ]; \
					then $(RM) $(NAME) && $(DELNAME); \
					else printf "$(_GREEN)Already remove $(NAME)$(_END)\n"; \
				fi

re			: fclean all

.PHONY		: all clean fclean re